// SPDX-FileCopyrightText: 2025 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <I3Test.h>

#include <cmath>
#include <sstream>
#include <utility>

#include <dataclasses/physics/detail/UpgradeLCFlags.h>
#include <dataclasses/physics/I3XDOMHit.h>
#include <icetray/I3Units.h>
#include <icetray/serialization.h>

namespace {
void setHitA(I3XDOMHit& hit) {
  hit.SetTime(3.*I3Units::microsecond)
    .SetLCFlags(UpgradeLCFlags::MultiModuleLC)
     .SetCharge(21.*I3Units::picocoulomb);
}

void setHitB(I3XDOMHit& hit) {
  hit.SetTime(32.*I3Units::microsecond)
    .SetLCFlags(UpgradeLCFlags::SingleModuleLC)
     .SetCharge(93.*I3Units::picocoulomb);
}
}


TEST_GROUP(I3XDOMHit);

TEST(default_constructor) {
  I3XDOMHit foo;
  ENSURE(std::isnan(foo.GetTime()),
         "Default constructor did not set time to NAN");
  ENSURE(foo.GetLCFlags() == UpgradeLCFlags::NoLC, "Default constructor did not set LC to false");
  ENSURE(std::isnan(foo.GetCharge()),
         "Default constructor did not set charge to NAN");
}

TEST(set) {
  I3XDOMHit foo;
  setHitA(foo);
  ENSURE_EPSILON(foo.GetTime()/I3Units::microsecond, 3., 1e-9,
                 "SetTime did not set time to 3us");
  ENSURE(foo.GetLCFlags() == UpgradeLCFlags::MultiModuleLC, "SetLCFlags did not set LC to true");
  ENSURE_EPSILON(foo.GetCharge()/I3Units::picocoulomb, 21., 1e-9,
                 "SetCharge did not set charge to 21pC");
}

TEST(equal_operator) {
  I3XDOMHit foo;
  setHitA(foo);
  I3XDOMHit bar;
  setHitA(bar);
  ENSURE(foo == bar, "I3XDOMHit operator== failed");
  bar.SetCharge(22.*I3Units::picocoulomb);
  ENSURE(!(foo == bar), "I3XDOMHit ! operator== failed");
  ENSURE(foo != bar, "I3XDOMHit ! operator== failed");
}

TEST(stream_operator) {
  I3XDOMHit foo;
  {
    std::ostringstream oss;
    oss << foo;
    ENSURE(!oss.str().empty(), "cannot stream/print default hit");
  }
  {
    setHitA(foo);
    std::ostringstream oss;
    oss << foo;
    ENSURE(!oss.str().empty(), "cannot stream/print hit");
  }
}

TEST(serialize) {
  I3XDOMHit foo;
  setHitA(foo);
  // Build a binary stringtream and serialize the I3XDOMHit
  std::ostringstream oss(std::ostringstream::binary);
  {
    icecube::archive::portable_binary_oarchive outAr(oss);
    outAr & make_nvp("Test", foo);
  }

  I3XDOMHit bar;
  // Deserialize a second I3XDOMHit from the serialized stream for comparison
  std::istringstream iss(oss.str(), std::istringstream::binary);
  {
    icecube::archive::portable_binary_iarchive inAr(iss);
    inAr & make_nvp("Test", bar);
  }
  ENSURE_EPSILON(bar.GetTime()/I3Units::microsecond, 3., 1e-9,
                 "Time didn't serialize correctly");
  ENSURE(bar.GetLCFlags() == UpgradeLCFlags::MultiModuleLC, "LC didn't serialize correctly");
  ENSURE_EPSILON(bar.GetCharge()/I3Units::picocoulomb, 21., 1e-9,
                 "Charge didn't serialize correctly");
  ENSURE(foo == bar, "hit didn't serialize correctly");
}

TEST(copy_move_constructor) {
  I3XDOMHit foo;
  setHitA(foo);
  {
    I3XDOMHit bar(foo);
    ENSURE(foo == bar, "copy constructor failed");
  }
  {
    I3XDOMHit bar(std::move(foo));
    ENSURE_EPSILON(bar.GetTime()/I3Units::microsecond, 3., 1e-9,
                  "Time didn't move construct correctly");
    ENSURE(bar.GetLCFlags() == UpgradeLCFlags::MultiModuleLC, "LC didn't move construct correctly");
    ENSURE_EPSILON(bar.GetCharge()/I3Units::picocoulomb, 21., 1e-9,
                  "Charge didn't move construct correctly");
  }
}

TEST(assignment_operator) {
  I3XDOMHit foo;
  setHitA(foo);
  I3XDOMHit bar;
  setHitB(bar);
  ENSURE(foo != bar, "Oups, hits shouldn't be equal");
  bar = foo;
  ENSURE(foo == bar, "assignment operator didn't work");

  setHitB(bar);
  ENSURE(foo != bar, "Oups, hits shouldn't be equal");
  bar = std::move(foo);
  ENSURE_EPSILON(bar.GetTime()/I3Units::microsecond, 3., 1e-9,
                 "Time didn't move assign correctly");
  ENSURE(bar.GetLCFlags() == UpgradeLCFlags::MultiModuleLC, "LC didn't move correctly");
  ENSURE_EPSILON(bar.GetCharge()/I3Units::picocoulomb, 21., 1e-9,
                 "Charge didn't move assign correctly");
}
