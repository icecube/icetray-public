#ifndef I3_DATAIO_TEST_H_INCLUDED
#define I3_DATAIO_TEST_H_INCLUDED

#include <string>
#include "boost/filesystem.hpp"
#include "I3Test.h"

namespace fs = boost::filesystem;

static std::string __attribute__ ((unused))
GetDataDir()
{
  namespace fs = boost::filesystem;

  ENSURE(getenv("I3_TESTDATA") != NULL,
         "I3_TESTDATA must be defined in the parent shell.");

  const std::string I3_TESTDATA(getenv("I3_TESTDATA"));
  fs::path data_dir(I3_TESTDATA + "/dataio");

  ENSURE(fs::exists(data_dir), "Directory "
         "'$I3_TESTDATA/dataio' doesn't exist.");

#if BOOST_VERSION > 104100
  return data_dir.native();
#else
  return data_dir.file_string();
#endif
}

#endif // I3_DATAIO_TEST_H_INCLUDED
