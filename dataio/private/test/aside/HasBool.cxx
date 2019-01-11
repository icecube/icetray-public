/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: HasBool.cxx 7856 2005-05-20 17:43:47Z olivas $

    @version $Revision: 1.2 $
    @date $Date: 2005-05-20 19:43:47 +0200 (Fri, 20 May 2005) $
    @author Troy D. Straszheim

*/

#include <icetray/I3Logging.h>
#include "serialization-test.h"
#include "dataclasses/I3Hit.h"

struct HasBool {
  bool b;
  template <class Archive>
  void serialize(Archive &ar, unsigned version)
  {
    ar & make_nvp("b", b);
  }
};

TEST_GROUP(HasBool);

I3_SERIALIZATION_TEST(HasBool);

TEST(verified_bool_test_xml)
{
  HasBool btrue, bfalse;
  std::ofstream ofs("/tmp/verified_bool_test.xml");
  string msg = "couldn't open temporary file /tmp/verified_bool_test.xml";
  ENSURE(ofs, msg.c_str());  
  {
    xml_oarchive oa(ofs);
    btrue.b = true;
    oa << make_nvp("HasTrueBool", btrue);
    bfalse.b = false;
    oa << make_nvp("HasFalseBool", bfalse);
  }
  ofs.close();
  std::ifstream ifs("/tmp/verified_bool_test.xml");
  msg = "couldn't open temporary file /tmp/verified_bool_test.xml";
  ENSURE(ifs, msg.c_str());  
  {
    btrue.b = false;
    xml_iarchive ia(ifs);
    ia >> make_nvp("HasTrueBool", btrue);
    ENSURE(btrue.b == true);
    bfalse.b = true;
    ia >> make_nvp("HasFalseBool", bfalse);
    ENSURE(bfalse.b == false);
  }
  ifs.close();
  unlink("/tmp/verified_bool_test.xml");
}

TEST(verified_bool_test_text)
{
  HasBool btrue, bfalse;
  std::ofstream ofs("/tmp/verified_bool_test.txt");
  string msg = "couldn't open temporary file /tmp/verified_bool_test.txt";
  ENSURE(ofs, msg.c_str());  
  {
    text_oarchive oa(ofs);
    btrue.b = true;
    oa << make_nvp("HasTrueBool", btrue);
    bfalse.b = false;
    oa << make_nvp("HasFalseBool", bfalse);
  }
  ofs.close();

  std::ifstream ifs("/tmp/verified_bool_test.txt");
  msg = "couldn't open temporary file /tmp/verified_bool_test.txt";
  ENSURE(ifs, msg.c_str());  
  {
    btrue.b = false;
    text_iarchive ia(ifs);
    ia >> make_nvp("HasTrueBool", btrue);
    ENSURE(btrue.b == true);
    bfalse.b = true;
    ia >> make_nvp("HasFalseBool", bfalse);
    ENSURE(bfalse.b == false);
  }
  ifs.close();
  unlink("/tmp/verified_bool_test.txt");
}

