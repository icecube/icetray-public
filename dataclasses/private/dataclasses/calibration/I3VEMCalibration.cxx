/**
 *
 * @version $Id$
 * @file I3VEMCalibration.cxx
 * @date $Date$
 */


#include <icetray/serialization.h>
#include "dataclasses/calibration/I3VEMCalibration.h"

I3VEMCalibration::~I3VEMCalibration() {}

template <class Archive>
void I3VEMCalibration::serialize(Archive& ar, unsigned version)
{
  if (version>i3vemcalibration_version_){
    log_fatal("Attempting to read version %u from file but running version %u of I3VEMCalibration class.",version,i3vemcalibration_version_);
  }
  ar & make_nvp("pePerVEM",      pePerVEM);
  ar & make_nvp("muPeakWidth",   muPeakWidth);
  ar & make_nvp("hglgCrossOver", hglgCrossOver);
  ar & make_nvp("corrFactor",    corrFactor);
}

std::ostream& operator<<(std::ostream& oss, const I3VEMCalibration& vc)
{
  oss << "[ I3VEMCalibration :: " << std::endl
      << "                pePerVEM  : " << vc.pePerVEM << std::endl
      << "                muPeakWidth  : " << vc.muPeakWidth << std::endl
      << "                hglgCrossOver  : " << vc.hglgCrossOver << std::endl
      << "                corrFactor  : " << vc.corrFactor << std::endl
      << "]" ;
  return oss;
}


I3_SERIALIZABLE(I3VEMCalibration);

