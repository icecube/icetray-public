// SPDX-FileCopyrightText: 2025 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <I3Test.h>
#include "icetray/I3Frame.h"

#include <dataclasses/calibration/I3AntennaCalibration.h>

TEST_GROUP(I3AntennaCal);

// Test I3AntennaCal class
TEST(constructor) {
  I3AntennaCal theCalib;
  ENSURE(I3AntennaCal::AntennaType::Unknown == theCalib.GetAntennaType());

  I3AntennaCalPtr thePtr(new I3AntennaCal());
}

TEST(setters_n_getters) {
  I3AntennaCal theCalib;

  theCalib.SetAntennaType(I3AntennaCal::AntennaType::SKALA2);
  ENSURE(I3AntennaCal::AntennaType::SKALA2 == theCalib.GetAntennaType());
  
  theCalib.SetCableType(I3AntennaCal::CableType::LMR400);
  ENSURE(I3AntennaCal::CableType::LMR400 == theCalib.GetCableType());

  theCalib.SetDaqType(I3AntennaCal::DaqType::Board2_Taxi3_2);
  ENSURE(I3AntennaCal::DaqType::Board2_Taxi3_2 == theCalib.GetDaqType());

  theCalib.SetCableLength(0.1234);
  ENSURE(0.1234 == theCalib.GetCableLength());
}

TEST(comparison_operators) {
  I3AntennaCal theCalib;
  theCalib.SetAntennaType(I3AntennaCal::AntennaType::SKALA2);
  theCalib.SetCableType(I3AntennaCal::CableType::LMR400);
  theCalib.SetDaqType(I3AntennaCal::DaqType::Board2_Taxi3_2);
  theCalib.SetCableLength(0.1234);

  I3AntennaCal theOtherCalib;
  theOtherCalib.SetAntennaType(I3AntennaCal::AntennaType::SKALA2);
  theOtherCalib.SetCableType(I3AntennaCal::CableType::LMR400);
  theOtherCalib.SetDaqType(I3AntennaCal::DaqType::Board2_Taxi3_2);
  theOtherCalib.SetCableLength(0.1234);

  ENSURE(theOtherCalib == theCalib);

  theOtherCalib.SetAntennaType(I3AntennaCal::AntennaType::SKALA4);
  ENSURE(theOtherCalib != theCalib);
  theOtherCalib.SetAntennaType(I3AntennaCal::AntennaType::SKALA2);
  ENSURE(theOtherCalib == theCalib);

  theOtherCalib.SetCableType(I3AntennaCal::CableType::Unknown);
  ENSURE(theOtherCalib != theCalib);
  theOtherCalib.SetCableType(I3AntennaCal::CableType::LMR400);
  ENSURE(theOtherCalib == theCalib);

  theOtherCalib.SetDaqType(I3AntennaCal::DaqType::Board1_Taxi3_0);
  ENSURE(theOtherCalib != theCalib);
  theOtherCalib.SetDaqType(I3AntennaCal::DaqType::Board2_Taxi3_2);
  ENSURE(theOtherCalib == theCalib);

  theOtherCalib.SetCableLength(0.4321);
  ENSURE(theOtherCalib != theCalib);
  theOtherCalib.SetCableLength(0.1234);
  ENSURE(theOtherCalib == theCalib);
}

TEST(radcalmap) {
  I3AntennaCalMap calMap;
  I3AntennaCal theCalib;
  theCalib.SetAntennaType(I3AntennaCal::AntennaType::SKALA2);
  theCalib.SetCableType(I3AntennaCal::CableType::LMR400);
  theCalib.SetDaqType(I3AntennaCal::DaqType::Board2_Taxi3_2);
  theCalib.SetCableLength(0.1234);

  AntennaKey key(0, 0);
  calMap[key] = theCalib;
}

TEST(printing) {
  I3AntennaCal theCalib;
  theCalib.SetAntennaType(I3AntennaCal::AntennaType::SKALA2);
  theCalib.SetCableType(I3AntennaCal::CableType::LMR400);
  theCalib.SetDaqType(I3AntennaCal::DaqType::Board2_Taxi3_2);
  theCalib.SetCableLength(0.1234);

  std::cerr << theCalib << std::endl;
}

// Test I3AntennaCalibration
TEST(radcalibration) {
  // make I3AntennaCal
  I3AntennaCal theCalib;
  theCalib.SetAntennaType(I3AntennaCal::AntennaType::SKALA2);
  theCalib.SetCableType(I3AntennaCal::CableType::LMR400);
  theCalib.SetDaqType(I3AntennaCal::DaqType::Board2_Taxi3_2);
  theCalib.SetCableLength(0.1234);

  // Put it in a map
  I3AntennaCalMap calMap;
  AntennaKey key(0, 0);
  calMap[key] = theCalib;

  // Put the map in a container
  I3AntennaCalibrationPtr frame_obj(new I3AntennaCalibration);
  frame_obj->startTime = I3Time(0);
  frame_obj->endTime = I3Time(0);
  frame_obj->antennaCalMap = calMap;

  // Put the container in a frame
  I3FramePtr frame(new I3Frame(I3Frame::Physics));
  frame->Put("I3AntennaCal", frame_obj);

  // Make sure we get back what we put in
  I3AntennaCalibration frame_obj_fetched = frame->Get<I3AntennaCalibration>("I3AntennaCal");
  I3AntennaCalMap calMap_fetched = frame_obj_fetched.antennaCalMap;
  ENSURE(calMap_fetched.size() == calMap.size());
  ENSURE(calMap_fetched[key] == calMap[key]);
}
