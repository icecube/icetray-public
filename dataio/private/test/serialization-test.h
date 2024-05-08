// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef SERIALIZATION_TEST_H_INCLUDED
#define SERIALIZATION_TEST_H_INCLUDED

#include "I3Test.h"
#include "icetray/I3Frame.h"
#include "icetray/open.h"

#include <boost/version.hpp>
#if BOOST_VERSION < 108500
#include <boost/filesystem/convenience.hpp>
#endif
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/lexical_cast.hpp>

#include <archive/portable_binary_archive.hpp>

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
