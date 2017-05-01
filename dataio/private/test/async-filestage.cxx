#include <I3Test.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Tray.h>
#include <dataio/I3FileStager.h>
#include <dataio/I3File.h>

TEST_GROUP(AsyncStaging)

boost::shared_ptr<I3FileStager> enable_file_staging(I3Tray& tray){
	auto pydataio = boost::python::import("icecube.dataio");
	boost::shared_ptr<I3FileStager> stager=
	  boost::python::extract<boost::shared_ptr<I3FileStager>>(pydataio.attr("get_stagers")());
	tray.GetContext().Put("I3FileStager",stager);
	return(stager);
}

TEST(AsyncStagedWrite){
	//use a compressed file to catch errors in closing/flushing filters!
	const std::string file=I3Test::testfile("aswrite.i3.gz");
	const unsigned int nFrames=10;
	{ //write via staging
		I3Tray tray;
		auto file_stager=enable_file_staging(tray);
		tray.AddModule("I3InfiniteSource")("Stream",I3Frame::Physics);
		tray.AddModule("I3AsyncWriter")
		  ("Filename","file://"+file) //use the local file stager
		  ("Streams",std::vector<I3Frame::Stream>{I3Frame::Physics});
		tray.Execute(nFrames);
	}
	{ //check that the whole file got staged out
		I3Tray tray;
		tray.AddModule("I3Reader")("Filename",file);
		unsigned int read=0;
		tray.AddModule([&](boost::shared_ptr<I3Frame> f){ read++; });
		tray.Execute();
		log_info_stream("Read " << read << " frames");
		ENSURE_EQUAL(read,nFrames);
	}
	std::remove(file.c_str());
}

TEST(AsyncStagedRead){
	const std::string file=I3Test::testfile("asread.i3.gz");
	const unsigned int nFrames=10;
	{ //generate a file
		I3Tray tray;
		tray.AddModule("I3InfiniteSource")("Stream",I3Frame::Physics);
		tray.AddModule("I3Writer")("Filename",file);
		tray.Execute(nFrames);
	}
	{ //ensure that we can read the file file when it is staged in
		I3Tray tray;
		auto file_stager=enable_file_staging(tray);
		tray.AddModule("I3AsyncReader")
		  ("Filename","file://"+file); //use the local file stager
		unsigned int read=0;
		tray.AddModule([&](boost::shared_ptr<I3Frame> f){ read++; });
		tray.Execute();
		ENSURE_EQUAL(read,nFrames);
	}
	std::remove(file.c_str());
}
