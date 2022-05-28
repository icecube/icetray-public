/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$

    @version $Revision$
    @date $Date$
    @author tschmidt
*/

#include <I3Test.h>

#include <exception>
#include <string>
#include <map>

#include <boost/filesystem.hpp>

#include <interfaces/I3OMKey2MBID.h>
#include <phys-services/I3XMLOMKey2MBID.h>


using namespace std;


void initI3XMLOMKey2MBIDTest(I3OMKey2MBIDPtr& omkey2mbid,
                             const string& omkey2mbidFile)
{
  ENSURE(getenv("I3_SRC"));
  string infile(getenv("I3_SRC"));
  ENSURE(!infile.empty());
  infile.append("/phys-services/resources/");
  infile.append(omkey2mbidFile);
  omkey2mbid = I3OMKey2MBIDPtr(new I3XMLOMKey2MBID(infile));
  ENSURE((bool)omkey2mbid);
}


TEST_GROUP(I3XMLOMKey2MBIDTest);


TEST(a_ctor)
{
  I3OMKey2MBIDPtr omkey2mbid;

  EXPECT_THROW(I3XMLOMKey2MBID tmp(""), "didn't throw!");
  EXPECT_THROW(I3XMLOMKey2MBID tmp("UnKNowNFiLE"), "didn't throw!");
  EXPECT_THROW(initI3XMLOMKey2MBIDTest(omkey2mbid, "test_duplicated_OMKey_mainboard_ids.xml"),
	       "Didn't throw");
}


TEST(b_operations)
{
  I3OMKey2MBIDPtr omkey2mbid;
  OMKey key;
  long long int mbid;

  initI3XMLOMKey2MBIDTest(omkey2mbid, "test_basic_operations_mainboard_ids.xml");

  key = omkey2mbid->GetOMKey(3);
  ENSURE_EQUAL(key.GetString(), 1);
  ENSURE_EQUAL(static_cast<int>(key.GetOM()), 3);

  EXPECT_THROW(key = omkey2mbid->GetOMKey(33), "didn't throw");

  key = OMKey(1, 3);
  mbid = omkey2mbid->GetMBID(key);
  ENSURE_EQUAL(mbid, 3);

  key = OMKey(1, 33);
  EXPECT_THROW(omkey2mbid->GetMBID(key), "didn't throw!");
}


TEST(c_DMIce)
{
  I3OMKey2MBIDPtr omkey2mbid;
  OMKey key;
  long long int mbid;

  initI3XMLOMKey2MBIDTest(omkey2mbid, "mainboard_ids.xml.gz");

  ENSURE_EQUAL(omkey2mbid->GetMBID(OMKey(0, 2)), 0x8c40d705fc4b);
  ENSURE_EQUAL(omkey2mbid->GetMBID(OMKey(0, 3)), 0x37793e5448e1);
  ENSURE_EQUAL(omkey2mbid->GetMBID(OMKey(0, 4)), 0x99b591c21d6e);
  ENSURE_EQUAL(omkey2mbid->GetMBID(OMKey(0, 5)), 0x04f1d6939870);
}


TEST(d_Merge)
{
  I3OMKey2MBIDPtr omkey2mbid;
  initI3XMLOMKey2MBIDTest(omkey2mbid, "test_basic_operations_mainboard_ids.xml");
  EXPECT_THROW(omkey2mbid->GetMBID(OMKey(0, 2)), "didn't throw");

  map<long long int, OMKey> conversionTable;
  conversionTable[0x8c40d705fc4b] = OMKey(0, 2);
  conversionTable[0x37793e5448e1] = OMKey(0, 3);

  ENSURE(getenv("I3_SRC"));
  string infile(getenv("I3_SRC"));
  ENSURE(!infile.empty());
  infile.append("/phys-services/resources/test_basic_operations_mainboard_ids.xml");
  boost::filesystem::path temp = (boost::filesystem::temp_directory_path() /
                                  boost::filesystem::unique_path());
  string outfile = temp.native();
  I3XMLOMKey2MBID::Merge(outfile, infile, conversionTable);

  omkey2mbid = I3OMKey2MBIDPtr(new I3XMLOMKey2MBID(outfile));
  ENSURE_EQUAL(omkey2mbid->GetMBID(OMKey(0, 2)), 0x8c40d705fc4b);
  ENSURE_EQUAL(omkey2mbid->GetMBID(OMKey(0, 3)), 0x37793e5448e1);

  boost::filesystem::remove(outfile);
}
