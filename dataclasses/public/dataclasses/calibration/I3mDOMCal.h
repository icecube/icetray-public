/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Definition of I3mDOMCal Class
 *
 * @file I3mDOMCal.h
 * @date 2026-4-30
 * @author lbloom12
 *
 */


// initiate the wrapper
#ifndef I3MDOMCALIBRATION_H_INCLUDED
#define I3MDOMCALIBRATION_H_INCLUDED


#include <dataclasses/calibration/I3DEggCal.h>


static const unsigned i3mdom_calibration_version_ = 2;


/**
 * @brief A struct to hold the mDOM PMT Calibration parameters
 */
struct I3mDOMCal {

    /**
     * constant values for all mDOM modules which are described in:
     * https://github.com/WIPACrepo/STM32Workspace/blob/master/wf-processing/include/wf-processing/mdom/mdom_constants.h
     */ 
    static const double adcToVolts;         // (Volts / ADC counts) MDOM_CNT_TO_V_FACTOR in mdom_constants.h
    static const double sampleRate;         // (MHz) mDOM digitizer Sample Rate
    static const double frontEndImpedance;  // (Ohms) MDOM_FRONT_END_IMPEDANCE in mdom_constants.h

    static const double mdomTimeOffset;     // (ns) Systematic timing offset between mDOMs and Gen1 DOMs

    static const double discSampleRate;     // (MHz) mDOM discriminator Sample Rate


    /// the slope and intercept of:   log(gain) = m*log(HV) + b
    LinearFit hvGainRelation;

    /// (ns) Time offset between the start of a pulse and when the photon initially hit the PMT
    double pmtTransitTime;

    /// (ns) variance of the transit time distribution about the mean
    double pmtTransitTimeSpread;

    /// the uncertainty in the waveform baseline, measured in ADC Counts.
    double adcBaselineRMS;

    /**
     * Since the DAC has significant hysteresis, the Baseline/DAC calibration will
     * simply set the DAC to the appropriate value to get the desired ADC baseline.
     * This yields a baseline value/DAC value pair, where the baseline value is valid
     * if and only if the DAC value matches.
     */
    double adcBaselineValue; // the requested baseline value in the calibration
    uint16_t adcBaselineDAC; // the DAC value returned by the calibration
    double GetValidBaselineValue(uint16_t baselineDACVal) {
        if (adcBaselineDAC == baselineDACVal) {
            return adcBaselineValue;
        } else {
            throw std::runtime_error("The specified DAC value does not match what is in calibration!");
        }
    }

    /*
     * Same as above, the Discriminator/DAC calibration identifies the DAC value where
     * the discriminator is at 0 Volts, then uses the known hardware relation to set
     * the discriminator to a prescribed voltage.
     */
    double discThreshold; // (Volts) the requested discriminator threshold in the calibration
    uint16_t discDAC;     // the DAC value returned by the calibration
    double GetValidDiscThreshold(uint16_t discDACVal) {
        if (discDAC == discDACVal) {
            return discThreshold;
        } else {
            throw std::runtime_error("The specified DAC value does not match what is in calibration!");
        }
    }


    // Comparison operators
    bool operator==(const I3mDOMCal& rhs) const {
        return (hvGainRelation == rhs.hvGainRelation &&
        CompareFloatingPoint::Compare_NanEqual(pmtTransitTime, rhs.pmtTransitTime) &&
        CompareFloatingPoint::Compare_NanEqual(pmtTransitTimeSpread, rhs.pmtTransitTimeSpread) &&
        CompareFloatingPoint::Compare_NanEqual(adcBaselineRMS, rhs.adcBaselineRMS) &&
        CompareFloatingPoint::Compare_NanEqual(adcBaselineValue, rhs.adcBaselineValue) &&
        adcBaselineDAC == rhs.adcBaselineDAC &&
        CompareFloatingPoint::Compare_NanEqual(discThreshold, rhs.discThreshold) &&
        discDAC == rhs.discDAC);
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
        adcBaselineRMS = NAN;
        adcBaselineValue = NAN;
        adcBaselineDAC = 0;
        discThreshold = NAN;
        discDAC = 0;
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
