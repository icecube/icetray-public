// SPDX-FileCopyrightText: 2025 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dataclasses/calibration/I3AntennaCalibration.h>

// I3AntennaCal: what goes into the I3AntennaCalMap with info for each antenna
I3AntennaCal::~I3AntennaCal() {}

template <class Archive>
void I3AntennaCal::serialize(Archive& ar, unsigned version) {
  if (version > i3antennacalibration_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3AntennaCal class.", version, i3antennacalibration_version_);

  ar & make_nvp("AntennaType", antennaType_);
  ar & make_nvp("CableType", cableType_);
  ar & make_nvp("DaqType", daqType_);
  ar & make_nvp("CableLength", cableLength_);
}

std::ostream& I3AntennaCal::Print(std::ostream& os) const {
  os << "[I3AntennaCal AntennaType: " << static_cast<int>(antennaType_) << '\n'
     << "                        CableType: " << static_cast<int>(cableType_) << '\n'
     << "                          DaqType: " << static_cast<int>(daqType_) << '\n'
     << "                      CableLength: " << cableLength_ << " ]";
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3AntennaCal& c) {
  return (c.Print(os));
}

std::ostream& operator<<(std::ostream& os, const I3AntennaCal::AntennaType t) {
  os << static_cast<int>(t);
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3AntennaCal::CableType t) {
  os << static_cast<int>(t);
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3AntennaCal::DaqType t) {
  os << static_cast<int>(t);
  return os;
}

// I3AntennaCalibration: Container storing the I3AntennaCalMap in the frame
I3AntennaCalibration::I3AntennaCalibration(){}

I3AntennaCalibration::~I3AntennaCalibration(){}

template <class Archive>
void 
I3AntennaCalibration::save(Archive& ar, unsigned version) const
{
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("antennacal", antennaCalMap);
  ar & make_nvp("StartTime",startTime);
  ar & make_nvp("EndTime",endTime);
}

template <class Archive>
void 
I3AntennaCalibration::load(Archive& ar, unsigned version) {  
  if (version>i3radcalibration_version_)
  log_fatal("Attempting to read version %u from file but running version %u of I3AntennaCalibration class.", version, i3radcalibration_version_);
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("antennacal", antennaCalMap);
  ar & make_nvp("StartTime",startTime);
  ar & make_nvp("EndTime",endTime);
}

I3_SERIALIZABLE(I3AntennaCal);
I3_SERIALIZABLE(I3AntennaCalMap);
I3_SPLIT_SERIALIZABLE(I3AntennaCalibration);
