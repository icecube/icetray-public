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

  ENSURE(getenv("I3_PORTS") != NULL,
         "I3_PORTS must be defined in the parent shell.");

  const std::string I3_BUILD(getenv("I3_PORTS"));
  fs::path data_dir(I3_BUILD + "/test-data/dataio");

  ENSURE(fs::exists(data_dir), "Directory "
         "'$I3_PORTS/test-data/dataio' doesn't exist.");

#if BOOST_VERSION > 104100
  return data_dir.native();
#else
  return data_dir.file_string();
#endif
}

#endif // I3_DATAIO_TEST_H_INCLUDED
