/**
 *  $Id$
 *
 *  Copyright (C) 2016
 *  Claudio Kopper <ckopper@icecube.wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 */

#include "dataclasses/I3RecoPulseSeriesMapApplySPECorrection.h"
#include "dataclasses/physics/I3RecoPulse.h"
#include "dataclasses/calibration/I3Calibration.h"
#include "boost/make_shared.hpp"
#include "boost/foreach.hpp"

I3RecoPulseSeriesMapApplySPECorrection::I3RecoPulseSeriesMapApplySPECorrection() 
  : pulses_key_(), calibration_key_(), shifted_() {}

I3RecoPulseSeriesMapApplySPECorrection::I3RecoPulseSeriesMapApplySPECorrection(
  const std::string &pulses_key,
  const std::string &calibration_key) 
  : pulses_key_(pulses_key), calibration_key_(calibration_key), shifted_() {}

I3RecoPulseSeriesMapConstPtr
I3RecoPulseSeriesMapApplySPECorrection::Apply(const I3Frame &frame) const
{
  typedef I3RecoPulseSeriesMap Map;
  typedef boost::shared_ptr<const Map> MapConstPtr;
  typedef Map::value_type Pair;
  typedef Pair::second_type Series;
  typedef Series::value_type Element;
 
  if (shifted_)
    return shifted_;

  I3CalibrationConstPtr calibration = frame.Get<I3CalibrationConstPtr>(calibration_key_);
  if (!calibration)
    log_fatal("Couldn't find '%s' in the frame!",
        calibration_key_.c_str());

  MapConstPtr in_pulses = frame.Get<MapConstPtr>(pulses_key_);
  if (!in_pulses)
    log_fatal("Couldn't find '%s' in the frame!",
        pulses_key_.c_str());

  I3RecoPulseSeriesMapPtr shifted = boost::make_shared<Map>();

  BOOST_FOREACH(const Pair &pair, *in_pulses) {
    // retrieve the calibration for this DOM
    std::map<OMKey, I3DOMCalibration>::const_iterator calib =
      calibration->domCal.find(pair.first);
    if (calib == calibration->domCal.end()) 
      log_fatal("Could not find DOM (%i/%u) in '%s'",
          pair.first.GetString(), pair.first.GetOM(),
          calibration_key_.c_str());
    const I3DOMCalibration& dom_calibration = calib->second;

    // Load the SPE corrections
    double atwdSPECorrection = 1.;
    double fadcSPECorrection = 1.;
    if (dom_calibration.IsMeanATWDChargeValid()) {
      atwdSPECorrection = 1. / dom_calibration.GetMeanATWDCharge();
    }
    if (dom_calibration.IsMeanFADCChargeValid()) {
      fadcSPECorrection = 1. / dom_calibration.GetMeanFADCCharge();
    }

    // insert an entry for this DOM into the output map
    // and retrieve a reference
    Series &shiftedvec = (*shifted)[pair.first];

    BOOST_FOREACH(const Element &element, pair.second) {
      // plain copy of the pulse first
     shiftedvec.push_back(element);
 
      // retrieve a reference to the new element
      Element &shifted_element = shiftedvec.back();

      // apply the correct correction for this pulse (ATWD or FADC)
      double speCorrection = element.GetFlags() & I3RecoPulse::ATWD ?
        atwdSPECorrection : fadcSPECorrection;

      // now set the shifted charge
      shifted_element.SetCharge(element.GetCharge() * speCorrection);
    }
  }
  
  // save in cache
  shifted_ = shifted;

  return shifted_;
}

bool
I3RecoPulseSeriesMapApplySPECorrection::operator==(
  const I3RecoPulseSeriesMapApplySPECorrection& other) const
{
  return (pulses_key_ == other.pulses_key_) && 
         (calibration_key_ == other.calibration_key_);
}

bool
I3RecoPulseSeriesMapApplySPECorrection::operator!=(
  const I3RecoPulseSeriesMapApplySPECorrection& other) const
{
  return (pulses_key_ != other.pulses_key_) ||
         (calibration_key_ != other.calibration_key_);
}

std::ostream& I3RecoPulseSeriesMapApplySPECorrection::Print(std::ostream& os) const
{
  os << "Pulses: " << pulses_key_ << " Calibration: " << calibration_key_;
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3RecoPulseSeriesMapApplySPECorrection& corr)
{
  return(corr.Print(os));
}

template <class Archive>
void
I3RecoPulseSeriesMapApplySPECorrection::serialize(Archive& ar, unsigned version)
{
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("PulsesKey", pulses_key_);
  ar & make_nvp("CalibrationKey", calibration_key_);
}

I3_SERIALIZABLE(I3RecoPulseSeriesMapApplySPECorrection);
