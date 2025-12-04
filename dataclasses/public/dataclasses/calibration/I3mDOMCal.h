/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Definition of I3mDOMCal Class
 *
 * @file I3mDOMCal.h
 * @date 2025-10-14
 * @author lbloom12
 *
 */


// initiate the wrapper
#ifndef I3MDOMCALIBRATION_H_INCLUDED
#define I3MDOMCALIBRATION_H_INCLUDED


#include <dataclasses/calibration/I3DEggCal.h>


static const unsigned i3mdom_calibration_version_ = 0;

/**
 * @brief A struct to hold the mDOM PMT Calibration parameters
 */
struct I3mDOMCal {

    /**
     * constant values for all mDOM modules which are described in:
     * https://github.com/WIPACrepo/STM32Workspace/blob/master/wf-processing/include/wf-processing/mdom/mdom_constants.h
     */ 
    static constexpr double adcToVolts = 0.045e-3;      // (Volts / ADC counts)
    static constexpr int16_t sampleRate = 120;          // (MHz) mDOM digitizer Sample Rate
    static constexpr double frontEndImpedance = 75.35;  // (Ohms)

    static constexpr double mdomTimeOffset = NAN;       // Systematic timing offset between mDOMs and Gen1 DOMs (ns)

    static constexpr int16_t discSampleRate = 960;      // (MHz) mDOM discriminator Sample Rate


    /**
     * Linearity parameters as described in:
     * DEgg Paper (https://arxiv.org/pdf/2212.14526) Equation 4.1
     * LinearityParameters are defined in I3DEggCal.h
     */
    LinearityParameters linearityParams;

    /// the slope and intercept of:   log(gain) = m*log(HV) + b
    LinearFit hvGainRelation;

    /// Time offset between the start of a pulse and when the photon initially hit the PMT
    double pmtTransitTime;

    /// variance of the transit time distribution about the mean
    double pmtTransitTimeSpread;



    // Comparison operators
    bool operator==(const I3mDOMCal& rhs) const {
        return (linearityParams == rhs.linearityParams &&
        hvGainRelation == rhs.hvGainRelation &&
        CompareFloatingPoint::Compare_NanEqual(pmtTransitTime, rhs.pmtTransitTime) &&
        CompareFloatingPoint::Compare_NanEqual(pmtTransitTimeSpread, rhs.pmtTransitTimeSpread));
    }
    bool operator!=(const I3mDOMCal& rhs) const {
        return !operator==(rhs);
    }


    // serialization
    template <class Archive>
    void serialize(Archive& ar, unsigned version);


    // constructor
    I3mDOMCal()
    {
        pmtTransitTime = NAN;
        pmtTransitTimeSpread = NAN;
    }

};
I3_POINTER_TYPEDEFS(I3mDOMCal);
I3_CLASS_VERSION(I3mDOMCal, i3mdom_calibration_version_);


// create a map of OMKey to I3mDOMCal
typedef I3Map<OMKey, I3mDOMCal> I3mDOMCalMap;
I3_POINTER_TYPEDEFS(I3mDOMCalMap);


// allows the printing of objects in these classes
std::ostream& operator<<(std::ostream& oss, const I3mDOMCal& c);
std::ostream& operator<<(std::ostream& oss, const I3mDOMCalMap& m);


// end the wrapper
#endif
