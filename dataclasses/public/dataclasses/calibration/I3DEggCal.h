/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Definition of I3DEggCal Class
 *
 * @file I3DEggCal.h
 * @date 2026-2-6
 * @author lbloom12
 *
 */


// initiate the wrapper
#ifndef I3DEGGCALIBRATION_H_INCLUDED
#define I3DEGGCALIBRATION_H_INCLUDED


#include <dataclasses/I3Map.h>
#include <dataclasses/calibration/I3DOMCalibration.h>


static const unsigned linearity_params_version_ = 0;
static const unsigned i3degg_calibration_version_ = 1;



/**
 * @brief A struct to hold the PMT Linearity parameters as described in
 * the DEgg Paper (https://arxiv.org/pdf/2212.14526) Equation 4.1
 * 
 * Relationship between the observed peak current (I_obs) versus the
 * ideal peak current (I_ideal = x) can be modeled as:
 * 
 * I_obs = x * ln(1 + x/p2) / ( ln(1 + x/p2) + (x/p0)*ln(1 + (x/p1)^3) )
 * 
 * where all variables and parameters are measured in milliamps (mA)
 */
struct LinearityParameters
{
  double p0;
  double p1;
  double p2;

  // comparison operators
  bool operator==(const LinearityParameters& rhs) const
  {
    return (CompareFloatingPoint::Compare_NanEqual(p0,rhs.p0) &&
        CompareFloatingPoint::Compare_NanEqual(p1,rhs.p1) &&
        CompareFloatingPoint::Compare_NanEqual(p2,rhs.p2));
  }
  bool operator!=(const LinearityParameters& rhs) const
  {
    return !operator==(rhs);
  }

  // serialization
  template <class Archive>
  void serialize(Archive& ar, unsigned version);

  // constructor
  LinearityParameters()
  {
    p0 = NAN;
    p1 = NAN;
    p2 = NAN;
  }
};
I3_CLASS_VERSION(LinearityParameters, linearity_params_version_);





/**
 * @brief A struct to hold the DEgg PMT Calibration parameters
 */
struct I3DEggCal {
  
    /**
     * constant values for all DEgg modules which are described in:
     * https://github.com/WIPACrepo/STM32Workspace/blob/master/wf-processing/include/wf-processing/degg/degg_constants.h
     */ 
    static const double adcToVolts;         // (Volts / ADC counts) DEGG_CNT_TO_V_FACTOR in degg_constants.h
    static const double sampleRate;         // (MHz) DEgg digitizer Sample Rate
    static const double frontEndImpedance;  // (Ohms) DEGG_FRONT_END_IMPEDANCE in degg_constants.h

    static const double deggTimeOffset;     // (ns) Systematic timing offset between DEggs and Gen1 DOMs

    /**
     * Linearity parameters as described in:
     * DEgg Paper (https://arxiv.org/pdf/2212.14526) Equation 4.1
     */
    LinearityParameters linearityParams;

    /// the slope and intercept of:   log(gain) = m*log(HV) + b
    LinearFit hvGainRelation;

    /// Time offset between the start of a pulse and when the photon initially hit the PMT
    double pmtTransitTime;

    /// variance of the transit time distribution about the mean
    double pmtTransitTimeSpread;

    /// the slope and intercept of:   ADC baseline = m*(DAC setting) + b
    LinearFit dacBaselineRelation;

    /// temperature of the Mainboard
    double temperature;

    /**
     * TauParam is defined in I3DOMCalibration.h and includes 7 parameters
     * that describe two different time-constants. The time-constants themselves
     * describe the electronic droop correction from the toroid and are of the form:
     *
     *     tau1(T) = P0 + ( P1/( 1 + exp(-T/P2) ))
     *     tau2(T) = P3 + ( P4/( 1 + exp(-T/P5) ))
     *
     * which are functions of the temperature of the mainboard T
     * 
     * These are combined by setting a TauFrac so that the electronics behave as:
     *
     *     (1 - TauFrac) * exp(-t/tau1) + (TauFrac) * exp(-t/tau2)
     * 
     * We only require one time-constant to parameterize the droop correction
     * in the DEgg which is achieved by setting TauFrac to zero and only setting
     * the first three parameters (P0, P1, P2) where P0 and P1 are measured in 
     * microseconds (us), P2 and T are measured in degrees Celsius
     */
    TauParam tauParams;
  



    // Comparison operators
    bool operator==(const I3DEggCal& rhs) const {
        return (linearityParams == rhs.linearityParams &&
        hvGainRelation == rhs.hvGainRelation &&
        CompareFloatingPoint::Compare_NanEqual(pmtTransitTime, rhs.pmtTransitTime) &&
        CompareFloatingPoint::Compare_NanEqual(pmtTransitTimeSpread, rhs.pmtTransitTimeSpread) &&
        dacBaselineRelation == rhs.dacBaselineRelation &&
        CompareFloatingPoint::Compare_NanEqual(temperature, rhs.temperature) &&
        tauParams == rhs.tauParams);
    }
    bool operator!=(const I3DEggCal& rhs) const {
        return !operator==(rhs);
    }


    // serialization
    template <class Archive>
    void serialize(Archive& ar, unsigned version);


    // constructor
    I3DEggCal() : tauParams(NAN, // P0 (default value)
		  	    NAN, // P1 (default value)
			    NAN, // P2 (default value)
			    1,   // P3 (not used)
			    0,   // P4 (not used)
			    1,   // P5 (not used)
			    0)   // TauFrac (set to zero)
      {
        pmtTransitTime = NAN;
        pmtTransitTimeSpread = NAN;
        temperature = NAN;
    }

};
I3_POINTER_TYPEDEFS(I3DEggCal);
I3_CLASS_VERSION(I3DEggCal, i3degg_calibration_version_);


// create a map of OMKey to I3DEggCal
typedef I3Map<OMKey, I3DEggCal> I3DEggCalMap;
I3_POINTER_TYPEDEFS(I3DEggCalMap);


// allows the printing of objects in these classes
std::ostream& operator<<(std::ostream& oss, const LinearityParameters& p);
std::ostream& operator<<(std::ostream& oss, const I3DEggCal& c);
std::ostream& operator<<(std::ostream& oss, const I3DEggCalMap& m);


// end the wrapper
#endif
