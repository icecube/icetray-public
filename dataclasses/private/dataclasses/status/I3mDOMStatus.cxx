/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Serialization and Printing for I3mDOMStatus and I3mDOMStatusMap Classes
 *
 * @file I3mDOMStatus.cxx
 * @date 2026-03-26
 * @author lbloom12
 *
 */


#include <icetray/serialization.h>
#include <dataclasses/status/I3mDOMStatus.h>


I3mDOMStatus::~I3mDOMStatus() {}


// serialization for I3mDOMStatus for proper storaging
template <class Archive>
void I3mDOMStatus::serialize (Archive& ar, const unsigned version)
{
  if (version>i3mdomstatus_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3mDOMStatus class.",version,i3mdomstatus_version_);

  ar & make_nvp("enabled", enabled);
  ar & make_nvp("TrigMode", trigMode);
  ar & make_nvp("adcThreshold", adcThreshold);
  ar & make_nvp("PreSamples", preSamples);
  ar & make_nvp("PostSamples", postSamples);
  ar & make_nvp("pmtHV", pmtHV);
  ar & make_nvp("adcBaselineDAC", adcBaselineDAC);
  ar & make_nvp("discDAC", discDAC);

  if (version > 0) {
      ar & make_nvp("readoutMode", readoutMode);
  }
}
I3_SERIALIZABLE(I3mDOMStatus);


// serialization for I3mDOMStatusMap for proper storaging
I3_SERIALIZABLE(I3mDOMStatusMap);


// define a printing function for the trigMode
std::string I3mDOMStatus::PrintTrigMode() const {
    switch (trigMode) {
        case I3mDOMStatus::ADC:  return "ADC";
        case I3mDOMStatus::Disc: return "Discriminator";
        default:                 return "UnknownTrigMode";
    }
}


// define a printing function for the readoutMode
std::string I3mDOMStatus::PrintReadoutMode() const {
    switch (readoutMode) {
        case I3mDOMStatus::FIXED_LENGTH:    return "FIXED_LENGTH";
        case I3mDOMStatus::VARIABLE_LENGTH: return "VARIABLE_LENGTH";
        default:                            return "UnknownReadoutMode";
    }
}


// define the printing operator for I3mDOMStatus
// for a better output than the address of the object
std::ostream& operator<<(std::ostream& oss, const I3mDOMStatus& s)
{
  oss << "[       I3mDOMStatus :: " << std::endl
      << "          PMT enabled : " << s.enabled << std::endl
      << "     PMT Trigger Mode : " << s.PrintTrigMode() << std::endl
      << "ADC Trigger Threshold : " << s.adcThreshold << std::endl
      << "         Readout Mode : " << s.PrintReadoutMode() << std::endl
      << "  Samples Pre-Trigger : " << +s.preSamples << std::endl
      << " Samples Post-Trigger : " << +s.postSamples << std::endl
      << "               PMT HV : " << s.pmtHV << std::endl
      << " DAC Baseline setting : " << s.adcBaselineDAC << std::endl
      << "     DAC Disc setting : " << s.discDAC << std::endl
      << "]" ;
  return oss;
}


// define the printing operator for I3mDOMStatusMap
// for a better output than the address of the object
std::ostream& operator<<(std::ostream& oss, const I3mDOMStatusMap& m)
{
  oss << "[ I3mDOMStatusMap :: " << std::endl;
  I3mDOMStatusMap::const_iterator iter = m.begin();
  for (;iter != m.end();iter++)
  {
    oss << "  " << iter->first << " : " << iter->second << std::endl;
  }
  oss << "]" ;
  return oss;
}
