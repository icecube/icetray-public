#ifndef SERIALIZATION_TEST_H_INCLUDED
#define SERIALIZATION_TEST_H_INCLUDED

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <serialization/shared_ptr.hpp>
#include <serialization/vector.hpp>
#include <serialization/string.hpp>
#include <boost/lexical_cast.hpp>

#include <I3Test.h>
#include <icetray/I3Logging.h>
#include <icetray/I3Frame.h>
#include <fstream>

#include <boost/iostreams/filtering_stream.hpp>
#include <archive/portable_binary_archive.hpp>
#include <icetray/open.h>

using icecube::archive::portable_binary_oarchive;
using icecube::archive::portable_binary_iarchive;

using icecube::serialization::make_nvp;
using icecube::serialization::base_object;

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
  ENSURE((bool)ofs, "couldn't open temp file");

  I3Frame frame;
  frame.Put("object", tptr);
  frame.save(ofs, std::vector<std::string>());
}

I3FramePtr load_i3_file(std::string fname);



template <class T>
void do_archive (std::string classname)
{
  std::string fname = classname + ".i3";
  boost::shared_ptr<T> t_out(new T);
  save_to_i3_file(t_out, fname);
  load_i3_file(fname);
  remove(fname.c_str());
}

#endif
