#include <I3Test.h>

#include "phys-services/I3FileOMKey2MBID.h"

#include <sstream>
#include <iostream>

using namespace std;

// These tests look for a few specific mbids and OMKeys that
// are in the test file.  Checks that the mapping works correctly
TEST_GROUP(OMKey2MBIDTest);

TEST(0df7b060acad)
{
  I3OMKey2MBIDPtr service;
  ENSURE(getenv("I3_SRC"));
  string infile(getenv("I3_SRC"));
  infile.append("/phys-services/resources/doms.txt");
  service = I3OMKey2MBIDPtr(new I3FileOMKey2MBID(infile));
  
  istringstream in;  
  long long int innum;
  
    // spot check a few
  in.str("0df7b060acad");
  in>>hex>>innum;
  cout<<service->GetMBID(OMKey(21,28))<<" =?= "<<innum<<endl;
  ENSURE(service->GetMBID(OMKey(21,34)) == innum,"0df7b060acad");
}

TEST(8b9f35308e27)
{
  I3OMKey2MBIDPtr service;
  ENSURE(getenv("I3_SRC"));
  string infile(getenv("I3_SRC"));
  infile.append("/phys-services/resources/doms.txt");
  service = I3OMKey2MBIDPtr(new I3FileOMKey2MBID(infile));
  
  istringstream in;  
  long long int innum;

  in.str("8b9f35308e27");
  in>>hex>>innum;
  cout<<service->GetMBID(OMKey(21,28))<<" =?= "<<innum<<endl;
  ENSURE(service->GetMBID(OMKey(21,28)) == innum,"8b9f35308e27");
}

TEST(d52b66ab6861)
{
  I3OMKey2MBIDPtr service;
  ENSURE(getenv("I3_SRC"));
  string infile(getenv("I3_SRC"));
  infile.append("/phys-services/resources/doms.txt");
  service = I3OMKey2MBIDPtr(new I3FileOMKey2MBID(infile));
  
  istringstream in;  
  long long int innum;
  
  in.str("d52b66ab6861");
  in>>hex>>innum;
  cout<<service->GetMBID(OMKey(21,28))<<" =?= "<<innum<<endl;
  ENSURE(service->GetMBID(OMKey(30,64)) == innum,"d52b66ab6861");
  
}
//Test one on a 2006 string
TEST(543c1369639d)
{
  I3OMKey2MBIDPtr service;
  ENSURE(getenv("I3_SRC"));
  string infile(getenv("I3_SRC"));
  infile.append("/phys-services/resources/doms.txt");
  service = I3OMKey2MBIDPtr(new I3FileOMKey2MBID(infile));

  istringstream in;
  long long int innum;

  in.str("543c1369639d");
  in>>hex>>innum;
  cout<<service->GetMBID(OMKey(21,28))<<" =?= "<<innum<<endl;
  ENSURE(service->GetMBID(OMKey(49,43)) == innum,"d52b66ab6861");

}



