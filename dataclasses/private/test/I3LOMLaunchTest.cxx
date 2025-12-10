// SPDX-FileCopyrightText: 2025 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <I3Test.h>

#include <cmath>
#include <sstream>
#include <utility>
#include <vector>

#include <dataclasses/physics/I3LOMLaunch.h>
#include <icetray/I3Units.h>
#include <icetray/serialization.h>

TEST_GROUP(I3LOMLaunch);

TEST(default_constructor) {

  I3LOMLaunch launch;
  ENSURE(std::isnan(launch.GetTime()), "Default constructor did not set time to NAN");
  ENSURE(!launch.HasValidTime(), "Default constructor did not set time to 'invalid'");
  EXPECT_THROW(launch.GetValidTime(), "Default constructor did not set time to 'invalid'");
  ENSURE(launch.GetADCDataHighGain().empty(), "Default must not create ADC data");
  ENSURE(launch.GetADCDataLowGain().empty(), "Default must not create ADC data");
  ENSURE(!launch.IsADCDataFilled(), "Default must not create ADC data");
  EXPECT_THROW(launch.GetADCDataLength(), "Cannot get ADC data length until it has been set");

}

TEST(set) {

  I3LOMLaunch launch;

  // Test setting valid time
  double t = 7*I3Units::microsecond;
  launch.SetTime(t, true);
  ENSURE_EPSILON(launch.GetTime(), t, 1e-9, "SetTime failed");
  ENSURE(launch.HasValidTime(), "Time is not valid");

  // Test setting invalid time
  t = 10*I3Units::microsecond;
  launch.SetTime(t, false);
  ENSURE_EPSILON(launch.GetTime(), t, 1e-9, "SetTime failed");
  ENSURE(!launch.HasValidTime(), "Time is not invalid");

  // Test setting ADC waveforms (common setter)
  std::vector<int> adcHighGain{1, 2, 3};
  std::vector<int> adcLowGain{4, 5, 6};
  launch.SetADCData(adcHighGain, adcLowGain);
  ENSURE(launch.GetADCDataHighGain() == adcHighGain, "Settings ADC high gain data failed");
  ENSURE(launch.GetADCDataLowGain() == adcLowGain, "Settings ADC low gain data failed");
  ENSURE(launch.IsADCDataFilled(), "ADC data filled flag is wrong");
  ENSURE(launch.GetADCDataLength() == adcHighGain.size(), "ADC length is wrong");

  // Test setting ADC waveforms (separate setters)
  std::vector<int> adcHighGain2{10, 20, 30, 40};
  launch.SetADCDataHighGain(adcHighGain2);
  ENSURE(launch.GetADCDataHighGain() == adcHighGain2, "Settings ADC high gain data failed");

  std::vector<int> adcLowGain2{50, 60, 70, 80};
  launch.SetADCDataLowGain(adcLowGain2);
  ENSURE(launch.GetADCDataLowGain() == adcLowGain2, "Settings ADC low gain data failed");

  // Test that trying to set high and low gain waveforms of different lengths fails
  std::vector<int> adcHighGain3{1, 2, 3};
  std::vector<int> adcLowGain3{4, 5, 6, 7};
  EXPECT_THROW(launch.SetADCData(adcHighGain3, adcLowGain3), "Expect error is try and set high and low gain waveforms of different lengths");

}

TEST(equal_operator) {

  // Define values
  double t = 1.;
  bool valid = true;
  std::vector<int> adcHighGain{1, 2, 3};
  std::vector<int> adcLowGain{4, 5, 6};

  // Create first launch
  I3LOMLaunch foo;
  foo.SetTime(t, valid);
  foo.SetADCData(adcHighGain, adcLowGain);

  // Check equality fails with a fresh (unfilled) new launch
  I3LOMLaunch bar;
  ENSURE(foo != bar, "Test != failed");

  // Check equality succeeds with a new launch with the same members
  bar.SetTime(t, valid);
  bar.SetADCData(adcHighGain, adcLowGain);
  ENSURE(foo == bar, "Test == failed");
}

TEST(serialize) {

  // Create a launch
  I3LOMLaunch foo;
  foo.SetTime(123., true);
  std::vector<int> adcHighGain{1, 2, 3};
  std::vector<int> adcLowGain{4, 5, 6};
  foo.SetADCData(adcHighGain, adcLowGain);

  // Build a binary stringstream and serialize the I3LOMLaunch "foo"
  std::ostringstream oss(std::ostringstream::binary);
  {
    icecube::archive::portable_binary_oarchive outAr(oss);
    outAr & make_nvp("Test", foo);
  }

  // Deserialize a second I3DOMLaunch "bar" from the serialized stream for comparison
  I3LOMLaunch bar;
  std::istringstream iss(oss.str(), std::istringstream::binary);
  {
    icecube::archive::portable_binary_iarchive inAr(iss);
    inAr & make_nvp("Test", bar);
  }

  // Now check that bar matches foo, e.g. nothing changes following serialization+deserialization
  ENSURE(foo == bar, "I3LOMLaunch serialization test failed");
}

TEST(copy_move_constructor) {

  I3LOMLaunch foo;
  double t = 1.;
  bool valid = true;
  std::vector<int> adcHighGain{1, 2, 3};
  std::vector<int> adcLowGain{4, 5, 6};
  foo.SetTime(t, valid);
  foo.SetADCData(adcHighGain, adcLowGain);

  // Test copy constructor
  {
    I3LOMLaunch bar(foo);
    ENSURE(foo == bar, "I3LOMLaunch copy constructor failed");
  }

  // Test move constructor
  {
    I3LOMLaunch bar(std::move(foo));
    ENSURE_EPSILON(bar.GetTime(), t, 1e-9, "Time didn't move construct correctly");
    ENSURE(bar.HasValidTime(), "Time didn't move construct correctly");
    ENSURE(bar.GetADCDataHighGain() == adcHighGain, "ADC high gain data didn't move construct correctly");
    ENSURE(bar.GetADCDataLowGain() == adcLowGain, "ADC low gain data didn't move construct correctly");
  }
}

TEST(assignment_operator) {

  I3LOMLaunch foo;
  std::vector<int> adcHighGain{1, 2, 3};
  std::vector<int> adcLowGain{4, 5, 6};
  foo.SetTime(1., true);
  foo.SetADCData(adcHighGain, adcLowGain);

  I3LOMLaunch bar;
  std::vector<int> adcHighGain2{10, 20, 30};
  std::vector<int> adcLowGain2{40, 50, 60};
  bar.SetTime(2., false);
  bar.SetADCData(adcHighGain2, adcLowGain2);

  ENSURE(foo != bar, "Launches shouldn't be equal before assignment");

  bar = foo;
  ENSURE(foo == bar, "assignment operator didn't work");

}
