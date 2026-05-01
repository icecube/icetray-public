/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Serialization and Printing for I3mDOMCal and I3mDOMCalMap Classes
 *
 * @file I3mDOMCal.cxx
 * @date 2026-4-30
 * @author lbloom12
 *
 */

#include <icetray/serialization.h>
#include <dataclasses/calibration/I3mDOMCal.h>
#include "icetray/I3Units.h"


/**
 * Define the constants in I3mDOMCal
 *
 * constant values for all mDOM modules which are described in:
 * https://github.com/WIPACrepo/STM32Workspace/blob/master/wf-processing/include/wf-processing/mdom/mdom_constants.h
 */
const double I3mDOMCal::adcToVolts = 0.045e-3 * I3Units::V;        // (Volts / ADC counts) MDOM_CNT_TO_V_FACTOR
const double I3mDOMCal::sampleRate = 120 * I3Units::megahertz;     // (MHz) mDOM digitizer Sample Rate
const double I3mDOMCal::frontEndImpedance = 75.35 * I3Units::ohm;  // (Ohms) MDOM_FRONT_END_IMPEDANCE

const double I3mDOMCal::mdomTimeOffset = NAN * I3Units::ns;        // (ns) Systematic timing offset between mDOMs and Gen1 DOMs

const double I3mDOMCal::discSampleRate = 960 * I3Units::megahertz; // (MHz) mDOM discriminator Sample Rate



// serialization for I3mDOMCal for proper storaging
template <class Archive>
void I3mDOMCal::serialize(Archive& ar, unsigned version)
{
  if (version > i3mdom_calibration_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3mDOMCal class.", version, i3mdom_calibration_version_);

  ar & make_nvp("hvGainRelation", hvGainRelation);
  ar & make_nvp("pmtTransitTime", pmtTransitTime);
  ar & make_nvp("pmtTransitTimeSpread", pmtTransitTimeSpread);

  if (version > 0) {
      ar & make_nvp("adcBaselineValue", adcBaselineValue);
      ar & make_nvp("adcBaselineDAC", adcBaselineDAC);
      ar & make_nvp("discThreshold", discThreshold);
      ar & make_nvp("discDAC", discDAC);
  }

  if (version > 1) {
      ar & make_nvp("adcBaselineRMS", adcBaselineRMS);
  }
}
I3_SERIALIZABLE(I3mDOMCal);


// serialization for I3mDOMCalMap for proper storaging
I3_SERIALIZABLE(I3mDOMCalMap);


// define the printing operator for I3mDOMCal for a better output than the address of the object
std::ostream& operator<<(std::ostream& oss, const I3mDOMCal& c)
{
  oss << "[            I3mDOMCal :: " << std::endl
      << "    ADC Counts-to-Volts : " << c.adcToVolts << std::endl
      << "            Sample Rate : " << c.sampleRate << std::endl
      << "    Front End Impedance : " << c.frontEndImpedance << std::endl
      << "       mDOM Time Offset : " << c.mdomTimeOffset << std::endl
      << "         DiscSampleRate : " << c.discSampleRate << std::endl
      << "            HV-Gain Fit : " << c.hvGainRelation << std::endl
      << "       PMT Transit Time : " << c.pmtTransitTime << std::endl
      << "    Transit Time Spread : " << c.pmtTransitTimeSpread << std::endl
      << "           Baseline RMS : " << c.adcBaselineRMS << std::endl
      << "Baseline DAC/Value Pair : " << c.adcBaselineDAC << " / " << c.adcBaselineValue << std::endl
      << "Disc DAC/Threshold Pair : " << c.discDAC << " / " << c.discThreshold << std::endl
      << "]" ;
  return oss;
}


// define the printing operator for I3mDOMCalMap for a better output than the address of the object
std::ostream& operator<<(std::ostream& oss, const I3mDOMCalMap& m)
{
  oss << "[ I3mDOMCalMap :: " << std::endl;
  I3mDOMCalMap::const_iterator iter = m.begin();
  for (;iter != m.end();iter++)
  {
    oss << "  " << iter->first << " : " << iter->second << std::endl;
  }
  oss << "]" ;
  return oss;
}
