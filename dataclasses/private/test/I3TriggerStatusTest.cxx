/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$

*/

#include <I3Test.h>

#include "dataclasses/status/I3TriggerStatus.h"
#include "icetray/I3Units.h"

#include <iostream>
#include <string>

#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>

using boost::assign::list_of;

using namespace std;

TEST_GROUP(I3TriggerStatusTest);

TEST(TriggerStatusSetGet)
{

  I3TriggerStatus myts; 

  map<I3TriggerStatus::Subdetector, I3TriggerReadoutConfig> & readoutconfig = 
    myts.GetReadoutSettings();

  I3TriggerReadoutConfig myreadout;
  myreadout.readoutTimeMinus = 5000.0*I3Units::ns;
  myreadout.readoutTimePlus = 6000.0*I3Units::ns;
  myreadout.readoutTimeOffset = -1000.0*I3Units::ns;

  readoutconfig[I3TriggerStatus::INICE] = myreadout;
  const map<I3TriggerStatus::Subdetector, I3TriggerReadoutConfig> & testconfig = 
    myts.GetReadoutSettings();
  I3TriggerReadoutConfig mytest = testconfig.find(I3TriggerStatus::INICE)->second;
  
  ENSURE_DISTANCE(5000.0, mytest.readoutTimeMinus/I3Units::ns, 0.01,
		  "Readout configs do not match.");
  ENSURE_DISTANCE(6000.0, mytest.readoutTimePlus/I3Units::ns, 0.01,
		  "Readout configs do not match.");
  ENSURE_DISTANCE(-1000.0, mytest.readoutTimeOffset/I3Units::ns, 0.01,
		  "Readout configs do not match.");

}

TEST(TriggerStatusSerialize)
{

  I3TriggerStatus myts; 

  map<I3TriggerStatus::Subdetector, I3TriggerReadoutConfig> & readoutconfig = 
    myts.GetReadoutSettings();

  I3TriggerReadoutConfig myreadout;
  myreadout.readoutTimeMinus = 5000.0*I3Units::ns;
  myreadout.readoutTimePlus = 6000.0*I3Units::ns;
  myreadout.readoutTimeOffset = -1000.0*I3Units::ns;

  readoutconfig[I3TriggerStatus::INICE] = myreadout;

 // Build a binary stringtream and serialize the I3TriggerStatus
        std::ostringstream oss(std::ostringstream::binary);
        {
                icecube::archive::portable_binary_oarchive outAr( oss );
                outAr & make_nvp("Test", myts);;
        }
  
  // Deserialize a second I3DOMLaunch from the serialized stream for comparison
        I3TriggerStatus mydecode;
        std::istringstream iss( oss.str(), std::istringstream::binary );
        {
                icecube::archive::portable_binary_iarchive inAr( iss );
                inAr & make_nvp("Test", mydecode);
        }



  const map<I3TriggerStatus::Subdetector, I3TriggerReadoutConfig>& testconfig = 
    mydecode.GetReadoutSettings();
  I3TriggerReadoutConfig mytest = testconfig.find(I3TriggerStatus::INICE)->second;
  
  ENSURE_DISTANCE(5000.0, mytest.readoutTimeMinus/I3Units::ns, 0.01,
		  "Readout configs do not match.");
  ENSURE_DISTANCE(6000.0, mytest.readoutTimePlus/I3Units::ns, 0.01,
		  "Readout configs do not match.");
  ENSURE_DISTANCE(-1000.0, mytest.readoutTimeOffset/I3Units::ns, 0.01,
		  "Readout configs do not match.");

}

TEST(TriggerStatusSettingsTest)
{

  I3TriggerStatus myts; 

  std::string good_int_key("good_int");
  int int_value(42);
  myts.SetTriggerConfigValue(good_int_key,int_value);

  boost::optional<int> int_test_value; 
  myts.GetTriggerConfigValue(good_int_key, int_test_value);
  ENSURE( int_test_value && int_value == int_test_value,
	  "Get/Set not working properly for ints." );

  std::string good_float_key("good_float");
  float float_value(0.5);
  myts.SetTriggerConfigValue(good_float_key, float_value);

  boost::optional<float> float_test_value;
  myts.GetTriggerConfigValue(good_float_key, float_test_value);
  ENSURE( (bool)float_test_value );
  ENSURE_DISTANCE( float_value, float_test_value.get(), 1e-9,
		   "Get/Set not working properly for floats." );

}

TEST(ConvertFloats)
{

  // this first group are going to be
  // the  most common forms
  std::vector<const char*> convertible_floats = list_of
    ("0.5")("-0.5")("+.5")(".5")("-.5")("+0.5")("+5")("5")("-5")
    ("0.5e10")("0.5e-10")("0.5E10")("0.5E-10");

  BOOST_FOREACH(const char* from, convertible_floats){
    boost::optional<float> to;
    Convert(std::string(from),to);
    ENSURE( (bool)to , "'%s' should convert to a float");
  }

  std::vector<const char*> unconvertible_floats = list_of
    ("0.5e0.1")("0.5E0.1")("-0.5e0.1")("-0.5E0.1")
    ("+0.5e0.1")("+0.5E0.1")("+0.5E0.1")("-0.5e-0.1")
    ("-0.5E-0.1")("+0.5E-0.1")("-0.5e+0.1")("-0.5E+0.1")
    ("+0.5e+0.1")("+0.5E+0.1")("+0.5E+.0.1")("+e0.5E+0.1")
    ("O.5")("+-0.5e+0.1")("--0.5E+0.1")("-+0.5e+0.1")
    ("++0.5E+0.1");

  BOOST_FOREACH(const char* from, unconvertible_floats){
    boost::optional<float> to;
    Convert(std::string(from),to);
    ENSURE( !to , "'%s' should not convert to a float");
  }

}

TEST(ConvertInts)
{
  // this first group are going to be
  // the  most common forms
  boost::optional<int> to;
  Convert("1", to);
  ENSURE( (bool)to );

  // Test that pathological inputs correctly
  Convert("0.5", to);
  ENSURE( !to );
}

TEST(ConvertBools)
{
  std::vector<const char*> convertible_bools = list_of
    ("true")("false")("True")("False")("T")("F")
    ("tRue")("falSe")("t")("f")("TRUE")("FALSE");

  BOOST_FOREACH(const char* from, convertible_bools){
    boost::optional<bool> to;
    Convert(std::string(from),to);
    ENSURE( (bool)to , "'%s' should convert to a bool");
  }

  std::vector<const char*> unconvertible_bools = list_of
    ("1.0")("0.")("2")("yup")("foshizzle");

  BOOST_FOREACH(const char* from, unconvertible_bools){
    boost::optional<bool> to;
    Convert(std::string(from),to);
    ENSURE( !to , "'%s' should convert to a bool");
  }

}

TEST(ConvertStrings)
{
  boost::optional<std::string> to;
  Convert("this is a string", to);
  ENSURE( (bool)to );
  Convert("there are many like them", to);
  ENSURE( (bool)to );
  Convert("but this one is mine", to);
  ENSURE( (bool)to );
}

TEST(SettingsFromString)
{

  I3TriggerStatus myts; 

  myts.SetTriggerConfigValue("good_float","0.5");
  myts.SetTriggerConfigValue("good_exp_float","3.14e-3");
  myts.SetTriggerConfigValue("good_Exp_float","3.14E-3");
  myts.SetTriggerConfigValue("good_int","10");
  myts.SetTriggerConfigValue("good_negative_int","-10");

  myts.SetTriggerConfigValue("bad_float","e-1.0");
  myts.SetTriggerConfigValue("bad_int","0.5");

  boost::optional<std::string> str_value;
  myts.GetTriggerConfigValue("good_float",str_value);
  ENSURE(str_value == std::string("0.5"));

  boost::optional<float> float_value;
  myts.GetTriggerConfigValue("good_float", float_value);  
  ENSURE((bool)float_value);
  ENSURE_DISTANCE( float_value.get(), 0.5, 1e-9 );

  ENSURE((bool)float_value);
  myts.GetTriggerConfigValue("good_exp_float", float_value);  
  ENSURE_DISTANCE( float_value.get(), 3.14e-3, 1e-9 );

  ENSURE((bool)float_value);
  myts.GetTriggerConfigValue("good_Exp_float", float_value);  
  ENSURE_DISTANCE( float_value.get(), 3.14e-3, 1e-9 );

  ENSURE((bool)float_value);
  myts.GetTriggerConfigValue("bad_float", float_value);
  ENSURE( !float_value );

  boost::optional<int> int_value;
  myts.GetTriggerConfigValue("good_int", int_value);
  ENSURE( int_value && int_value == 10 );

  myts.GetTriggerConfigValue("good_negative_int", int_value);
  ENSURE((bool)int_value);
  ENSURE( int_value && int_value == -10 );

  myts.GetTriggerConfigValue("bad_int", int_value);
  ENSURE(!int_value);
  
  myts.GetTriggerConfigValue("good_float", int_value);
  ENSURE(!int_value);

  myts.GetTriggerConfigValue("bad_float", int_value);
  ENSURE(!int_value);
  
}

TEST(SettingsFromStringRequired)
{

  I3TriggerStatus myts; 

  myts.SetTriggerConfigValue("good_float","0.5");
  myts.SetTriggerConfigValue("good_exp_float","3.14e-3");
  myts.SetTriggerConfigValue("good_Exp_float","3.14E-3");
  myts.SetTriggerConfigValue("good_int","10");
  myts.SetTriggerConfigValue("good_negative_int","-10");

  myts.SetTriggerConfigValue("bad_float","e-1.0");
  myts.SetTriggerConfigValue("bad_int","0.5");

  std::string str_value;
  myts.GetTriggerConfigValue("good_float",str_value);
  ENSURE(str_value == std::string("0.5"));

  float float_value(NAN);
  myts.GetTriggerConfigValue("good_float", float_value);  
  ENSURE_DISTANCE( float_value, 0.5, 1e-9 );

  myts.GetTriggerConfigValue("good_exp_float", float_value);  
  ENSURE_DISTANCE( float_value, 3.14e-3, 1e-9 );

  myts.GetTriggerConfigValue("good_Exp_float", float_value);  
  ENSURE_DISTANCE( float_value, 3.14e-3, 1e-9 );

  int int_value;
  myts.GetTriggerConfigValue("good_int", int_value);
  ENSURE( int_value == 10 );

  myts.GetTriggerConfigValue("good_negative_int", int_value);
  ENSURE( int_value == -10 );

  // the following are required but the conversion will
  // fail, so they should throw.
  EXPECT_THROW(myts.GetTriggerConfigValue("bad_int", int_value), "This should throw."); 
  EXPECT_THROW(myts.GetTriggerConfigValue("good_float", int_value), "This should throw.");
  EXPECT_THROW(myts.GetTriggerConfigValue("bad_float", float_value), "This should throw.");
  EXPECT_THROW(myts.GetTriggerConfigValue("bad_float", int_value), "This should throw.");

}
