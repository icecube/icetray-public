/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Serialization and Printing for I3DEggStatus and I3DEggStatusMap Classes
 *
 * @file I3DEggStatus.cxx
 * @date 2026-03-26
 * @author lbloom12
 *
 */


#include <icetray/serialization.h>
#include <dataclasses/status/I3DEggStatus.h>


I3DEggStatus::~I3DEggStatus() {}


// serialization for I3DEggStatus for proper storaging
template <class Archive>
void I3DEggStatus::serialize (Archive& ar, const unsigned version)
{
  if (version>i3deggstatus_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3DEggStatus class.",version,i3deggstatus_version_);

  ar & make_nvp("enabled", enabled);
  ar & make_nvp("trigMode", trigMode);
  ar & make_nvp("trigThreshold", trigThreshold);
  ar & make_nvp("firCoefficients", firCoefficients);
  ar & make_nvp("preSamples", preSamples);
  ar & make_nvp("postSamples", postSamples);
  ar & make_nvp("pmtHV", pmtHV);
  ar & make_nvp("baselineDAC", baselineDAC);

  if (version > 0) {
      ar & make_nvp("readoutMode", readoutMode);
  }

}
I3_SERIALIZABLE(I3DEggStatus);


// serialization for I3DEggStatusMap for proper storaging
I3_SERIALIZABLE(I3DEggStatusMap);


// define a printing function for the firCoefficients array of integers
std::string I3DEggStatus::PrintFIRCoefficients() const {
  std::stringstream ss;
  ss << "(";
  for (unsigned i = 0; i < 16; ++i) {
      ss << short(firCoefficients[i]);
      if (i < 15) {
          ss << ", ";
      }
  }
  ss << ")";
  return ss.str();
}


// define a printing function for the trigMode
std::string I3DEggStatus::PrintTrigMode() const {
    switch (trigMode) {
        case I3DEggStatus::ADC: return "ADC";
        case I3DEggStatus::FIR: return "FIR";
        default:                return "UnknownTrigMode";
    }
}


// define a printing function for the readoutMode
std::string I3DEggStatus::PrintReadoutMode() const {
    switch (readoutMode) {
        case I3DEggStatus::FIXED_LENGTH:    return "FIXED_LENGTH";
        case I3DEggStatus::VARIABLE_LENGTH: return "VARIABLE_LENGTH";
        default:                            return "UnknownReadoutMode";
    }
}


// define the printing operator for I3DEggStatus
// for a better output than the address of the object
std::ostream& operator<<(std::ostream& oss, const I3DEggStatus& s)
{
  oss << "[      I3DEggStatus :: " << std::endl
      << "         PMT enabled : " << s.enabled << std::endl
      << "    PMT Trigger Mode : " << s.PrintTrigMode() << std::endl
      << "   Trigger Threshold : " << s.trigThreshold << std::endl
      << "    FIR Coefficients : " << s.PrintFIRCoefficients() << std::endl
      << "        Readout Mode : " << s.PrintReadoutMode() << std::endl
      << " Samples Pre-Trigger : " << +s.preSamples << std::endl
      << "Samples Post-Trigger : " << +s.postSamples << std::endl
      << "              PMT HV : " << s.pmtHV << std::endl
      << "DAC Baseline setting : " << s.baselineDAC << std::endl
      << "]" ;
  return oss;
}


// define the printing operator for I3DEggStatusMap
// for a better output than the address of the object
std::ostream& operator<<(std::ostream& oss, const I3DEggStatusMap& m)
{
  oss << "[ I3DEggStatusMap :: " << std::endl;
  I3DEggStatusMap::const_iterator iter = m.begin();
  for (;iter != m.end();iter++)
  {
    oss << "  " << iter->first << " : " << iter->second << std::endl;
  }
  oss << "]" ;
  return oss;
}
