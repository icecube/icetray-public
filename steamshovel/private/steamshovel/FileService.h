#ifndef I3_SHOVEL_FILE_SERVICE_H
#define I3_SHOVEL_FILE_SERVICE_H

#include "shovelio/I3FrameSequence.h"

#include <icetray/I3Logging.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Logging.h>
#include <dataclasses/geometry/I3Geometry.h>

#include <boost/python/object.hpp>

#include <QObject>
#include <QTimer>
#include <QString>
#include <QStringList>
#include <QMetaType>

#include <vector>

Q_DECLARE_METATYPE(I3FramePtr);
Q_DECLARE_METATYPE(I3GeometryConstPtr);

struct FrameInfo {
	unsigned idx_;
	I3Frame::Stream stream_;
	std::string sub_stream_;
	FrameInfo():
		idx_(),
		stream_(),
		sub_stream_()
	{}

	FrameInfo(unsigned idx,
	          const shovelio::I3FrameSequence::IndexItem& item):
		idx_(idx),
		stream_(item.stream_),
		sub_stream_(item.sub_stream_)
	{}
};

typedef std::vector<FrameInfo> FrameIndex;

class FileService : public QObject
{
	Q_OBJECT ;

	Q_PROPERTY( QStringList openFiles READ openFiles );
	Q_PROPERTY( unsigned currentIndex READ currentIndex );
	Q_PROPERTY( I3FramePtr currentFrame READ currentFrame );
	Q_PROPERTY( bool framesAreProcessed READ framesAreProcessed );
	unsigned nFrames() const { return index_.size(); }
	Q_PROPERTY( unsigned nFrames READ nFrames );
	Q_PROPERTY( I3GeometryConstPtr defaultGeometry READ defaultGeometry WRITE setDefaultGeometry );

	SET_LOGGER( "FileService" );

private:

	QTimer timer_;
	shovelio::I3FrameSequence frames_;
	FrameIndex index_;
	unsigned current_; // current position in user index
	unsigned frame_idx_; // current position in file index, see timerEvent
	I3FramePtr current_frame_;
	boost::python::object frame_filter_;
	I3GeometryConstPtr default_geo_;

protected Q_SLOTS:
	void updateIndex();

public:
	FileService( QObject* parent = 0 );
	~FileService();

	bool hasOpenFiles() const { return frames_.GetNFiles() > 0; }
	QStringList openFiles() const;

	const FrameIndex& frameIndex() const { return index_; }
	void saveFrames( QString, const std::set<unsigned>&, bool );

	unsigned currentIndex() { return current_; }
	I3FramePtr currentFrame() { return current_frame_; }

	void setFilters(QStringList lst);
	QStringList getFilters() const;

	void setNBackward(unsigned n) { frames_.SetNBackward(n); }
	void setNForward(unsigned n) { frames_.SetNForward(n); }

	unsigned getNBackward() const { return frames_.GetNBackward(); }
	unsigned getNForward() const { return frames_.GetNForward(); }

	bool framesAreProcessed(); // to be used by scripts

	I3GeometryConstPtr defaultGeometry() const { return default_geo_; }
	void setDefaultGeometry( I3GeometryConstPtr geo ){ default_geo_ = geo; }

public Q_SLOTS:
	void openLocalFile( const std::string& path );
	void openRemoteFile( const std::string& url );
	void closeAllFiles();
	void closeLastFile();
	void reloadAllFiles();

	bool selectFrame( unsigned i );
	bool advanceFrame( int delta );
	bool nextMatchingFrame( const I3Frame::Stream& fstop );
	bool prevMatchingFrame( const I3Frame::Stream& fstop );

	void setFrameFilter( boost::python::object );

Q_SIGNALS:
	void fileAdded( QString path, bool isSinglePass );
	void fileListChanged();
	void frameChanged( I3FramePtr frame, unsigned idx, const FrameInfo& );
	void newSegment( unsigned start, unsigned end, bool complete );
	void clearFrames();
};

#endif /* I3_SHOVEL_FILE_SERVICE_H */
