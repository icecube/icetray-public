/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#include <dataclasses/external/CompareFloatingPoint.h>
#include <dataclasses/physics/I3mDOMLaunch.h>
#include <icetray/I3Units.h>


// xDOMApp Reference Manual:
// "The mDOM FPGA samples the discriminator logic level at 960 MHz"
const double I3mDOMLaunch::TOTHit::SAMPLING_RATE = 960 * I3Units::megahertz;


template <class Archive>
void I3mDOMLaunch::TOTHit::serialize(Archive& ar, unsigned int version) {
  if (version > i3mdomlaunch_version_) {
    log_fatal("Attempting to read version %u from file"
              " but running version %u of I3mDOMLaunch class.",
              version, i3mdomlaunch_version_);
  }
  ar & make_nvp("time", time_);
  ar & make_nvp("tot", tot_);
}

std::ostream& I3mDOMLaunch::TOTHit::Print(std::ostream& os) const {
  os << "I3mDOMLaunch::TOTHit(time=" << time_
     << ", tot=" << tot_ << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3mDOMLaunch::TOTHit& hit) {
  return hit.Print(os);
}

std::ostream& operator<<(std::ostream& os,
                         const I3mDOMLaunch::TriggerType& triggerType) {
  switch(triggerType) {
    case I3mDOMLaunch::TriggerType::UNDEFINED:
      os << "UNDEFINED";
      break;
    case I3mDOMLaunch::TriggerType::SOFTWARE:
      os << "SOFTWARE";
      break;
    case I3mDOMLaunch::TriggerType::ADC_THRESHOLD:
      os << "ADC_THRESHOLD";
      break;
    case I3mDOMLaunch::TriggerType::DISCRIMINATOR:
      os << "DISCRIMINATOR";
      break;
    case I3mDOMLaunch::TriggerType::EXTERNAL:
      os << "EXTERNAL";
      break;
  }
  return os;
}

bool
I3mDOMLaunch::TOTHit::operator==(const I3mDOMLaunch::TOTHit& rhs) const {
  return (CompareFloatingPoint::Compare(time_, rhs.time_) &&
          CompareFloatingPoint::Compare(tot_, rhs.tot_));
}


template <class Archive>
void I3mDOMLaunch::serialize(Archive& ar, unsigned int version) {
  if (version > i3mdomlaunch_version_) {
    log_fatal("Attempting to read version %u from file"
              " but running version %u of I3mDOMLaunch class.",
              version, i3mdomlaunch_version_);
  }
  ar & make_nvp("launch", launch_);
  ar & make_nvp("triggerType", triggerType_);
  ar & make_nvp("totHits", totHits_);
}

std::ostream& I3mDOMLaunch::Print(std::ostream& os) const {
  os << "I3mDOMLaunch(launch=" << launch_
     << ", triggertype=" << triggerType_
     << ", #tothits=" << totHits_.size() << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3mDOMLaunch& launch) {
  return launch.Print(os);
}

bool
I3mDOMLaunch::operator==(const I3mDOMLaunch& rhs) const {
  return (launch_ == rhs.launch_ &&
          triggerType_ == rhs.triggerType_ &&
          totHits_ == rhs.totHits_);
}


I3_SERIALIZABLE(I3mDOMLaunch);
I3_SERIALIZABLE(I3mDOMLaunch::TOTHit);
I3_SERIALIZABLE(I3mDOMLaunchSeriesMap);
