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

class I3InfiniteSource : public I3Module
{
  public:
	I3InfiniteSource(const I3Context& context);
	void Configure();
	void Process();

  private:
	boost::iostreams::filtering_istream ifs_;
	I3Frame::Stream stream_;
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
		log_info("Prefixing with filename %s", filename.c_str());
		I3::dataio::open(ifs_, filename);
	}
}

void I3InfiniteSource::Process()
{
	log_trace("%s: %s", GetName().c_str(), __PRETTY_FUNCTION__);

	I3FramePtr frame(new I3Frame(stream_));
	if (!ifs_.empty()) {
		// Get frame from prefix file
		frame->load(ifs_);
		if (ifs_.peek() == EOF)
			ifs_.reset();
	}

	PushFrame(frame);
}

