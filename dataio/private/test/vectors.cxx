/**
   copyright  (C) 2004
   the icecube collaboration
   $Id$

   @version $Revision$
   @date $Date$

*/

#include <I3Test.h>
#include <fstream>
#include <icetray/serialization.h>
#include <dataclasses/I3Vector.h>

#include <icetray/I3Logging.h>
#include <icetray/open.h>
#include <icetray/Utility.h>

#include "serialization-test.h"
#include "dataio-test.h"

#include <boost/preprocessor.hpp>
#include <boost/foreach.hpp>

using namespace icecube::archive;
using namespace std;

TEST_GROUP(vectors);

I3FramePtr load_i3_file(std::string fname)
{
  boost::iostreams::filtering_istream ifs;
  I3::dataio::open(ifs, fname);
  I3FramePtr f(new I3Frame);
  f->load(ifs);
  return f;
}

template <class T>
void do_vector (std::string classname)
{
  log_debug("writing");
  string fname = string("testvector") + classname + ".i3";
  boost::shared_ptr<I3Vector<T> > t_out(new I3Vector<T>);
  t_out->resize(100);
  for (unsigned i=0; i<100; i++)
    (*t_out)[i] = i;
  save_to_i3_file(t_out, fname);

  log_debug("reading");
  I3FramePtr frm = load_i3_file(fname);
  const I3Vector<T>& v = frm->template Get<I3Vector<T> >("object");
  for (unsigned i=0; i<100; i++)
    ENSURE_EQUAL(v[i], T(i));

  log_debug("reading past versions");
  string src = GetDataDir();
  vector<string> i3files;
  string globstr = src + "/serialization/*/" + fname;
  log_debug("globbing '%s'", globstr.c_str());
  glob(globstr.c_str(), i3files);
  ENSURE(i3files.size() != 0);
  BOOST_FOREACH(const string& s, i3files)
    {
      log_info("%s", s.c_str());
      I3FramePtr fp = load_i3_file(s);
      ENSURE((bool)fp);
      cout << "From " << s << ":\n" << *fp << "\n";
      const I3Vector<T>& vd = fp->template Get<I3Vector<T> >("object");
      for (unsigned i=0; i<100; i++)
	ENSURE_EQUAL(vd[i], T(i));
    }

  remove(fname.c_str());
}

//
//  Clean Me Up: These tests should be done via the I3VectorWhatever
//  typedefs in I3Vector.h... not stdint typedefs like uint64_t, as
//  e.g. I3VectorInt64 might be I3Vector<long>, but int64_t might be
//  long long.  Although the sizes of the types are the same, "long"
//  and "long long" are different types for the purposes of
//  I3_SERIALIZABLE (ie BOOST_CLASS_EXPORT, and therefore registration
//  of serialization methods by std::type_info*)
//

TEST(bool) { do_vector<bool>("bool"); }
TEST(char) { do_vector<char>("char"); }
TEST(uint16_t) { do_vector<uint16_t>("uint16_t"); }
TEST(int16_t) { do_vector<int16_t>("int16_t"); }
TEST(uint32_t) { do_vector<uint32_t>("uint32_t"); }
TEST(int32_t) { do_vector<int32_t>("int32_t"); }

#if __WORDSIZE == 64 || defined(_LP64)
typedef long myint64_t;
typedef unsigned long myuint64_t;
#else
typedef long long myint64_t;
typedef unsigned long long myuint64_t;
#endif

TEST(int64_t) {
  do_vector<myint64_t>("int64_t");
}
TEST(uint64_t) {
  do_vector<myuint64_t>("uint64_t");
}
TEST(double) { do_vector<double>("double"); }
TEST(float) { do_vector<float>("float"); }
