/**
 *  $Id$
 *  
 *  Copyright (C) 2011
 *  The IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 */

#include <icetray/open.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/scoped_ptr.hpp>

#include "dataio/I3FileStager.h"

class I3InfiniteSource : public I3Module
{
  public:
	I3InfiniteSource(const I3Context& context);
	void Configure();
	void Process();

  private:
	I3FileStagerPtr file_stager_;
	I3::dataio::shared_filehandle current_filename_;
	
	boost::iostreams::filtering_istream ifs_;
	I3Frame::Stream stream_;
        SET_LOGGER("I3InfiniteSource");
};

I3_MODULE(I3InfiniteSource);

I3InfiniteSource::I3InfiniteSource(const I3Context& context)
    : I3Module(context), stream_(I3Frame::DAQ)
{

	AddParameter("Stream", "Type of frame to emit", stream_);
	AddParameter("Prefix", "Path to I3 file with frames to prefix to "
	    "the stream (e.g. GCD information)", "");

	AddOutBox("OutBox");
}

void I3InfiniteSource::Configure()
{
	std::string filename;

	GetParameter("Stream", stream_);
	GetParameter("Prefix", filename);
	if (filename.length() > 0) {
		log_info("Prefixing with filename/url %s", filename.c_str());

		file_stager_ = context_.Get<I3FileStagerPtr>();
		if (!file_stager_) {
			file_stager_ = I3TrivialFileStager::create();
		}
		file_stager_->WillReadLater(filename);
		current_filename_ = file_stager_->GetReadablePath(filename);

		I3::dataio::open(ifs_, *current_filename_);

		log_debug("Opened file %s", current_filename_->c_str());
	}
}

void I3InfiniteSource::Process()
{
	log_trace("%s: %s", GetName().c_str(), __PRETTY_FUNCTION__);
	
	if(PeekFrame())
		log_fatal("I3InfiniteSource should only be used as a driving module");

	I3FramePtr frame(new I3Frame(stream_));
	if (!ifs_.empty()) {
		// Get frame from prefix file
		frame->load(ifs_);
		if (ifs_.peek() == EOF) {
			// we will never read anything again. release all the pointers.
			ifs_.reset();
			current_filename_.reset();
			file_stager_.reset();
		}
	}

	PushFrame(frame);
}

