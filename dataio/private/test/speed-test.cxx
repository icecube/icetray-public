/**
   $Id$
   speed tests.
*/

#define I3_I3FRAME_TESTING 1
#include <I3Test.h>
#include <fstream>
#include <icetray/serialization.h>

#include <icetray/I3Logging.h>

#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <icetray/I3Bool.h>
#include <dataclasses/I3Double.h>
#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/physics/I3Waveform.h>
#include <icetray/I3Frame.h>
#include <icetray/open.h>
#include <icetray/I3TrayInfo.h>
#include <boost/preprocessor.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace icecube::archive;
using namespace std;
using namespace boost::iostreams;

namespace dataio = I3::dataio;

TEST_GROUP(performance);
I3_SERIALIZABLE(I3Vector<I3Vector<char> >);

namespace ar = icecube::archive;


//
//  Make an I3Frame full of stuff
//
I3FramePtr make_frame()
{
  I3FramePtr fr(new I3Frame);
  for (double d=0; d<10; d++)
    {
      I3DoublePtr i3dp(new I3Double(d));
      fr->Put(boost::lexical_cast<string>(d)+ "double", i3dp);
      I3WaveformSeriesMapPtr wf(new I3WaveformSeriesMap);
      for (int st=0; st<80; st++)
	for (int om=0; om<60; om++)
	  (*wf)[OMKey(st, om)].resize(1);
      fr->Put(boost::lexical_cast<string>(d)+"waveform", wf);
    }

  boost::shared_ptr<I3Vector<I3Vector<char> > > vvchp(new I3Vector<I3Vector<char> >);
  vvchp->resize(100);
  BOOST_FOREACH(I3Vector<char>& vcr, *vvchp)
    {
      vcr.resize(10000);
    }
  fr->Put("blob", vvchp);
  return fr;
}

// 
//  checks one slot for an object, verifies deserialization works.
//  Get<I3FrameObject> is sufficient to trigger full deserialization.
//
void
check_slot(const I3Frame& frame, const std::string& what)
{
  ENSURE(!frame.has_ptr(what));
  ENSURE(frame.has_blob(what));
  frame.Get<I3FrameObject>(what);
  ENSURE(frame.has_ptr(what));
}

//
//  Looks at each of the objects in the frame created by make_frame.
//  This is to trigger deserialization.
//
void inspect_frame(const I3Frame& frame)
{
  for (double d=0; d<10; d++)
    {
      check_slot(frame, boost::lexical_cast<string>(d)+ "double");
      check_slot(frame, boost::lexical_cast<string>(d)+ "waveform");
    }
  check_slot(frame, "blob");
}

void test_iostreams(I3Frame& frame, const std::string& filename, unsigned ntimes)
{

#ifndef NDEBUG
  cout << "***\n***  Test not built optimized, will be slow.\n***\n";
#endif

  unlink(filename.c_str());

  //
  //  Writing test
  //
  time_t start = time(0);
  {
    cout << "Frame with " << frame.size() << " elements." << endl;
    boost::iostreams::filtering_ostream ofs;
    dataio::open(ofs, filename, 0);

    for (unsigned i=0; i<ntimes; i++)
      frame.save(ofs);

  }
  time_t stop = time(0);
  struct stat ss;
  stat(filename.c_str(), &ss);
  double megs = 1024*1024;


  cout << "\n  Frame written " << ntimes 
       << " times (" << ss.st_size << " bytes) in " << stop-start << " seconds. "
       << " (" << ((stop > start) ? ss.st_size/(stop-start)/megs : std::numeric_limits<double>::max()) << " Mb/sec)" << endl;

  //
  //  Reading test w/o full deserialization
  //
  start = time(0);
  {
    boost::iostreams::filtering_istream ifs;
    dataio::open(ifs, filename);
    for (unsigned i=0; i<ntimes; i++)
      {
	I3Frame f;
	f.load(ifs);
      }
  }
  stop = time(0);
  cout << "\n  Frame read (but contents not inspected) " << ntimes 
       << " times (" << ss.st_size << " bytes) in " << stop-start << " seconds. "
       << " (" << ((stop > start) ? ss.st_size/(stop-start)/megs : std::numeric_limits<double>::max()) << " Mb/sec)" << endl;

  //
  //  Reading test with full deserialization
  //
  start = time(0);
  {
    boost::iostreams::filtering_istream ifs;
    dataio::open(ifs, filename);
    for (unsigned i=0; i<ntimes; i++)
      {
	I3Frame f;
	f.load(ifs);
	inspect_frame(f);
      }
  }
  stop = time(0);
  cout << "\n  Frame read and contents inspected " << ntimes 
       << " times (" << ss.st_size << " bytes) in " << stop-start << " seconds. "
       << " (" << ((stop > start) ? ss.st_size/(stop-start)/megs : std::numeric_limits<double>::max()) << " Mb/sec)" << endl;

  unlink(filename.c_str());
}

TEST(d_fiftyevents)
{
  I3FramePtr fr = make_frame();
#ifdef NDEBUG
  test_iostreams(*fr, "polesimulation.i3", 250);
#else
  test_iostreams(*fr, "polesimulation.i3", 3);
#endif
}
