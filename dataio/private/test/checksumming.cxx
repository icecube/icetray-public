/**
    $Id$
    checksum tests.
*/

#include <I3Test.h>
#include <fstream>
#include <icetray/serialization.h>

#include <icetray/I3Logging.h>

#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <icetray/I3Bool.h>
#include <dataclasses/I3Double.h>
#include <dataclasses/physics/I3Particle.h>
#include <icetray/I3Frame.h>
#include <icetray/open.h>
#include <icetray/I3TrayInfo.h>
#include <boost/preprocessor.hpp>
#include <boost/lexical_cast.hpp>

using namespace icecube::archive;
using namespace std;
using namespace boost::iostreams;

TEST_GROUP(checksumming);

void test(I3Frame& frame, const std::string& filename)
{
  unlink(filename.c_str());

  cout << filename << ":\n" << frame << "\n";
  {
    std::ofstream ofs(filename.c_str(), ios::binary);
    frame.save(ofs);
  }
  {
    std::ifstream ifs(filename.c_str(), ios::binary);
    vector<string> skip;
    I3FramePtr newframe(new I3Frame);
    newframe->load(ifs);
  }

  unlink(filename.c_str());
}

void loadframes(const std::string &where, unsigned nframes)
{
  vector<std::streampos> frame_positions;
  {
    filtering_istream ifs;
    I3::dataio::open(ifs, where);
    for (unsigned i=0; i<nframes; i++)
      {
	I3FramePtr newframe(new I3Frame);
	newframe->load(ifs);
	cout << *newframe << "\n";
	cout << "stream size=" << ifs.size() << "\n";
	void* stdifs = ifs.component<void>(ifs.size()-1);
	cout << "stdifs is at " << stdifs << "\n";
      }
  }
}

TEST(a_empty_frame)
{
  I3Frame frame;
  test(frame, "empty_frame.i3");
}

TEST(b_frame_plus_bool)
{
  I3BoolPtr bp(new I3Bool);
  bp->value = false;
  I3Frame fr;
  fr.Put("abool", bp);
  ENSURE(fr.Has("abool"));
  test(fr, "one_i3bool.i3");
}

TEST(c_hundred_doubles)
{
  I3Frame fr;
  for (double d=0; d<100; d++)
    {
      I3DoublePtr i3dp = I3DoublePtr(new I3Double(d));
      fr.Put(boost::lexical_cast<string>(d), I3DoublePtr(new I3Double(d)));
    }
  test(fr, "hundred_doubles.i3");
}

TEST(d_trayinfo)
{
  I3Frame fr;
  I3TrayInfoPtr tip(new I3TrayInfo);
  fr.Put(tip);
  test(fr, "empty_trayinfo.i3");

  tip->host_info["dawg"] = "cats!";
  tip->svn_externals = ":DLKFA:LDKFJA:LDFKJA:LDFKJA:DLFKJA:DLFKJA:DFLKAD:LFKJAD:LFKJADF:LKJDF";
  test(fr, "nonempty_trayinfo.i3");
}

TEST(fail_reading_truncated_file)
{
  //
  //  Back in V3 we had checksumming for a bit there, but it was messy
  //  and it missed certain objects.  with v4 we have clean
  //  checksumming, but not when loading v3 files.
  //
  EXPECT_THROW(loadframes(string(getenv("I3_SRC")) 
			  + "/dataio/resources/data/serialization/truncated_hundred_doubles-v4.i3.gz",1),
	       "This should throw.");
}  
  
TEST(fail_reading_corrupted_file)
{
  EXPECT_THROW(loadframes(string(getenv("I3_SRC"))
			  + "/dataio/resources/data/serialization/corrupt_hundred_doubles-v4.i3.gz",1),
	       "This should throw.");
}

