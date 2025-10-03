// SPDX-FileCopyrightText: 2025 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <I3Test.h>

#include <cmath>
#include <sstream>
#include <utility>
#include <vector>

#include <dataclasses/physics/I3DEggLaunch.h>
#include <icetray/I3Units.h>
#include <icetray/serialization.h>

namespace {

std::vector<int> setLaunchA(I3DEggLaunch& launch, bool lazy=true) {
  std::vector<int> adcData{0x03, 0x02, 0x04, 0x09, 0x02c9};
  launch.SetTime(3.*I3Units::microsecond, true)
        .SetLCBit(true)
        .SetADCData(adcData, lazy)
        .SetNPreSamples(2)
        .SetBaselineValue(7.31, true);
  return adcData;
}

std::vector<int> setLaunchB(I3DEggLaunch& launch) {
  std::vector<int> adcData{893, 88, 6, 1842, 11, 1846, 3};
  launch.SetTime(32.*I3Units::microsecond)
        .SetLCBit(false)
        .SetADCData(adcData)
        .SetNPreSamples(3)
        .SetBaselineValue(8.31, false);
  return adcData;
}

}


TEST_GROUP(I3DEggLaunch);

TEST(default_constructor) {
  I3DEggLaunch foo;
  ENSURE(std::isnan(foo.GetTime()),
         "Default constructor did not set time to NAN");
  ENSURE(!foo.HasValidTime(),
         "Default constructor did not set time to 'invalid'");
  EXPECT_THROW(foo.GetValidTime(),
               "Default constructor did not set time to 'invalid'");
  ENSURE(!foo.GetLCBit(), "Default constructor did not set LC to false");
  ENSURE(foo.GetADCData().empty(), "Default must not create ADC data");
  ENSURE_EQUAL(foo.GetNPreSamples(), 0u,
         "Default constructor did not set '#samples before trigger' to 0");
  ENSURE(std::isnan(foo.GetBaselineValue()),
         "Default constructor did not set baseline to NAN");
  ENSURE(!foo.HasValidBaselineValue(),
         "Default constructor did not set baseline to 'invalid'");
  EXPECT_THROW(foo.GetValidBaselineValue(),
               "Default constructor did not set baseline to 'invalid'");
}

TEST(set) {
  I3DEggLaunch foo;
  std::vector<int> adcData = setLaunchA(foo);
  ENSURE_EPSILON(foo.GetTime()/I3Units::microsecond, 3., 1e-9,
                 "SetTime did not set time to 3us");
  ENSURE(foo.HasValidTime(), "SetTime did not set time to 'valid'");
  ENSURE_EPSILON(foo.GetValidTime()/I3Units::microsecond, 3., 1e-9,
                 "SetTime did not set time to 'valid' 3us");
  ENSURE(foo.GetLCBit(), "SetLCBit did not set LC to true");
  ENSURE(foo.GetADCData() == adcData,
         "SetADCData did not set ADC data matching it's input");
  ENSURE_EQUAL(foo.GetNPreSamples(), 2u,
               "SetNPreSamples did not set '#samples before trigger' to 2");
  ENSURE_EPSILON(foo.GetBaselineValue(), 7.31, 1e-9,
                 "SetBaselineValue did not set baseline to 7.31");
  ENSURE(foo.HasValidBaselineValue(),
         "SetBaselineValue did not set baseline to 'valid'");
  ENSURE_EPSILON(foo.GetBaselineValue(), 7.31, 1e-9,
                 "SetBaselineValue did not set baseline to 'valid' 7.31");
  foo.SetLCBit(false);
  ENSURE(!foo.GetLCBit(), "SetLCBit did not set LC to false");
  foo.SetTime(7.*I3Units::microsecond, false);
  ENSURE_EPSILON(foo.GetTime()/I3Units::microsecond, 7., 1e-9,
                 "SetTime did not set time to 7us");
  ENSURE(!foo.HasValidTime(), "SetTime did not set time to 'invalid'");
  EXPECT_THROW(foo.GetValidTime(), "SetTime did not set time to 'invalid'");
  EXPECT_THROW(foo.SetTime(INFINITY, true),
               "SetTime must not call INFINITY a 'valid' time");
  EXPECT_THROW(foo.SetTime(NAN, true),
               "SetTime must not call NAN a 'valid' time");
  foo.SetBaselineValue(4.87, false);
  ENSURE_EPSILON(foo.GetBaselineValue(), 4.87, 1e-9,
                 "SetBaselineValue did not set baseline to 4.87");
  ENSURE(!foo.HasValidBaselineValue(),
         "SetBaselineValue did not set baseline to 'invalid'");
  EXPECT_THROW(foo.GetValidBaselineValue(),
               "SetBaselineValue did not set baseline to 'invalid'");
  EXPECT_THROW(foo.SetBaselineValue(INFINITY, true),
               "SetBaselineValue must not call INFINITY a 'valid' baseline");
  EXPECT_THROW(foo.SetBaselineValue(NAN, true),
               "SetBaselineValue must not call NAN a 'valid' baseline");
}

TEST(delta_compression) {
  std::vector<char> deltaCompressedData{static_cast<char>(0xbb), 0x62, 0x01, 0x02,
                                        0x0b, 0x00, 0x00, 0x00};
  unsigned int nSamples = 5u;
  std::vector<int> adcData{0x03, 0x02, 0x04, 0x09, 0x02c9};
  bool lazy = false;
  do {
    I3DEggLaunch foo;
    foo.SetDeltaCompressedADCData(deltaCompressedData, nSamples, lazy);
    ENSURE_EQUAL(foo.GetADCData().size(), nSamples,
                 "delta-compression is broken, output size doesn't match");
    ENSURE(foo.GetADCData() == adcData, "delta-compression is broken");
    lazy = !lazy;
  } while (lazy);
}

TEST(equal_operator) {
  I3DEggLaunch foo;
  std::vector<int> adcDataA = setLaunchA(foo);
  I3DEggLaunch bar;
  setLaunchA(bar);
  ENSURE(foo == bar, "I3DEggLaunch operator== failed");
  bar.SetBaselineValue(11.11, true);;
  ENSURE(!(foo == bar), "I3DEggLaunch ! operator== failed");
  ENSURE(foo != bar, "I3DEggLaunch ! operator== failed");
}

TEST(stream_operator) {
  I3DEggLaunch foo;
  {
    std::ostringstream oss;
    oss << foo;
    ENSURE(!oss.str().empty(), "cannot stream/print default launch");
  }
  {
    std::vector<int> adcData = setLaunchA(foo);
    std::ostringstream oss;
    oss << foo;
    ENSURE(!oss.str().empty(), "cannot stream/print launch");
  }
}

TEST(serialize) {
  bool lazy = false;
  do {
    I3DEggLaunch foo;
    std::vector<int> adcData = setLaunchA(foo, lazy);
    // Build a binary stringstream and serialize the I3DEggLaunch
    std::ostringstream oss(std::ostringstream::binary);
    {
      icecube::archive::portable_binary_oarchive outAr(oss);
      outAr & make_nvp("Test", foo);
    }

    I3DEggLaunch bar;
    // Deserialize a second I3XDOMLaunch from the serialized stream for comparison
    std::istringstream iss(oss.str(), std::istringstream::binary);
    {
      icecube::archive::portable_binary_iarchive inAr(iss);
      inAr & make_nvp("Test", bar);
    }
    // this will compare the compressed data of foo and bar...
    ENSURE(foo == bar,
          "I3DEggLaunch operator== failed, launch didn't serialize correctly");
    // ...so let's also force a comparison of uncompressed data
    ENSURE(foo.GetADCData() == bar.GetADCData(),
           "I3DEggLaunch operator== failed, launch didn't serialize correctly"
           " - check the uncompressed data!!!");
    lazy = !lazy;
  } while (lazy);
}

TEST(copy_move_constructor) {
  I3DEggLaunch foo;
  std::vector<int> adcData = setLaunchA(foo);
  {
    I3DEggLaunch bar(foo);
    ENSURE(foo == bar, "I3DEggLaunch copy constructor failed");
  }
  {
    I3DEggLaunch bar(std::move(foo));
    ENSURE_EPSILON(bar.GetTime()/I3Units::microsecond, 3., 1e-9,
                   "Time didn't move construct correctly");
    ENSURE(bar.HasValidTime(), "Time didn't move construct correctly");
    ENSURE(bar.GetLCBit(), "LC didn't move construct correctly");
    ENSURE(bar.GetADCData() == adcData,
           "ADC data didn't move construct correctly");
    ENSURE_EQUAL(bar.GetNPreSamples(), 2u,
                 "#presamples didn't move construct correctly");
    ENSURE_EPSILON(bar.GetBaselineValue(), 7.31, 1e-9,
                   "baseline didn't move construct correctly");
    ENSURE(bar.HasValidBaselineValue(),
           "baseline didn't move construct correctly");
  }
}

TEST(assignment_operator) {
  I3DEggLaunch foo;
  std::vector<int> adcDataA = setLaunchA(foo);
  I3DEggLaunch bar;
  std::vector<int> adcDataB = setLaunchB(bar);
  ENSURE(foo != bar, "Oups, launches shouldn't be equal");
  bar = foo;
  ENSURE(foo == bar, "assignment operator didn't work");

  adcDataB = setLaunchB(bar);
  ENSURE(foo != bar, "Oups, launches shouldn't be equal");
  bar = std::move(foo);
  ENSURE_EPSILON(bar.GetTime()/I3Units::microsecond, 3., 1e-9,
                 "Time didn't move assign correctly");
  ENSURE(bar.GetLCBit(), "LC didn't move correctly");
  ENSURE_EQUAL(bar.GetNPreSamples(), 2u,
               "#presamples didn't move assign correctly");
  ENSURE_EPSILON(bar.GetBaselineValue(), 7.31, 1e-9,
                 "baseline didn't move assign correctly");
  ENSURE(bar.HasValidBaselineValue(),
         "baseline didn't move assign correctly");
}
