/**
 * copyright  (C) 2006
 * the icecube collaboration
 * @version $Id$
 * @file I3DOMFunctions.h
 * @date $Date$
 */
#ifndef I3DOMFUNCTIONS_H_INCLUDED
#define I3DOMFUNCTIONS_H_INCLUDED

//includes
#include "dataclasses/status/I3DOMStatus.h"
#include "dataclasses/calibration/I3DOMCalibration.h"
#include "dataclasses/physics/I3DOMLaunch.h"

#include <vector>

/**
 * manually determined relationship between
 * collected charge in picocoulumb and peak voltage in millivolt
 */
#define __SPE_PEAK_VOLTAGE_CHARGE_RATIO__ 8.1

/** 
 * @brief  A class containing a collection of functions that 
 *  use dom status/calibration information to return "calculated"
 *  status items (Gain, SPEMean, etc)
 *
 * expandible in the future, as needed
 */

/**
 * Return the ATWD sampling rate for chip 0 or 1
 */
double ATWDSamplingRate (unsigned int chip, const I3DOMStatus& , const I3DOMCalibration&);

/**
 * Return the calculated PMT gain
 */
double PMTGain (const I3DOMStatus& , const I3DOMCalibration&);

/**
 * Return the calculated SPEMean (the ADC "charge", in GV ns, corresponding to
 * the calibrated value of 1 PE). This converts ADC amplitudes into units of
 * photoelectrons.
 */
double SPEMean (const I3DOMStatus& , const I3DOMCalibration&);

/**
 * Return the expectation value of the SPE distribution, in PE, for this DOM.
 * Whereas 1 PE (see above) is the most likely value for one PE of charge,
 * the usual mean charge per PE is usually somewhat less. This can be used to
 * estimate the number of photons responsible for a given number of
 * photoelectrons.
 */
double MeanSPECharge (const I3DOMCalibration&);

/**
 * Get the FADC baseline (in counts)
 */
double FADCBaseline (const I3DOMStatus& , const I3DOMCalibration&);

/**
 * Get the total transit time (PMT transit time + delay)
 */
double TransitTime (const I3DOMStatus&, const I3DOMCalibration&);

/**
 * Get the calibrated SPE Discrimiator threshold (pC)
 */
double SPEDiscriminatorThreshold (const I3DOMStatus&, const I3DOMCalibration&, double const spePeakVoltageChargeRatio =  __SPE_PEAK_VOLTAGE_CHARGE_RATIO__);

/**
 *  Updated approximation function for SPE DOM threshold from D.Chirkin
 */
double SPEPMTThreshold (const I3DOMStatus&, const I3DOMCalibration&);

/**
 * Get the calibrated MPE Discrimiator threshold (pC)
 */
double MPEDiscriminatorThreshold (const I3DOMStatus&, const I3DOMCalibration&, double const spePeakVoltageChargeRatio =  __SPE_PEAK_VOLTAGE_CHARGE_RATIO__);

/**
 *  OldspeThreshold - a function to return the old "psuedo calibration" values
 *          from the raw DAC values (now stored in DOMStatus record). Previously,
 *          these psuedo-calibrations were applied by I3OmDb and stored in
 *          DOMStatus.  This changed with offline V02-02-00 releases.
 * 
 *      Returns the speThresold value in I3Units::Volt
 */
double OldspeThreshold(const I3DOMStatus&);

/**
 *  OldmpeThreshold - a function to return the old "psuedo calibration" values
 *          from the raw DAC values (now stored in DOMStatus record). Previously,
 *          these psuedo-calibrations were applied by I3OmDb and stored in
 *          DOMStatus.  This changed with offline V02-02-00 releases.
 * 
 *      Returns the mpeThresold value in I3Units::Volt
 */
double OldmpeThreshold(const I3DOMStatus&);


/**
 * Given a string which contains the DOMCal version (e.g. "6.1.0"), return a vector<int>
 * which contains the numbers as its elements (i.e. what's between the separating "."s)
 */
std::vector<int> DOMCalVersion (const I3DOMCalibration&);

/**
 * Converts a ATWDselect enumeration into ATWD ID.
 *  This corrects the problem that I3DOMLaunch::ATWDSelect is enumd 1 or 2
 *   while all I3DOMCalibration are enumerated as 0 or 1 for the 2 ATWD chips.
 *   Changing the enum in I3DOMLaunch would cause nasty serialization issues.
 *   This moves the conversion function that has always been in DOMCalibrator
 *   to DOMFunctions to be available to all.
 */

unsigned int WhichATWD(const I3DOMLaunch&);


#endif //I3DOMFUNCTIONS_H_INCLUDED
