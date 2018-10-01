/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @version $Revision$
 * @date $Date$
 * @author troy d. straszheim
 *
 * This tests that the global GetService<> works; that the underlying
 * context-switching is done correctly.
 */

#include <I3Test.h>
#include <boost/preprocessor/stringize.hpp>
#include "icetray/I3Tray.h"
#include "icetray/I3ServiceFactory.h"
#include <icetray/I3Frame.h>
#include <icetray/OMKey.h>
#include <boost/assign/std/vector.hpp>

#include <cmath>
using namespace std;	// Fix broken implemenation of C99/3BSD fpclassify
			// functions on some GCC versions, where they may or
			// may not appear in std::

using namespace boost::assign;

TEST_GROUP(ModuleParameterConversions);

static bool bool_param;
static int int_param;
static long long_param;
static double double_param;
static std::string string_param;
static std::vector<int> intvec_param;
static std::vector<double> doublevec_param;
static std::vector<std::string> stringvec_param;
static OMKey omkey_param;
static std::vector<OMKey> omkeyvec_param;

struct ParamConversionCheckModule : I3Module
{
  ParamConversionCheckModule(const I3Context& context) : I3Module(context) 
  { 
    bool_param = true;
    AddParameter("bool_param", "description of bool", bool_param);

    int_param = std::numeric_limits<int>::max();
    AddParameter("int_param", "description of int", int_param);

    long_param = std::numeric_limits<long>::max();
    AddParameter("long_param", "description of long", long_param);

    double_param = 3.1415926535897932;
    AddParameter("double_param", "description of double", double_param);

    string_param = "We can't stop here.  This is Bat Country!";
    AddParameter("string_param", "description of string", string_param);
    
    intvec_param.clear();
    intvec_param += 0,1,2,3,4,5,6,7,8,9; 
    AddParameter("intvec_param", "vector of ints", intvec_param);

    doublevec_param.clear();
    doublevec_param += 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9;
    AddParameter("doublevec_param", "vector of doubles", doublevec_param);

    stringvec_param.clear();
    stringvec_param += "one", "two", "three";
    AddParameter("stringvec_param", "vector of strings", stringvec_param);

    omkey_param = OMKey(-666,666);
    AddParameter("omkey_param", "OMKey!", omkey_param);

    omkeyvec_param.clear();
    omkeyvec_param += OMKey(0,0), OMKey(-1,1), OMKey(-2,2), OMKey(-3,3);
    AddParameter("omkeyvec_param", "OMG a vector of omkeys lol", omkeyvec_param);
  }

  virtual void Configure() 
  { 
    GetParameter("bool_param", bool_param);
    GetParameter("int_param", int_param);
    GetParameter("long_param", long_param);
    GetParameter("double_param", double_param);
    GetParameter("string_param", string_param);
    GetParameter("intvec_param", intvec_param);
    GetParameter("doublevec_param", doublevec_param);
    GetParameter("stringvec_param", stringvec_param);
    GetParameter("omkey_param", omkey_param);
    GetParameter("omkeyvec_param", omkeyvec_param);
  }

  virtual void Process() 
  { 
    log_trace("%s",__PRETTY_FUNCTION__);
    I3FramePtr frame(new I3Frame(I3Frame::Physics));
    PushFrame(frame, "OutBox");
  }

  virtual void Resume() { ; }
  virtual void Finish() { ; }
  virtual void Abort() { ; }
};

I3_MODULE(ParamConversionCheckModule);

#define _EXPECT_THROW(name, value, obj)					  \
  {									  \
    try {                                                                 \
      I3Tray tray;                                                        \
      tray.AddModule<ParamConversionCheckModule>("mod");		  \
      tray.SetParameter("mod", BOOST_PP_STRINGIZE(name), value);	  \
      log_trace("egh, about to execute");				  \
      tray.Execute(0);							  \
      FAIL("This should have thrown.");                                   \
    } catch (const obj& e) {                             		  \
      PyErr_Clear();                                                      \
    }						                          \
  }

#define GET_SUCCEEDS(name, value)					\
  {									\
    I3Tray tray;                                                        \
    tray.AddModule<ParamConversionCheckModule>("mod");	                \
    ENSURE(tray.SetParameter("mod", BOOST_PP_STRINGIZE(name), value));	\
    tray.Execute(0);							\
    ENSURE(name == value);						\
  }

#define GET_ONLY(name, value)						\
  {									\
    I3Tray tray;                                                        \
    tray.AddModule<ParamConversionCheckModule>("mod");			\
    ENSURE(tray.SetParameter("mod", BOOST_PP_STRINGIZE(name), value));	\
    tray.Execute(0);							\
  }

// straightforward: no conversions
bool b; 
short shrt; 
int i; 
long l; 
long long ll;
float f;
double d; 
const char *c_str = "some string";
std::string s = "some string";

TEST(a_nonexistent)
{
  // bool converts to everything
  b = true;
  _EXPECT_THROW(nonexistent_parameter, b, std::runtime_error);
}

TEST(a2_nonexistent)
{
  _EXPECT_THROW(nonexistent_parameter, i, std::runtime_error);
}

TEST(b_bool)
{
  GET_SUCCEEDS(bool_param, b);
  GET_SUCCEEDS(int_param, b);
  GET_SUCCEEDS(long_param, b);
  GET_SUCCEEDS(double_param, b);

  _EXPECT_THROW(string_param, b, boost::python::error_already_set);

  b = false;
  GET_SUCCEEDS(bool_param, b);
  GET_SUCCEEDS(int_param, b);
  GET_SUCCEEDS(long_param, b);
  GET_SUCCEEDS(double_param, b);
}

TEST(c_ints)
{
  i = 0;
  GET_SUCCEEDS(bool_param, false); 
  i = 1;
  GET_SUCCEEDS(bool_param, false); 
  GET_SUCCEEDS(bool_param, true);
  GET_SUCCEEDS(bool_param, true); 

  i = 13;
  GET_SUCCEEDS(int_param, i);
  GET_SUCCEEDS(long_param, i);
  GET_SUCCEEDS(double_param, i);
  i = 14;
  GET_SUCCEEDS(int_param, i);
  GET_SUCCEEDS(long_param, i);
  GET_SUCCEEDS(double_param, i);
}

TEST(d_longs)
{
  l = 0;
  GET_SUCCEEDS(bool_param, l); 

  l = 1;
  GET_SUCCEEDS(bool_param, l); 

  l = 13;
  GET_SUCCEEDS(int_param, l);
  GET_SUCCEEDS(long_param, l);
  GET_SUCCEEDS(double_param, l);
  l = 14;
  GET_SUCCEEDS(int_param, l);
  GET_SUCCEEDS(long_param, l);
  GET_SUCCEEDS(double_param, l);
  if (INT_MAX < LONG_MAX)
    {
      l = INT_MAX;
      l++;
      _EXPECT_THROW(int_param, l, boost::python::error_already_set);
      l = INT_MIN;
      l--;
      _EXPECT_THROW(int_param, l, boost::python::error_already_set);
    }
  else
    {
      l = LONG_MAX;
      GET_SUCCEEDS(int_param, l);
      l = LONG_MIN;
      GET_SUCCEEDS(int_param, l);
    }
      
  GET_SUCCEEDS(long_param, l);
  GET_SUCCEEDS(double_param, l);
}

TEST(e_doubles)
{
  d = 6.0;
  _EXPECT_THROW(bool_param, d, boost::python::error_already_set);
  _EXPECT_THROW(int_param, d, boost::python::error_already_set);
  _EXPECT_THROW(long_param, d, boost::python::error_already_set);
  GET_SUCCEEDS(double_param, d);

  GET_SUCCEEDS(string_param, s);

  // check precision
  d = 0.1234568901234567;
  GET_SUCCEEDS(double_param, d);   
  ENSURE_DISTANCE(double_param, d, 0.0000000000000001 );

  d = NAN;
  GET_ONLY(double_param, d);
  ENSURE(std::isnan(double_param));

  d = std::numeric_limits<double>::max() * -2.;
  ENSURE(std::isinf(d));
  ENSURE(d < 0);
  GET_ONLY(double_param, d);
  ENSURE(std::isinf(double_param));
  ENSURE(double_param < 0);

  d = std::numeric_limits<double>::max() * 2.;
  ENSURE(std::isinf(d));
  ENSURE(d > 0);
  GET_ONLY(double_param, d);
  ENSURE(std::isinf(double_param));

}

TEST(f_strings_etc)
{
  {
    I3Tray tray;   
    tray.AddModule<ParamConversionCheckModule>("mod");
    
    ENSURE(tray.SetParameter("mod", "string_param", "some string"));
    tray.Execute(1);
    ENSURE(strcmp(c_str, "some string") == 0); 
  }

  std::vector<int> ivec;
  ivec += 1, 2, 3;
  std::vector<double> dvec;
  dvec += 7.8, 2.04, 0.003;
  GET_SUCCEEDS(intvec_param, ivec);
  GET_SUCCEEDS(doublevec_param, dvec);

  std::vector<std::string> svec;
  svec += "foo", "bar", "baz";
  GET_ONLY(stringvec_param, svec);
}

TEST(g_omkeys)
{    
  OMKey key(9,9);
  std::vector<OMKey> keyvec;
  keyvec += OMKey(0,0), OMKey(3,9), OMKey(4,7);

  GET_SUCCEEDS(omkey_param, key);
  GET_SUCCEEDS(omkeyvec_param, keyvec);

  _EXPECT_THROW(omkey_param, b, boost::python::error_already_set);
  _EXPECT_THROW(omkey_param, i, boost::python::error_already_set);
  _EXPECT_THROW(omkey_param, d, boost::python::error_already_set);
  _EXPECT_THROW(omkey_param, s, std::runtime_error);
}

TEST(case_insensitivity)
{
  double_param = 0.0000;
  int_param = 0;
  I3Tray tray;   
  tray.AddModule<ParamConversionCheckModule>("mod");
  
  ENSURE(tray.SetParameter("mod", "DoUbLe_PaRaM", 3.14159));
  ENSURE(tray.SetParameter("mod", "int_paraM", 314159));
  tray.Execute(1);
  ENSURE_EQUAL(double_param, 3.14159);
  ENSURE_EQUAL(int_param, 314159);
}


struct GetsWithoutAdding : public I3Module
{
  GetsWithoutAdding(const I3Context& context) : I3Module(context) 
  { 
  }

  void Configure() { 
    int i;
    GetParameter("never_added", i);
  }
};

I3_MODULE(GetsWithoutAdding);

TEST(module_gets_parameter_that_it_never_added)
{
  I3Tray tray;
  tray.AddModule<GetsWithoutAdding>("gets");
  try {
    tray.Execute(0); // causes Configure() to get called
    FAIL("that should have thrown");
  } catch (const std::exception& e) { }
}


struct AddsWithoutGetting : public I3Module
{
  int i;

  AddsWithoutGetting(const I3Context& context) : I3Module(context) 
  { 
    i=3;
    AddParameter("never_gets_this_int", 
		 "This here lonesome int is ignored at Configure() time",
		 i);
  }
};

I3_MODULE(AddsWithoutGetting);

TEST(module_adds_parameter_that_it_never_gets)
{
  I3Tray tray;
  tray.AddModule<AddsWithoutGetting>("gets");
  try {
    tray.Execute(0); 
    // causes Configure() to get called
    // a module is allowed to Add params that dont get Get later.
  } catch (const std::exception& e) {
    FAIL("that should not have thrown");
 }
}

