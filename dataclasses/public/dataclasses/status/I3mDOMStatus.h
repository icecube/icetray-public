/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Definition of I3mDOMStatus Class
 *
 * @file I3mDOMStatus.h
 * @date 2026-03-26
 * @author lbloom12
 *
 */


// initiate the wrapper
#ifndef I3MDOMSTATUS_H_INCLUDED
#define I3MDOMSTATUS_H_INCLUDED


#include <dataclasses/I3Map.h>
#include <icetray/OMKey.h>
#include <dataclasses/external/CompareFloatingPoint.h>


static const unsigned i3mdomstatus_version_ = 1;



struct I3mDOMStatus {

  /**
   * Channel on/off switch. Is the PMT sending readouts?
  */
  bool enabled;

  /**
   * The different kinds of triggers that can be used for the mDOM PMTs:
   * 
   * - Analog-to-Digital Converter (ADC) = trigger whenever the counts in a
   *                                       single bin exceeds threshold 
   *                                       (threshold set for entire module)
   * 
   * - Discriminator (Disc) = trigger whenever the discriminator exceeds some threshold
   *                          (only for mDOMs, normal data taking operation)
   */
  enum mDOMPMTTrigMode { UnknownTrigMode = -1 , ADC = 0, Disc = 1 };


  /**
   * mDOM PMT internal trigger mode
   */
  mDOMPMTTrigMode trigMode;

  /**
   * Count threshold for the ADC triggering mode.
   * This value is set for the entire mDOM module
   * (not different for each of the 24 individual PMTs)
  */
  uint16_t adcThreshold;

  /**
   * The mDOM PMT can readout its collected data with either a fixed number of samples in a launch
   * or with a variable number of samples in a launch (configured with preSamples & postSamples)
   */
  enum mDOMReadoutMode { UnknownReadoutMode = -1 , FIXED_LENGTH = 0, VARIABLE_LENGTH = 1 };

  /**
   * The readout mode for launches from this PMT
   */
  mDOMReadoutMode readoutMode;

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
  uint16_t adcBaselineDAC;

  /**
   * DAC setting used for shifting the discriminator voltage 
   */
  uint16_t discDAC;



  // destructor
  ~I3mDOMStatus();

  // constructor
  I3mDOMStatus():
    enabled(true),
    trigMode(UnknownTrigMode),
    adcThreshold(0),
    readoutMode(UnknownReadoutMode),
    preSamples(0),
    postSamples(0),
    pmtHV(NAN),
    adcBaselineDAC(0),
    discDAC(0)
  {};



  // comparison operators
  bool operator==(const I3mDOMStatus& rhs) const
  {
    return (enabled == rhs.enabled &&
            trigMode == rhs.trigMode &&
            adcThreshold == rhs.adcThreshold &&
            readoutMode == rhs.readoutMode &&
            preSamples == rhs.preSamples &&
            postSamples == rhs.postSamples &&
            CompareFloatingPoint::Compare_NanEqual(pmtHV, rhs.pmtHV) &&
            adcBaselineDAC == rhs.adcBaselineDAC &&
            discDAC == rhs.discDAC);
  }
  bool operator!=(const I3mDOMStatus& rhs) const
  {
    return !operator==(rhs);
  }


  // A printing function to display the mDOMPMTTrigMode member assigned to trigMode as a string
  std::string PrintTrigMode() const;

  // A printing function to display the mDOMReadoutMode member assigned to readoutMode as a string
  std::string PrintReadoutMode() const;


  // set up serialization
  template <class Archive> void serialize(Archive& ar, const unsigned version);

};
I3_POINTER_TYPEDEFS(I3mDOMStatus);
I3_CLASS_VERSION(I3mDOMStatus, i3mdomstatus_version_);


// create a map of OMKey to I3mDOMStatus
typedef I3Map<OMKey, I3mDOMStatus> I3mDOMStatusMap;
I3_POINTER_TYPEDEFS(I3mDOMStatusMap);


// allows the printing of objects in these classes
std::ostream& operator<<(std::ostream& oss, const I3mDOMStatus& s);
std::ostream& operator<<(std::ostream& oss, const I3mDOMStatusMap& m);

#endif //I3MDOMSTATUS_H_INCLUDED
