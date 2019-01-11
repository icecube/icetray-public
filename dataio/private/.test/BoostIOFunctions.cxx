#include <I3Test.h>
#include "dataio/I3BoostIOFunctions.h"
#include <fstream>

using boost::filesystem::path;
using std::string;

TEST_GROUP(BoostIOFunctions);

TEST(filename_to_archive){
  using boost_io_functions::filename_to_archive;
  //there are only two valid return values for this function
  boost_io::archive ar;
  ar = filename_to_archive("somename.binary");
  ENSURE(ar == boost_io::binary,"Wrong archive type returned");
  ar = filename_to_archive("somedir/somename.binary");
  ENSURE(ar == boost_io::binary,"Wrong archive type returned");

  ar = filename_to_archive("somename.someindex.binary");
  ENSURE(ar == boost_io::binary,"Wrong archive type returned");
  ar = filename_to_archive("somedir/somename.someindex.binary");
  ENSURE(ar == boost_io::binary,"Wrong archive type returned");

  ar = filename_to_archive("somename.xml");
  ENSURE(ar == boost_io::xml,"Wrong archive type returned");
  ar = filename_to_archive("somedir/somename.xml");
  ENSURE(ar == boost_io::xml,"Wrong archive type returned");

  ar = filename_to_archive("somename.someindex.xml");
  ENSURE(ar == boost_io::xml,"Wrong archive type returned");
  ar = filename_to_archive("somedir/somename.someindex.xml");
  ENSURE(ar == boost_io::xml,"Wrong archive type returned");

  ar = filename_to_archive("somedir/somename.XML");
  ENSURE(ar == boost_io::invalid,"This type should be invalid");
  ar = filename_to_archive("somename.BIN");
  ENSURE(ar == boost_io::invalid,"This type should be invalid");
}
