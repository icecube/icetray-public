/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Definition of I3XDOMLaunch class
 *
 * @file I3XDOMLaunch.h
 * @date 2025-06-02
 * @author Schmidt
 * @author Stuttard
 *
 */

#ifndef I3XDOMLaunch_H_INCLUDED
#define I3XDOMLaunch_H_INCLUDED

#include <cmath>
#include <ostream>
#include <utility>
#include <vector>

#include <icetray/I3Logging.h>
#include <icetray/serialization.h>


static const unsigned int i3xdomlaunch_version_ = 0;

/** Direct readout of ADC data/a waveform from an xDOM such as DEgg or mDOM.
 * 
 * Waveforms that cannot be feature extracted (cp. class I3XDOMHit) are stored
 * in an instance of I3DEggLaunch or I3mDOMLaunch that are composed of
 * I3XDOMLaunch eventually.
 */
class I3XDOMLaunch
{
 public:
  enum class FEXResult {
    UNDEFINED=0,
    SUCCESS=1,
    WAVEFORM_UNEXTRACTABLE=2,
    INVALID_INPUT_PARAMETERS=3,
    INVALID_CONFIG=4,
    INTERNAL_ERROR=5,
    DOMAIN_ERROR=6,
    CHARGE_ERROR=7,
    MAX_GRADIENT_ERROR=8
  };

  enum class SelectionType {
    UNDEFINED=0,
    MIN_BIAS=1
  };

  static const unsigned int START_BITS_PER_WORD = 3;  // this is input to the
  static const unsigned int MAX_BITS_PER_WORD = 17;   // delta compression


  /** Default constructor.
   * 
   * It sets time and baseline to NAN plus invalid, local coincidence to false
   * and the number of pre-samples to zero. Delta-compressed ADC data is empty.
   */
  I3XDOMLaunch()
   : time_(NAN, false),
     lc_(false),
     nSamples_(0),
     startBitsPerWord_(START_BITS_PER_WORD),
     maxBitsPerWord_(MAX_BITS_PER_WORD),
     nPreSamples_(0),
     baselineValue_(NAN, false),
     fexResult_(FEXResult::UNDEFINED),
     selectionType_(SelectionType::UNDEFINED) {}

  // implicit destructor, copy constructor, copy assignment,
  // move constructor and move assignment

  /** Print the launch.
   *
   * Inserts formatted launch data into \p os.
   * @param os The output stream.
   * @return The output stream.
   */
  std::ostream& Print(std::ostream& os) const;

  /** Return the ADC launch time.
   * 
   * @return Launch time.
   */
  double GetTime() const { return time_.first; }
  /** Return whether the ADC launch time is valid.
   * 
   * @return True, if the launch time is valid.
   */
  bool HasValidTime() const { return time_.second; }
  /** Return the ADC launch time, if valid.
   * 
   * @return Launch time.
   * @throws runtime_error if the launch time is invalid.
   */
  double GetValidTime() const {
    if (!time_.second) {
      throw std::runtime_error("I3XDOMLaunch: invalid ADC launch time");
    }
    return time_.first;
  }
  /** Set the ADC launch time.
   *
   * @param time Launch time.
   * @param valid Whether or not the launch time is valid
   * (optional, default is true).
   * @return This xDOM launch.
   */
  I3XDOMLaunch& SetTime(double time, bool valid=true) {
    if (!std::isfinite(time) && valid) {
      throw std::runtime_error("infinite or NAN is not a valid time");
    }
    time_ = std::make_pair(time, valid);
    return *this;
  }

  /** Return the local coincidence bit.
   * 
   * @return Local coincidence bit.
   */
  bool GetLCBit() const { return lc_; }
  /** Set the local coincidence bit.
   *
   * @param lc Local coincidence bit.
   * @return This xDOM launch.
   */
  I3XDOMLaunch& SetLCBit(bool lc) {
    lc_ = lc;
    return *this;
  }

  /** Return the ADC data/waveform.
   * 
   * @return ADC data.
   */
  const std::vector<int>& GetADCData() const;
  /** Set the delta-compressed ADC data/waveform.
   *
   * The typical caller of this method is the 'payload parsing' that
   * decodes pDAQ data into dataclasses. pDAQ is supposed to provide
   * the ADC data/waveform (delta-)compressed.
   * @param adcData Delta-compressed ADC data.
   * @param nSamples The number of samples after uncompressioning \p adcData.
   * @param lazy Uncompress the ADC data within this call if \p lazy is false.
   * If \p lazy is true, uncompress the ADC data later, when it is actually
   * needed (Optional, default is true).
   * @return This xDOM launch.
   */
  I3XDOMLaunch& SetDeltaCompressedADCData(const std::vector<char>& adcData,
                                          unsigned int nSamples,
                                          bool lazy=true) {
    adcData_ = adcData;
    return SetDeltaCompressedADCData(nSamples, lazy);
  }
  /** Set the delta-compressed ADC data/waveform.
   *
   * The typical caller of this method is the 'payload parsing' that
   * decodes pDAQ data into dataclasses. pDAQ is supposed to provide
   * the ADC data/waveform (delta-)compressed.
   * @param adcData Delta-compressed ADC data.
   * @param nSamples The number of samples after uncompressioning \p adcData.
   * @param lazy Uncompress the ADC data within this call if \p lazy is false.
   * If \p lazy is true, uncompress the ADC data later, when it is actually
   * needed (Optional, default is true).
   * @return This xDOM launch.
   */
  I3XDOMLaunch& SetDeltaCompressedADCData(std::vector<char>&& adcData,
                                          unsigned int nSamples,
                                          bool lazy=true) {
    adcData_ = adcData;
    return SetDeltaCompressedADCData(nSamples, lazy);
  }
  /** Set the ADC data/waveform.
   *
   * This is an alternative method to SetDeltaCompressedADCData that could be
   * used by for instance simulation. Input data is 'raw' that is uncompressed.
   * @param adcData ADC data.
   * @param lazy Compress the ADC data within this call if \p lazy is false.
   * If \p lazy is true, compress the ADC data later, when it is actually
   * needed (Optional, default is true).
   * @return This xDOM launch.
   */
  I3XDOMLaunch& SetADCData(const std::vector<int>& adcData,
                           bool lazy=true) {
    adcCache_ = adcData;
    return SetADCData(lazy);
  }
  /** Set the ADC data/waveform.
   *
   * This is an alternative method to SetDeltaCompressedADCData that could be
   * used by for instance simulation. Input data is 'raw' that is uncompressed.
   * @param adcData ADC data.
   * @param lazy Compress the ADC data within this call if \p lazy is false.
   * If \p lazy is true, compress the ADC data later, when it is actually
   * needed (Optional, default is true).
   * @return This xDOM launch.
   */
  I3XDOMLaunch& SetADCData(std::vector<int>&& adcData,
                           bool lazy=true) {
    adcCache_ = adcData;
    return SetADCData(lazy);
  }

  /** Return the number of samples recorded before trigger.
   * 
   * @return The number of pre-samples.
   */
  unsigned int GetNPreSamples() const { return nPreSamples_; }
  /** Set the number of samples recorded before trigger.
   *
   * @param nPreSamples The number of pre-samples.
   * @return This xDOM launch.
   */
  I3XDOMLaunch& SetNPreSamples(unsigned int nPreSamples) {
    nPreSamples_ = nPreSamples;
    return *this;
  }

  /** Return the baseline value/height.
   * 
   * @return Baseline value.
   */
  double GetBaselineValue() const { return baselineValue_.first; }
  /** Return whether the baseline value/height is valid.
   * 
   * @return True, if the baseline value is valid.
   */
  bool HasValidBaselineValue() const { return baselineValue_.second; }
  /** Return the baseline value/height, if valid.
   * 
   * @return Baseline value.
   * @throws runtime_error if the baseline value is invalid.
   */
  double GetValidBaselineValue() const {
    if (!baselineValue_.second) {
      throw std::runtime_error("I3XDOMLaunch: invalid baseline value");
    }
    return baselineValue_.first;
  }
  /** Set the baseline value/height.
   *
   * @param baselineValue Baseline value.
   * @param valid Whether or not the baseline value is valid
   * (optional, default is true).
   * @return This xDOM launch.
   */
  I3XDOMLaunch& SetBaselineValue(double baselineValue, bool valid=true) {
    if (!std::isfinite(baselineValue) && valid) {
      throw std::runtime_error("infinite or NAN is not a valid baseline value");
    }
    baselineValue_ = std::make_pair(baselineValue, valid);
    return *this;
  }

  /** Get the FEX result.
   * 
   * @return The result.
   */
  FEXResult GetFEXResult() const {
    return fexResult_;
  }
  /** Set the FEX result.
   * 
   * @param result The result.
   * @return This xDOM launch.
   */
  I3XDOMLaunch& SetFEXResult(FEXResult result) {
    fexResult_ = result;
    return *this;
  }

  /** Get the selection type.
   * 
   * @return The type.
   */
  SelectionType GetSelectionType() const {
    return selectionType_;
  }
  /** Set the selection type.
   * 
   * @param result The type.
   * @return This xDOM launch.
   */
  I3XDOMLaunch& SetSelectionType(SelectionType type) {
    selectionType_ = type;
    return *this;
  }

  /** Compare if the launch is equal to another.
   *
   * @param rhs Another launch.
   * @return true if rhs is equal to this launch, false otherwise.
   */
  bool operator==(const I3XDOMLaunch& rhs) const;

  /** Compare if the launch is not equal to another.
   *
   * @param rhs Another launch.
   * @return true if rhs is not equal to this launch, false otherwise.
   */
  bool operator!=(const I3XDOMLaunch& rhs) const {
    return !(*this == rhs);
  }

 private:
  std::pair<double, bool> time_;
  bool lc_;
  mutable std::vector<char> adcData_;
  mutable unsigned int nSamples_;
  mutable std::vector<int> adcCache_;
  unsigned int startBitsPerWord_;  // eventually have a getter/setter
  unsigned int maxBitsPerWord_;    // for these in future (-> serialize!)
  unsigned int nPreSamples_;
  std::pair<double, bool> baselineValue_;
  FEXResult fexResult_;
  SelectionType selectionType_;


  I3XDOMLaunch& SetDeltaCompressedADCData(unsigned int nSamples, bool lazy) {
    nSamples_ = nSamples;
    if (lazy) {
      adcCache_.clear();
    } else {
      Uncompress();
    }
    return *this;
  }

  I3XDOMLaunch& SetADCData(bool lazy) {
    if (lazy) {
      adcData_.clear();
      nSamples_ = 0;
    } else {
      Compress();
    }
    return *this;
  }

  void Compress() const;
  void Uncompress() const;


  friend class icecube::serialization::access;
  template <class Archive> void save(Archive & ar, unsigned int version) const;
  template <class Archive> void load(Archive & ar, unsigned int version);
  I3_SERIALIZATION_SPLIT_MEMBER();
};


/** Insert formatted xDOM launch data into an output stream.
 *
 * @param os The output stream.
 * @param launch A launch.
 * @return The output stream.
 */
std::ostream& operator<<(std::ostream& os, const I3XDOMLaunch& launch);


I3_CLASS_VERSION(I3XDOMLaunch, i3xdomlaunch_version_);

#endif  // I3XDOMLaunch_H_INCLUDED


