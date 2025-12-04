/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Serialization and Printing for I3PMTCal and I3PMTCalMap Classes
 *
 * @file I3PMTCal.cxx
 * @date 2025-08-21
 * @author lbloom12
 *
 */


#include <icetray/serialization.h>
#include <dataclasses/calibration/I3PMTCal.h>


// serialization for I3PMTCal for proper storaging
template <class Archive>
void I3PMTCal::serialize(Archive& ar, unsigned version)
{
  if (version > i3pmt_calibration_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3PMTCal class.", version, i3pmt_calibration_version_);

  ar & make_nvp("relativePmtEff", relativePmtEff);
  ar & make_nvp("noiseRate", noiseRate);
}
I3_SERIALIZABLE(I3PMTCal);


// serialization for I3PMTCalMap for proper storaging
I3_SERIALIZABLE(I3PMTCalMap);



// define the printing operator for I3PMTCal for a better output than the address of the object
std::ostream& operator<<(std::ostream& oss, const I3PMTCal& c)
{
  oss << "[             I3PMTCal :: " << std::endl
      << "Relative PMT Efficiency : " << c.relativePmtEff << std::endl
      << "         PMT Noise Rate : " << c.noiseRate << std::endl
      << "]" ;
  return oss;
}


// define the printing operator for I3PMTCalMap for a better output than the address of the object
std::ostream& operator<<(std::ostream& oss, const I3PMTCalMap& m)
{
  oss << "[ I3PMTCalMap :: " << std::endl;
  I3PMTCalMap::const_iterator iter = m.begin();
  for (;iter != m.end();iter++)
  {
    oss << "  " << iter->first << " : " << iter->second << std::endl;
  }
  oss << "]" ;
  return oss;
}
