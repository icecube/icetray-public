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

using namespace boost::assign;

TEST_GROUP(VectorParameters);

static std::vector<int> intvec_param;
static std::vector<unsigned long> ulongvec_param;
static std::vector<double> doublevec_param;
static std::vector<std::string> stringvec_param;
static OMKey omkey_param;
static std::vector<OMKey> omkeyvec_param;

struct VectorParamsTestModule : I3Module
{
  VectorParamsTestModule(const I3Context& context) : I3Module(context) 
  { 
    intvec_param.clear();
    intvec_param += 0,1,2,3,4,5,6,7,8,9; 
    AddParameter("intvec_param", "vector of ints", intvec_param);

    ulongvec_param.clear();
    ulongvec_param += 0,1,2,3,4;
    AddParameter("ulongvec_param", "vector of ulongs", ulongvec_param);

    doublevec_param.clear();
    doublevec_param += 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9;
    AddParameter("doublevec_param", "vector of doubles", doublevec_param);

    stringvec_param.clear();
    stringvec_param += "one", "two", "three";
    AddParameter("stringvec_param", "vector of strings", stringvec_param);

    omkeyvec_param.clear();
    omkeyvec_param += OMKey(0,0), OMKey(-1,1), OMKey(-2,2), OMKey(-3,3);
    AddParameter("omkeyvec_param", "a vector of omkeys.  So there.", omkeyvec_param);
  }

  virtual void Configure() 
  { 
    GetParameter("intvec_param", intvec_param);
    GetParameter("doublevec_param", doublevec_param);
    GetParameter("ULongVec_Param", ulongvec_param);
    GetParameter("stringvec_param", stringvec_param);
    GetParameter("omkeyvec_param", omkeyvec_param);
  }

  virtual void Process() 
  { 
    log_trace("%s",__PRETTY_FUNCTION__);
    I3FramePtr frame(new I3Frame(I3Frame::Physics));
    PushFrame(frame);
  }
};

I3_MODULE(VectorParamsTestModule);

TEST(dumb_strings)
{
  std::vector<std::string> stringv;
  stringv += "c0", "c1", "c2";
  stringvec_param.clear();
  I3Tray tray;
  tray.AddModule("VectorParamsTestModule")
    ("stringvec_param", stringv);
  tray.Execute(0);

  ENSURE_EQUAL(stringvec_param.size(), 3u);
  ENSURE_EQUAL(stringvec_param[0], "c0");
  ENSURE_EQUAL(stringvec_param[1], "c1");
  ENSURE_EQUAL(stringvec_param[2], "c2");
}

TEST(leading_trailing_and_embedded_whitespace)
{
  std::vector<std::string> stringv;
  stringvec_param.clear();
  stringv += " leadingspace", 
    "trailingspace ", 
    "embedded space", 
    "\t", 
    "\n", 
    " \t\n",
    ".. \t\n .. \r\n\t ..";

  I3Tray tray;
  tray.AddModule("VectorParamsTestModule")
    ("stringvec_param", stringv);
  tray.Execute(0);

  ENSURE_EQUAL(stringvec_param.size(), stringv.size());

  for (unsigned i=0; i<stringv.size(); i++)
    ENSURE_EQUAL(stringvec_param[i], stringv[i]);

}


