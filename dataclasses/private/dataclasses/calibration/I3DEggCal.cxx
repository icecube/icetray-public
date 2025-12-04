/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Serialization and Printing for LinearityParameters,
 * I3DEggCal, and I3DEggCalMap Classes
 *
 * @file I3DEggCal.cxx
 * @date 2025-10-23
 * @author lbloom12
 *
 */

#include <icetray/serialization.h>
#include <dataclasses/calibration/I3DEggCal.h>


// serialization for LinearityParameters for proper storaging
template <class Archive>
void LinearityParameters::serialize(Archive& ar, unsigned version)
{
  if (version > linearity_params_version_)
    log_fatal("Attempting to read version %u from file but running version %u of LinearityParameters class.", version, linearity_params_version_);

  ar & make_nvp("linearityP0", p0);
  ar & make_nvp("linearityP1", p1);
  ar & make_nvp("linearityP2", p2);
}
I3_SERIALIZABLE(LinearityParameters);



// serialization for I3DEggCal for proper storaging
template <class Archive>
void I3DEggCal::serialize(Archive& ar, unsigned version)
{
  if (version > i3degg_calibration_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3DEggCal class.", version, i3degg_calibration_version_);

  ar & make_nvp("linearityParams", linearityParams);
  ar & make_nvp("hvGainRelation", hvGainRelation);
  ar & make_nvp("pmtTransitTime", pmtTransitTime);
  ar & make_nvp("pmtTransitTimeSpread", pmtTransitTimeSpread);
  ar & make_nvp("dacBaselineRelation", dacBaselineRelation);
  ar & make_nvp("temperature", temperature);
  ar & make_nvp("tauParams", tauParams);
}
I3_SERIALIZABLE(I3DEggCal);


// serialization for I3DEggCalMap for proper storaging
I3_SERIALIZABLE(I3DEggCalMap);



// define the printing operator for LinearityParameters for a better output than the address of the object
std::ostream& operator<<(std::ostream& oss, const LinearityParameters& p)
{
  oss << "[LinearityParameters: p0(" << p.p0 << ") p1(" << p.p1
      << ") p2(" << p.p2 << ")]" ;
  return oss;
}




// define the printing operator for I3DEggCal for a better output than the address of the object
std::ostream& operator<<(std::ostream& oss, const I3DEggCal& c)
{
  oss << "[         I3DEggCal :: " << std::endl
      << "         Temperature : " << c.temperature << std::endl
      << " ADC Counts-to-Volts : " << c.adcToVolts << std::endl
      << "         Sample Rate : " << c.sampleRate << std::endl
      << " Front End Impedance : " << c.frontEndImpedance << std::endl
      << "    DEgg Time Offset : " << c.deggTimeOffset << std::endl
      << "Linearity Parameters : " << c.linearityParams << std::endl
      << "         HV-Gain Fit : " << c.hvGainRelation << std::endl
      << "    PMT Transit Time : " << c.pmtTransitTime << std::endl
      << " Transit Time Spread : " << c.pmtTransitTimeSpread << std::endl
      << "    DAC-Baseline Fit : " << c.dacBaselineRelation << std::endl
      << "Droop Tau Parameters : " << c.tauParams << std::endl
      << "]" ;
  return oss;
}


// define the printing operator for I3DEggCalMap for a better output than the address of the object
std::ostream& operator<<(std::ostream& oss, const I3DEggCalMap& m)
{
  oss << "[ I3DEggCalMap :: " << std::endl;
  I3DEggCalMap::const_iterator iter = m.begin();
  for (;iter != m.end();iter++)
  {
    oss << "  " << iter->first << " : " << iter->second << std::endl;
  }
  oss << "]" ;
  return oss;
}
