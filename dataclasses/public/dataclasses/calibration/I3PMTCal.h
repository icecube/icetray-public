/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Definition of the Generic Calibration Class
 *
 * @file I3PMTCal.h
 * @date 2026-03-26
 * @author lbloom12
 *
 */


#ifndef I3PMTCAL_H_INCLUDED
#define I3PMTCAL_H_INCLUDED

#include <icetray/OMKey.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/external/CompareFloatingPoint.h>
#include <dataclasses/calibration/SPEChargeDistribution.h>

static const unsigned i3pmt_calibration_version_ = 1;


/**
 * @brief A struct to hold generic calibration parameters
 * for all PMTs in the detector
 */
struct I3PMTCal {

  /**
   * Relative PMT efficiency, normalized to 1.0 for the average PMT (type-dependant).
   * Value is relative to nominal efficiency for the specified PMT model
   */
  double relativePmtEff;

  /**
   * Measure PMT noise rate, in Hz. Used in old noise-generator
   */
  double noiseRate;

  /**
   * The fit parameters for the Single Photoelectron (SPE) distribution of charge.
   * The SPEChargeDistribution type allows for an arbitrary number of exponential and
   * gaussian PDFs to be included upon initialization.
   * See /dataclasses/resources/docs/spechargedistribution.rst for documentation
   */
  SPEChargeDistribution speChargeDist;



  // Comparison operators
  bool operator==(const I3PMTCal& rhs) const {
    return (CompareFloatingPoint::Compare_NanEqual(relativePmtEff, rhs.relativePmtEff) &&
    CompareFloatingPoint::Compare_NanEqual(noiseRate, rhs.noiseRate) &&
    speChargeDist == rhs.speChargeDist);
  }
  bool operator!=(const I3PMTCal& rhs) const {
    return !operator==(rhs);
  }


  // serialization
  template <class Archive>
  void serialize(Archive& ar, unsigned version);


  // constructor
  I3PMTCal()
  {
    relativePmtEff = NAN;
    noiseRate = NAN;
  }

};
I3_CLASS_VERSION(I3PMTCal, i3pmt_calibration_version_);
I3_POINTER_TYPEDEFS(I3PMTCal);


// create a map of OMKey to I3PMTCal
typedef I3Map<OMKey, I3PMTCal> I3PMTCalMap;
I3_POINTER_TYPEDEFS(I3PMTCalMap);


// allows for the printing of objects in these classes
std::ostream& operator<<(std::ostream& oss, const I3PMTCal& c);
std::ostream& operator<<(std::ostream& oss, const I3PMTCalMap& m);

#endif //I3PMTCAL_H_INCLUDED
