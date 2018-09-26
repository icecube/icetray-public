#include "scripting/gil.h"
#include "FileService.h"
#include "moc_FileService.cpp"

#include <dataio/I3FileStager.h>
#include <icetray/open.h>
#include <icetray/serialization.h>

#include <QTimerEvent>
#include <QSettings>
#include <QByteArray>
#include <QCoreApplication>

#include <boost/python.hpp>
#include <boost/foreach.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/constants.hpp>

#include <algorithm>
#include <set>

namespace boost { namespace iostreams {
	// Bidirectional device based on QByteArray
	struct QIOStream {
		typedef char char_type;
		typedef bidirectional_device_tag category;
		QByteArray& array_;
		std::streamsize pos_;
		QIOStream( QByteArray& a ) :
			array_( a ),
			pos_(0)
		{}
		std::streamsize write( const char* s, std::streamsize n ){
			array_.append( s, n );
			return n;
		}
		std::streamsize read( char* s, std::streamsize n ){ 
			const char* c = array_.constData() + pos_;
			n = std::min( std::streamsize(array_.size()) - pos_, n );
			// boost::iostreams indicates eof by return value -1
			if (n == 0) return -1;
			std::copy( c, c + n, s );
			pos_ += n;
			return n;
		}
	};
}}

namespace {

	class StagingSystem{
		I3FileStagerPtr stager_;
		std::vector< I3::dataio::shared_filehandle > files_;

	public:
		StagingSystem(){
			using namespace boost::python;
			scripting::ScopedGIL gil;
			try{
				object dataio = import( "icecube.dataio" );
				object result = dataio.attr( "get_stagers" )();
				stager_ = extract< I3FileStagerPtr >(result);
			}
			catch( error_already_set& ){
				PyErr_Print();
				throw std::runtime_error( "Python error while creating I3FileStager" );
			}
		}

		I3::dataio::shared_filehandle fetch( const std::string& url ){
			I3::dataio::shared_filehandle handle =
			  stager_->GetReadablePath( url );
			files_.push_back( handle ); // keep handle alive
			return handle;
		}
	};

	I3GeometryConstPtr loadGeometry( QByteArray array ){
		namespace io = boost::iostreams;
		namespace ar = icecube::archive;
		if( array.isEmpty() ) return I3GeometryConstPtr();
		io::filtering_istream is;
		is.push( io::gzip_decompressor() );
		is.push( io::QIOStream( array ) );
		ar::portable_binary_iarchive ia( is );
		I3Geometry* geo = new I3Geometry;
		ia & *geo;
		return I3GeometryConstPtr( geo );
	}

	QByteArray saveGeometry( I3GeometryConstPtr geo ){
		namespace io = boost::iostreams;
		namespace ar = icecube::archive;
		QByteArray array;
		if( !geo ) return array;
		using namespace boost::iostreams;
		io::filtering_ostream os;
		os.push( io::gzip_compressor( 9 ) );
		os.push( io::QIOStream( array ) );
		ar::portable_binary_oarchive oa( os );
		oa & *geo;
		return array;
	}

} // namespace

FileService::FileService( QObject* parent ):
	QObject( parent ),
	current_( -1 ),
	frame_idx_( 0 )
{
	QSettings settings;
	setNForward(
	    settings.value( "frameio/cache_nforward", 5 ).toUInt() );
	setNBackward(
	    settings.value( "frameio/cache_nbackward", 5 ).toUInt() );
	setFilters(
	    settings.value( "frameio/ignored_keys" ).toStringList() );
	default_geo_ = loadGeometry(
	    settings.value( "frameio/default_geometry" ).toByteArray()
	);

	timer_.setSingleShot( true );
	connect( &timer_, SIGNAL(timeout()), SLOT(updateIndex()) );
}

FileService::~FileService(){
	QSettings settings;
	settings.setValue( "frameio/cache_nforward", getNForward() );
	settings.setValue( "frameio/cache_nbackward", getNBackward() );
	settings.setValue( "frameio/ignored_keys", getFilters() );
	settings.setValue( "frameio/default_geometry", saveGeometry( default_geo_ ) );
}

QStringList FileService::openFiles() const {
	QStringList lst;
	for( unsigned i = 0; i < frames_.GetNFiles(); ++i )
		lst.push_back( QString::fromStdString(frames_.GetPath(i)) );
	return lst;
}

void FileService::saveFrames( QString filename,
                              const std::set<unsigned>& indices,
                              bool saveParents ){
	shovelio::I3FrameSequence::LockedIndex lindex( frames_ );

	std::set<unsigned> internal;
	typedef std::set<unsigned> iterator;
	BOOST_FOREACH( unsigned filtered_idx, indices ){
		// indices are locations in the filtered FileService index,
		// get corresponding locations in unfiltered I3FrameSequence::Index
		if( filtered_idx >= index_.size() )
			continue;
		unsigned idx = index_[filtered_idx].idx_;
		bool indexNotSeenBefore = internal.insert( idx ).second;

		if( saveParents ){
			// unwind stack of parents
			while( indexNotSeenBefore ){
				unsigned pidx = lindex[idx].parent_;
				if( pidx == idx )
					break; // we arrived at the root node
				idx = pidx;
				indexNotSeenBefore = internal.insert( idx ).second;
			}
		}
	}

	// save the frames
	boost::iostreams::filtering_ostream ofs;
	I3::dataio::open( ofs, filename.toStdString() );
	BOOST_FOREACH( unsigned idx, internal )
		frames_[idx]->save(ofs);
	ofs.reset();
}

void FileService::setFilters( QStringList filters ){
	using shovelio::raw::FilterList;
	FilterList f_lst;
	BOOST_FOREACH( QString s, filters )
		f_lst.push_back( s.toStdString() );
	frames_.SetFilter( f_lst );
}

QStringList FileService::getFilters() const {
	using shovelio::raw::FilterList;
	const FilterList& f_lst = frames_.GetFilter();
	QStringList lst;
	BOOST_FOREACH( const std::string& s, f_lst )
		lst.push_back( QString::fromStdString(s) );
	return lst;
}

bool FileService::framesAreProcessed() {
	return frames_.IsIndexing() ||
	       frame_idx_ < frames_.GetSize();
}

void FileService::setFrameFilter( boost::python::object filter ){
	Q_EMIT clearFrames();
	frame_filter_ = filter;
	frame_idx_ = 0;
	current_ = -1;
	current_frame_.reset();
	index_.clear();
	Q_EMIT frameChanged( current_frame_, current_, FrameInfo() );
	updateIndex();
}

void FileService::openLocalFile( const std::string& path ){
	log_debug_stream( "openLocalFile on " << path );

	using shovelio::raw::File;
	const File::Type t = frames_.AddFile( path );
	Q_EMIT fileAdded( QString::fromStdString(path), t == File::SinglePass );
	Q_EMIT fileListChanged();
	updateIndex();
}

void FileService::openRemoteFile( const std::string& url ){
	log_debug_stream( "openRemoteFile on " << url );

	static StagingSystem stagingSystem;
	I3::dataio::shared_filehandle handle = stagingSystem.fetch( url );
	log_debug_stream( "Read url " << url << " to local file " << *handle );
	openLocalFile( *handle );
}

void FileService::closeAllFiles(){
	log_debug_stream( "closeAllFiles called" );
	timer_.stop();
	frame_idx_ = 0;
	current_ = unsigned(-1);
	current_frame_.reset();
	index_.clear();
	frames_.CloseAllFiles();
	Q_EMIT fileListChanged();
	Q_EMIT clearFrames();
	Q_EMIT frameChanged( current_frame_, current_, FrameInfo() );
}

void FileService::reloadAllFiles(){
	QStringList filenames = openFiles();
	closeAllFiles();	
	// re-open files
	BOOST_FOREACH( QString filename, filenames )
#if QT_VERSION >= 0x050000
		openLocalFile( filename.toLatin1().data() );
#else
		openLocalFile( filename.toAscii().data() );
#endif
}

void FileService::closeLastFile(){
	log_debug_stream( "closeLastFile called" );
	frames_.CloseLastFile();
	shovelio::I3FrameSequence::LockedIndex lindex( frames_ );
	const unsigned frames_end = lindex.GetSize();

	if( frame_idx_ > frames_end ){
		frame_idx_ = frames_end;
	}

	for( FrameIndex::iterator
	       iter = index_.begin(), end = index_.end();
	     iter != end; ++iter ){
		if( iter->idx_ >= frames_end ){
			index_.erase( iter, end );
			current_ = unsigned(-1);
			current_frame_.reset();
			Q_EMIT frameChanged( current_frame_, current_, FrameInfo() );
			break;
		}
	}

	Q_EMIT fileListChanged();
	Q_EMIT clearFrames();
	if( hasOpenFiles() ){
		Q_EMIT newSegment( 0, index_.size(), false );
		updateIndex();
	}
}

bool FileService::selectFrame( unsigned i ){
	if( i >= index_.size() )
		return false;

	current_ = i;
	const FrameInfo& finfo = index_[i];
	log_trace("selecting frame: user idx = %u, file idx = %u", i, finfo.idx_);

	current_frame_ = frames_[finfo.idx_];

	// insert default I3Geometry if it is missing
	const std::string geo_key = I3DefaultName<I3Geometry>::value();
	if( default_geo_ &&
	    current_frame_->GetStop() != I3Frame::TrayInfo &&
	    !current_frame_->Has( geo_key ) )
		current_frame_->Put( geo_key, default_geo_ );

	Q_EMIT frameChanged( current_frame_, current_, finfo );
	return true;
}

bool FileService::advanceFrame( int delta ){
	return selectFrame( current_ + delta );
}

bool FileService::nextMatchingFrame( const I3Frame::Stream& fstop ){
	if( current_ >= index_.size() )
		return false;
	unsigned i = current_ + 1;
	for( ; i < index_.size() &&
	        index_[i].stream_ != fstop ;
	     ++i );
	// selectFrame does nothing if idx is out of range
	return selectFrame( i );
}

bool FileService::prevMatchingFrame( const I3Frame::Stream& fstop ){
	if( current_ == 0 )
		return false;
	unsigned i = std::min( current_, unsigned(index_.size() - 1) ) - 1;
	for( ; i < index_.size() &&
	         index_[i].stream_ != fstop ;
	     --i );
	// selectFrame does nothing if idx is out of range
	return selectFrame( i );
}

void FileService::updateIndex(){
	using namespace boost::python;
	using namespace shovelio;

	if( !hasOpenFiles() )
		return;

	const bool index_complete = !frames_.IsIndexing();
	const unsigned start = index_.size();
	const unsigned nchunk = frame_filter_ ? 32 : 256;
	unsigned frames_end = 0;

	{ 
		I3FrameSequence::LockedIndex lindex( frames_ );
		frames_end = lindex.GetSize();
	
		if( frame_filter_ ){
			if( !index_complete ){
				// tell the world we started working here, but let internal
				// file indexing finish before starting frame filtering; this
				// prevents random seeking in the file which may be very slow
				Q_EMIT newSegment( 0, 0, false );
				timer_.start( 500 );
				return;
			}
		} else {
			index_.reserve(frames_end);
		}

		for( unsigned c = frame_idx_;
			 (frame_idx_ - c) < nchunk && frame_idx_ < frames_end;
			 ++frame_idx_ ){
			const I3FrameSequence::IndexItem& item = lindex[frame_idx_];
			if( frame_filter_ ){
				scripting::ScopedGIL gil;
				try{
					object result = frame_filter_( frame_idx_, frames_[frame_idx_] );
					if( extract<bool>( result )() )
						index_.push_back( FrameInfo(frame_idx_, item) );
				}catch( error_already_set& e ){
					PyErr_Clear();
				}
			} else {
				index_.push_back(
				    FrameInfo( frame_idx_, item ) 
				);
			}
		}
	}

	const bool complete = index_complete && frame_idx_ == frames_end;
	Q_EMIT newSegment( start, index_.size(), complete );
	QCoreApplication::processEvents(); // allow GUI interactivity

	if( !complete ){
		if( !frame_filter_ && (index_.size() - start) < nchunk )
			timer_.start( 500 );
		else
			updateIndex();
	}
}
