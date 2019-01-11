#ifndef SERIALIZATION_TEST_H_INCLUDED
#define SERIALIZATION_TEST_H_INCLUDED

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace boost::filesystem;

#include <boost/archive/portable_binary_oarchive.hpp>
#include <boost/archive/portable_binary_iarchive.hpp>

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/lexical_cast.hpp>

#include <I3Test.h>
#include <icetray/I3Logging.h>
#include <icetray/I3Frame.h>
#include <fstream>

#include <boost/iostreams/filtering_stream.hpp>
#include <icetray/open.h>

using boost::archive::portable_binary_oarchive;
using boost::archive::portable_binary_iarchive;

using boost::serialization::make_nvp;
using boost::serialization::base_object;

#define SERIALIZE(TYPE)				\
  TEST(TYPE) {					\
    do_archive<TYPE>(BOOST_PP_STRINGIZE(TYPE));	\
  } 

template <typename T>
static 
void save_to_i3_file(boost::shared_ptr<T> tptr, std::string fname)
{
  boost::iostreams::filtering_ostream ofs;
  I3::dataio::open(ofs, fname, 0);
  ENSURE(ofs, "couldn't open temp file");

  I3Frame frame;
  frame.Put("object", tptr);
  frame.save(ofs, std::vector<std::string>());
}

I3FramePtr load_i3_file(std::string fname);



template <class T>
void do_archive (std::string classname)
{
  std::string fname = classname + ".i3";
  shared_ptr<T> t_out(new T);
  save_to_i3_file(t_out, fname);
  load_i3_file(fname);
}

#endif
