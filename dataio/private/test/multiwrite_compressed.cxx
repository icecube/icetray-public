// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <I3Test.h>
#include <icetray/I3Tray.h>
#include <icetray/open.h>
#include <dataio/I3File.h>

#include <boost/format.hpp>

#include <stdio.h> //for remove()

TEST_GROUP(multiwrite_compressed);

void do_multiwrite_test(const std::string& suffix, bool lenient=false){
	std::string filenameTemplate=I3Test::testfile("multi-%u.i3"+suffix);

	const unsigned int expectedFiles=4;

	GetIcetrayLogger()->SetLogLevel(I3LOG_INFO);
	I3Tray writerTray;
	writerTray.AddModule("I3InfiniteSource","Source");
	writerTray.AddModule("Dump","dump");
	writerTray.AddModule("I3MultiWriter","Writer")("Filename",filenameTemplate)
		("SizeLimit",10)("Streams",std::vector<I3Frame::Stream>{I3Frame::DAQ});
	writerTray.Execute(expectedFiles);
	writerTray.Finish();

	unsigned int frameCount=0;
	for(unsigned int i=0; i<expectedFiles; i++){
		boost::format f(filenameTemplate);
		f % i;
		try{
			dataio::I3File infile(f.str(),dataio::I3File::Mode::read);
			unsigned int fileFrameCount=0;
			while(infile.more()){
				infile.pop_frame();
				fileFrameCount++;
			}
			if(!fileFrameCount){
				log_fatal_stream(f.str() << " contains no readable frames");
			}
			frameCount+=fileFrameCount;
		}
		catch(std::runtime_error& err){
			if(!lenient){
				log_fatal_stream("Failed to read " << f.str() << ": " << err.what());
				FAIL("Read failed");
			}
			else{
				break;
			}
		}
	}
	ENSURE_EQUAL(expectedFiles,frameCount,"All frames must be read back");

	//clean up
	for(unsigned int i=0; i<expectedFiles; i++){
		boost::format f(filenameTemplate);
		f % i;
		::remove(f.str().c_str());
	}
}

TEST(multiwrite_plain){
	do_multiwrite_test("");
}
//These tests must be set to be lenient about the number of files generated because these
//filters do not flush correctly/completely.
TEST(multiwrite_gz){
	do_multiwrite_test(".gz", true);
}
TEST(multiwrite_bz2){
	do_multiwrite_test(".bz2", true);
}
#ifdef I3_WITH_ZSTD
TEST(multiwrite_zst){
	do_multiwrite_test(".zst");
}
#endif
