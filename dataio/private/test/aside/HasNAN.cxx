/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: HasNAN.cxx 7856 2005-05-20 17:43:47Z olivas $

    @version $Revision: 1.1 $
    @date $Date: 2005-05-20 19:43:47 +0200 (Fri, 20 May 2005) $
    @author Troy D. Straszheim

*/

#include "icetray/test/serialization-test.h"
#include "dataclasses/I3Hit.h"
#include <sstream>

struct HasNan {
  float f, g;
  double d, e;

  HasNan() {
    f = NAN;
    g = NAN;
    d = NAN;
    e = NAN;
  }

  template <class Archive>
  void serialize(Archive &ar, unsigned version)
  {
    ar & make_nvp("f", f);
    ar & make_nvp("f", g);
    ar & make_nvp("d", d);
    ar & make_nvp("d", e);
  }
};

TEST_GROUP(HasNan);

I3_SERIALIZATION_TEST(HasNan);

// this test shows if streaming in a float as "nan" from text actually
// works or bails out.  The behavior is in libstdc++, not in our stuff.

#if 0
TEST(stdio_nan)
{
  ostringstream oss;
  float f = NAN, g = NAN;
  oss << f;
  istringstream iss(oss.str());
  cout << "oss.str == " << oss.str() << endl;
  cout << "iss.str == " << iss.str() << endl;
  //  ENSURE(iss.str() == "nan");
  f = 2;
  ENSURE(!iss.fail());
  iss >> f;
  ENSURE(!iss.fail());
  cout << "f=" << f << endl;
  ENSURE(isnan(f));
}
#endif

TEST(a_nan)
{
  double nans[5];
  {
    for (int i=0; i<5; i++)
      {
	nans[i] = NAN;
	ENSURE(isnan(nans[i]));
      }
    std::ofstream ofs("/tmp/one_nan.xml");
    string msg = "couldn't open temporary file /tmp/one_nan.xml";
    ENSURE(ofs, msg.c_str());
    xml_oarchive oa(ofs);
    for (int i=0; i<5; i++)
      oa << make_nvp("a_nan", nans[i]);
    ofs.close();
  }
  {
    std::ifstream ifs("/tmp/one_nan.xml");
    string msg = "couldn't open temporary file /tmp/one_nan.xml";
    ENSURE(ifs, msg.c_str());
    xml_iarchive ia(ifs);
    for (int i=0; i<5; i++)
      {
	ia >> make_nvp("a_nan", nans[i]);
	ENSURE(isnan(nans[i]));
      }
    ifs.close();
  }
  unlink("/tmp/one_nan.xml");
}

TEST(verified_nan_test)
{
  HasNan n;
  std::ofstream ofs("/tmp/verified_nan_test.xml");
  string msg = "couldn't open temporary file /tmp/verified_nan_test.xml";
  ENSURE(ofs, msg.c_str());
  {
    xml_oarchive oa(ofs);
    oa << make_nvp("HasNans", n);
  }
  {
    xml_oarchive oa(ofs);
    n.f = 1.0;
    n.d = 1.0;
    oa << make_nvp("HasntNans", n);
  }

  ofs.close();
  std::ifstream ifs("/tmp/verified_nan_test.xml");
  msg = "couldn't open temporary file /tmp/verified_nan_test.xml";
  ENSURE(ifs, msg.c_str());  
  {
    xml_iarchive ia(ifs);
    ia >> make_nvp("HasNans", n);
    ENSURE(isnan(n.d));
    ENSURE(isnan(n.f));
  }
  {
    xml_iarchive ia(ifs);
    ia >> make_nvp("HasntNans", n);
    ENSURE(n.d== 1.0);
    ENSURE(n.f== 1.0);
  }
  ifs.close();
  unlink("/tmp/verified_nan_test.xml");
}
