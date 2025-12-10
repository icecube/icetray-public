/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#include <dataclasses/external/CompareFloatingPoint.h>
#include <dataclasses/physics/I3LOMLaunch.h>


bool I3LOMLaunch::IsADCDataFilled() const {
  return CheckADCData(adcDataHighGain_, adcDataHighGain_);
}


unsigned int I3LOMLaunch::GetADCDataLength() const {

  // First the ADC data
  if( ! CheckADCData(adcDataHighGain_, adcDataHighGain_)) {
    log_fatal("Cannot get LOM ADC data length. Either ADC data is empty, or there is a mismatch between the high and low gain channels");
  }

  // If checks passed, return the length. Can use either channel, both must be the same length.
  return adcDataHighGain_.size();
}


I3LOMLaunch& I3LOMLaunch::SetADCData(std::vector<int>& adcHG, std::vector<int>& adcLG) {

  // First the input ADC data
  if( ! CheckADCData(adcHG, adcLG)) {
    log_fatal("Cannot set the LOM ADC data. Either the inputs data is empty, or there is a mismatch between the high and low gain channels");
  }

  // Store the ADC daata
  adcDataHighGain_ = adcHG;
  adcDataLowGain_ = adcLG;

  return *this;

}


bool I3LOMLaunch::CheckADCData(const std::vector<int>& adcHG, const std::vector<int>& adcLG) const {
  /*
    Check the ADC data provided (a) has >0 samples, and (b) is the same length for high and low gain
  */

  size_t adcHGLength = adcHG.size();
  size_t adcLGLength = adcLG.size();

  if (adcHGLength == 0) {
    return false;
  }
  if (adcLGLength == 0) {
    return false;
  }
  if (adcHGLength != adcLGLength) {
    return false;
  }

  return true;

}


std::ostream& I3LOMLaunch::Print(std::ostream& os) const {
  os << "I3LOMLaunch(time=" << time_.first << ", length=" << GetADCDataLength() << ")";
  return os;
}


std::ostream& operator<<(std::ostream& os, const I3LOMLaunch& launch) {
  return launch.Print(os);
}


template <class Archive>
void I3LOMLaunch::serialize(Archive& ar, unsigned int version) {

  if (version > i3lomlaunch_version_) {
    log_fatal("Attempting to read version %u from file"
              " but running version %u of I3LOMLaunch class.",
              version, i3lomlaunch_version_);
  }

  ar & make_nvp("time", time_);
  ar & make_nvp("waveformHighGain", adcDataHighGain_);
  ar & make_nvp("waveformLowGain", adcDataLowGain_);
}

bool I3LOMLaunch::operator==(const I3LOMLaunch& rhs) const {
  bool retVal = (
      time_.second == rhs.time_.second &&
      CompareFloatingPoint::Compare(time_.first, rhs.time_.first) &&
      time_.second == rhs.time_.second
    );

  if(IsADCDataFilled()) {
      retVal = (adcDataHighGain_ == rhs.adcDataHighGain_) && retVal;
      retVal = (adcDataLowGain_ == rhs.adcDataLowGain_) && retVal;
  }

  return retVal;
}


I3_SERIALIZABLE(I3LOMLaunch);
I3_SERIALIZABLE(I3LOMLaunchSeriesMap);
