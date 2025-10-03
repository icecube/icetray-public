// SPDX-FileCopyrightText: 2025 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <I3Test.h>

#include <cmath>
#include <sstream>
#include <utility>

#include <dataclasses/physics/I3NoiseSuppressionRecord.h>
#include <icetray/I3Units.h>
#include <icetray/serialization.h>


namespace {

void setRecordA(I3NoiseSuppressionRecord& record) {
  record.SetTime(3.*I3Units::microsecond)
#ifdef I3NOISESUPPRESSIONRECORD_INCLUDE_SIZES
        .SetExclusionWindowSize(300.*I3Units::microsecond)
        .SetLCWindowSize(400.*I3Units::microsecond)
#endif
        .SetNNoiseHits(11);
}

void setRecordB(I3NoiseSuppressionRecord& record) {
  record.SetTime(32.*I3Units::microsecond)
#ifdef I3NOISESUPPRESSIONRECORD_INCLUDE_SIZES
        .SetExclusionWindowSize(500.*I3Units::microsecond)
        .SetLCWindowSize(600.*I3Units::microsecond)
#endif
        .SetNNoiseHits(33);
}

}


TEST_GROUP(I3NoiseSuppressionRecord);

TEST(default_constructor) {
  I3NoiseSuppressionRecord foo;
  ENSURE(std::isnan(foo.GetTime()),
         "Default constructor did not set time to NAN");
#ifdef I3NOISESUPPRESSIONRECORD_INCLUDE_SIZES
  ENSURE(std::isnan(foo.GetExclusionWindowSize()),
         "Default constructor did not set size to NAN");
  ENSURE(std::isnan(foo.GetLCWindowSize()),
         "Default constructor did not set LC size to NAN");
#endif
  ENSURE_EQUAL(foo.GetNNoiseHits(), 0u,
               "Default constructor did not set N to 0")
}

TEST(set) {
  I3NoiseSuppressionRecord foo;
  setRecordA(foo);
  ENSURE_EPSILON(foo.GetTime()/I3Units::microsecond, 3., 1e-9,
                 "SetTime did not set time to 3us");
#ifdef I3NOISESUPPRESSIONRECORD_INCLUDE_SIZES
  ENSURE_EPSILON(foo.GetExclusionWindowSize()/I3Units::microsecond, 300., 1e-9,
                 "SetSize did not set time to 300us");
  ENSURE_EPSILON(foo.GetLCWindowSize()/I3Units::microsecond, 400., 1e-9,
                 "SetLCSize did not set time to 400us");
#endif
  ENSURE_EQUAL(foo.GetNNoiseHits(), 11u, "SetN did not set N to 11")
}

TEST(equal_operator) {
  I3NoiseSuppressionRecord foo;
  setRecordA(foo);
  I3NoiseSuppressionRecord bar;
  setRecordA(bar);
  ENSURE(foo == bar, "I3NoiseSuppressionRecord operator== failed");
  bar.SetNNoiseHits(22);
  ENSURE(!(foo == bar), "I3NoiseSuppressionRecord ! operator== failed");
  ENSURE(foo != bar, "I3NoiseSuppressionRecord ! operator== failed");
}

TEST(stream_operator) {
  I3NoiseSuppressionRecord foo;
  {
    std::ostringstream oss;
    oss << foo;
    ENSURE(!oss.str().empty(), "cannot stream/print default record");
  }
  {
    setRecordA(foo);
    std::ostringstream oss;
    oss << foo;
    ENSURE(!oss.str().empty(), "cannot stream/print record");
  }
}

TEST(serialize) {
  I3NoiseSuppressionRecord foo;
  setRecordA(foo);
  // Build a binary stringtream and serialize the I3NoiseSuppressionRecord
  std::ostringstream oss(std::ostringstream::binary);
  {
    icecube::archive::portable_binary_oarchive outAr(oss);
    outAr & make_nvp("Test", foo);
  }

  I3NoiseSuppressionRecord bar;
  // Deserialize a second I3NoiseSuppressionRecord from the serialized stream for comparison
  std::istringstream iss(oss.str(), std::istringstream::binary);
  {
    icecube::archive::portable_binary_iarchive inAr(iss);
    inAr & make_nvp("Test", bar);
  }
  ENSURE_EPSILON(bar.GetTime()/I3Units::microsecond, 3., 1e-9,
                 "Time didn't serialize correctly");
#ifdef I3NOISESUPPRESSIONRECORD_INCLUDE_SIZES
  ENSURE_EPSILON(bar.GetExclusionWindowSize()/I3Units::microsecond, 300., 1e-9,
                 "Size didn't serialize correctly");
  ENSURE_EPSILON(bar.GetLCWindowSize()/I3Units::microsecond, 400., 1e-9,
                 "LC size didn't serialize correctly");
#endif
  ENSURE_EQUAL(bar.GetNNoiseHits(), 11u, "N didn't serialize correctly")
  ENSURE(foo == bar,
         "I3NoiseSuppressionRecord operator== failed, record didn't serialize correctly");
}

TEST(copy_move_constructor) {
  I3NoiseSuppressionRecord foo;
  setRecordA(foo);
  {
    I3NoiseSuppressionRecord bar(foo);
    ENSURE(foo == bar, "I3NoiseSuppressionRecord copy constructor failed");
  }
  {
    I3NoiseSuppressionRecord bar(std::move(foo));
    ENSURE_EPSILON(bar.GetTime()/I3Units::microsecond, 3., 1e-9,
                  "Time didn't move construct correctly");
#ifdef I3NOISESUPPRESSIONRECORD_INCLUDE_SIZES
    ENSURE_EPSILON(bar.GetExclusionWindowSize()/I3Units::microsecond, 300., 1e-9,
                  "Size didn't move construct correctly");
    ENSURE_EPSILON(bar.GetLCWindowSize()/I3Units::microsecond, 400., 1e-9,
                  "LC size didn't move construct correctly");
#endif
    ENSURE_EQUAL(bar.GetNNoiseHits(), 11u, "N didn't move construct correctly")
  }
}

TEST(assignment_operator) {
  I3NoiseSuppressionRecord foo;
  setRecordA(foo);
  I3NoiseSuppressionRecord bar;
  setRecordB(bar);
  ENSURE(foo != bar, "Oups, records shouldn't be equal");
  bar = foo;
  ENSURE(foo == bar, "assignment operator didn't work");

  setRecordB(bar);
  ENSURE(foo != bar, "Oups, records shouldn't be equal");
  bar = std::move(foo);
  ENSURE_EPSILON(bar.GetTime()/I3Units::microsecond, 3., 1e-9,
                 "Time didn't move assign correctly");
#ifdef I3NOISESUPPRESSIONRECORD_INCLUDE_SIZES
  ENSURE_EPSILON(bar.GetExclusionWindowSize()/I3Units::microsecond, 300., 1e-9,
                 "Size didn't move assign correctly");
  ENSURE_EPSILON(bar.GetLCWindowSize()/I3Units::microsecond, 400., 1e-9,
                 "LC size didn't move assign correctly");
#endif
  ENSURE_EQUAL(bar.GetNNoiseHits(), 11u, "N didn't move assign correctly")
}
