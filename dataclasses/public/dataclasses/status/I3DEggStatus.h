/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Definition of I3DEggStatus and I3DEggStatusMap Classes
 *
 * @file I3DEggStatus.h
 * @date 2026-03-26
 * @author lbloom12
 *
 */


// initiate the wrapper
#ifndef I3DEGGSTATUS_H_INCLUDED
#define I3DEGGSTATUS_H_INCLUDED


#include <icetray/OMKey.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/external/CompareFloatingPoint.h>


static const unsigned i3deggstatus_version_ = 1;


struct I3DEggStatus {

  /**
   * Channel on/off switch. Is the PMT sending readouts?
   */
  bool enabled;

  /**
   * The different kinds of triggers that can be used for the DEgg PMTs:
   * 
   * - Analog-to-Digital Converter (ADC) = trigger whenever the counts in a
   *                                       single bin exceeds threshold
   * 
   * - Finite Impulse Response (FIR) = add up the counts from a set width of bins, 
   *                                   weighted by their coefficients, and trigger 
   *                                   whenever this sum exceeds a count threshold 
   *                                   (done as to combat PMT noise so that a noisy
   *                                   count spike in a single bin does not trigger)
   */
  enum DEggPMTTrigMode { UnknownTrigMode = -1 , ADC = 0, FIR = 1 };


  /**
   * DEgg PMT internal trigger mode
   */
  DEggPMTTrigMode trigMode;

  /**
   * threshold for the number of counts integrated over the width of samples
   */
  uint16_t trigThreshold;

  /**
   * The coefficients of the FIR filter, comprised of 16 8-bit two’s complement signed integers
   *
   * More detailed description of how this is implemented in Feature Extraction is found on pg.47 of:
   * https://uwprod.sharepoint.com/:w:/r/sites/icecubeupgrade/_layouts/15/Doc.aspx?sourcedoc=%7B7890b5f7-e337-4973-8d57-517a5e04bb02%7D&action=view&wdAccPdf=0&wdparaid=77FCD26
   */
  int8_t firCoefficients[16];

  /**
   * The DEgg PMT can readout its collected data with either a fixed number of samples in a launch
   * or with a variable number of samples in a launch (configured with preSamples & postSamples)
   */
  enum DEggReadoutMode { UnknownReadoutMode = -1 , FIXED_LENGTH = 0, VARIABLE_LENGTH = 1 };

  /**
   * The readout mode for launches from this PMT
   */
  DEggReadoutMode readoutMode;
  
  /**
   * number of samples included in the readout BEFORE the sample that was triggered (preSamples)
   * number of samples included in the readout AFTER the trigger condition is no longer satisfied (postSamples)
   */
  uint8_t preSamples;
  uint8_t postSamples;

  /**
   * Real operating PMT HV (Volts)
   */
  double pmtHV;

  /**
   * DAC setting used for shifting the waveform baseline
   */
  uint16_t baselineDAC;



  // destructor
  ~I3DEggStatus();

  // constructor
  I3DEggStatus():
    enabled(true),
    trigMode(UnknownTrigMode),
    trigThreshold(0),
    readoutMode(UnknownReadoutMode),
    preSamples(0),
    postSamples(0),
    pmtHV(NAN),
    baselineDAC(0)
  {
    for (unsigned i = 0; i < 16; ++i) {
      firCoefficients[i] = 0;
    }
  };



  // comparison operators
  bool operator==(const I3DEggStatus& rhs) const
  {
    // check to see if all FIR coefficients are equal
    bool firCoefficients_equal = true;
    for (unsigned i = 0; i < 16; ++i) {
      if (firCoefficients[i] != rhs.firCoefficients[i]) {
        firCoefficients_equal = false;
        break;
      }
    }

    // return equal only if all parameters are individually equivalent
    return (enabled == rhs.enabled &&
            trigMode == rhs.trigMode &&
            trigThreshold == rhs.trigThreshold &&
            firCoefficients_equal &&
            readoutMode == rhs.readoutMode &&
            preSamples == rhs.preSamples &&
            postSamples == rhs.postSamples &&
            CompareFloatingPoint::Compare_NanEqual(pmtHV, rhs.pmtHV) &&
            baselineDAC == rhs.baselineDAC);
  }
  bool operator!=(const I3DEggStatus& rhs) const
  {
    return !operator==(rhs);
  }


  // A printing function for the firCoefficients array of integers
  std::string PrintFIRCoefficients() const;

  // A printing function to display the DEggPMTTrigMode member assigned to trigMode as a string
  std::string PrintTrigMode() const;

  // A printing function to display the DEggReadoutMode member assigned to readoutMode as a string
  std::string PrintReadoutMode() const;
  
 
  // set up serialization
  template <class Archive> void serialize(Archive& ar, unsigned version);

};
I3_POINTER_TYPEDEFS(I3DEggStatus);
I3_CLASS_VERSION(I3DEggStatus, i3deggstatus_version_);


// create a map of OMKey to I3DEggStatus
typedef I3Map<OMKey, I3DEggStatus> I3DEggStatusMap;
I3_POINTER_TYPEDEFS(I3DEggStatusMap);


// allows the printing of objects in these classes
std::ostream& operator<<(std::ostream& oss, const I3DEggStatus& s);
std::ostream& operator<<(std::ostream& oss, const I3DEggStatusMap& m);

#endif //I3DEGGSTATUS_H_INCLUDED
