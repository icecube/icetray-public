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

#include <icetray/I3Tray.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>
#include <icetray/OMKey.h>
#include <boost/assign/std/vector.hpp>

using namespace boost::assign;

static bool bool_param;
static unsigned char uchar_param;
static int int_param;
static long long_param;
static double double_param;
static std::string string_param;
static std::vector<int> intvec_param;
static std::vector<double> doublevec_param;
static std::vector<std::string> stringvec_param;
static OMKey omkey_param;
static std::vector<OMKey> omkeyvec_param;

struct MyService{};
static boost::shared_ptr<MyService> service_ptr_param;

struct AllParametersModule : I3Module
{
  AllParametersModule(const I3Context& context) : I3Module(context) 
  { 
    bool_param = true;
    AddParameter("bool_param", "description of bool", bool_param);

    uchar_param = std::numeric_limits<unsigned char>::max();
    AddParameter("uchar_param", "description of uchar", uchar_param);

    int_param = std::numeric_limits<int>::max();
    AddParameter("int_param", "description of int", int_param);

    long_param = std::numeric_limits<long>::max();
    AddParameter("long_param", "description of long", long_param);

    //      double_param = std::numeric_limits<double>::max();
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

    service_ptr_param = boost::shared_ptr<MyService>();
    AddParameter("service_ptr_param", "pointer to service.",service_ptr_param);
  }

  virtual void Configure() 
  { 
    GetParameter("bool_param", bool_param);
    GetParameter("uchar_param", uchar_param);
    GetParameter("int_param", int_param);
    GetParameter("long_param", long_param);
    GetParameter("double_param", double_param);
    GetParameter("string_param", string_param);
    GetParameter("intvec_param", intvec_param);
    GetParameter("doublevec_param", doublevec_param);
    GetParameter("stringvec_param", stringvec_param);
    GetParameter("omkey_param", omkey_param);
    GetParameter("omkeyvec_param", omkeyvec_param);
    GetParameter("service_ptr_param",service_ptr_param);
  }

  //No need to test methods that do nothing
  //LCOV_EXCL_LINE
  virtual void Process() { }
  virtual void Finish() { ; }
};

I3_MODULE(AllParametersModule);
