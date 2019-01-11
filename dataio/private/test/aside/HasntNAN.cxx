/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: HasntNAN.cxx 7032 2005-04-27 21:44:45Z olivas $

    @version $Revision: 1.1 $
    @date $Date: 2005-04-27 23:44:45 +0200 (Wed, 27 Apr 2005) $
    @author Troy D. Straszheim

*/

#include "icetray/test/serialization-test.h"
#include "dataclasses/I3Hit.h"

struct HasntNAN {
  float f;
  double d;

  HasntNAN() {
    f = 0;
    d = 0;
  }

  template <class Archive>
  void serialize(Archive &ar, unsigned version)
  {
    ar & make_nvp("f", f);
    ar & make_nvp("d", d);
  }
};

TEST_GROUP(HasntNAN);

I3_SERIALIZATION_TEST(HasntNAN);
