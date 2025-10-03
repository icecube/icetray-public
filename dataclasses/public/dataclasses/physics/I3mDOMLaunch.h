/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Definition of I3mDOMLaunch class
 *
 * @file I3mDOMLaunch.h
 * @date 2025-06-02
 * @author Schmidt
 * @author Stuttard
 *
 */

#ifndef I3MDOMLAUNCH_H_INCLUDED
#define I3MDOMLAUNCH_H_INCLUDED

#include <ostream>
#include <vector>

#include <dataclasses/I3Map.h>
#include <dataclasses/physics/detail/I3XDOMLaunch.h>
#include <icetray/I3PointerTypedefs.h>
#include <icetray/I3Units.h>
#include <icetray/OMKey.h>
#include <icetray/serialization.h>


static const unsigned int i3mdomlaunch_version_ = 0;

/** Direct readout of ADC data/a waveform from a mDOM.
 *
 * Waveforms that cannot be feature extracted (cp. class I3XDOMHit) are stored
 * in an instance of I3mDOMLaunch.
 */
class I3mDOMLaunch
{
 public:
  /** A basic TOT hit like it could originate from a TDC.
   * 
   * The mDOM data contains the 'Compressed Discriminator Logic Waveform'
   * that is decoded into a series of TOTHits. So, just a start time and a
   * 'time over threshold'.
   *
   * @todo
   * TODO: the TOTHits' times should be somehow aligned to the launch time...
   *       currently start times start at 0, but probably should start at some
   *       negative number (cf. pre-samples of the mDOM launch).
   * @note
   * NOTE: the upgrade readout dataclasses are even though they are located in
   *       dataclasses, also strongly linked to payload-parsing. they will
   *       likely reach a final 'configuration' not before new payload-parsing
   *       is finished, tested, verified and released.
   */
  class TOTHit
  {
   public:
    static const double SAMPLING_RATE;


    TOTHit() : time_(NAN), tot_(NAN) {}
    TOTHit(double time, double tot) : time_(time), tot_(tot) {}
    TOTHit(unsigned int time, unsigned int tot,
           double samplingRate=SAMPLING_RATE)
     : time_(time / samplingRate), tot_(tot / samplingRate) {}

    // implicit destructor, copy constructor, copy assignment,
    // move constructor and move assignment

    /** Print the hit.
     *
     * Inserts formatted hit data into \p os.
     * @param os The output stream.
     * @return The output stream.
     */
    std::ostream& Print(std::ostream& os) const;

    double GetTime() const { return time_; }
    TOTHit& SetTime(double time) {
      time_ = time;
      return *this;
    }
    double GetTOT() const { return tot_; }
    TOTHit& SetTOT(double tot) {
      tot_ = tot;
      return *this;
    }

    /** Compare if the hit is equal to another.
     *
     * @param rhs Another hit.
     * @return true if rhs is equal to this hit, false otherwise.
     */
    bool operator==(const TOTHit& rhs) const;

    /** Compare if the hit is not equal to another.
     *
     * @param rhs Another hit.
     * @return true if rhs is not equal to this hit, false otherwise.
     */
    bool operator!=(const TOTHit& rhs) const {
      return !(*this == rhs);
    }

   private:
    double time_;
    double tot_;


    friend class icecube::serialization::access;
    template <class Archive> void serialize(Archive& ar, unsigned int version);
  };


  enum class TriggerType
  {
    UNDEFINED=0,
    SOFTWARE=1,
    ADC_THRESHOLD=2,
    DISCRIMINATOR=3,
    EXTERNAL=4
  };


  /** Default constructor.
   */
  I3mDOMLaunch() : launch_(), triggerType_(TriggerType::UNDEFINED), totHits_() {}

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
  double GetTime() const { return launch_.GetTime(); }
  /** Return whether the ADC launch time is valid.
   * 
   * @return True, if the launch time is valid.
   */
  bool HasValidTime() const { return launch_.HasValidTime(); }
  /** Return the ADC launch time, if valid.
   * 
   * @return Launch time.
   * @throws runtime_error if the launch time is invalid.
   */
  double GetValidTime() const { return launch_.GetValidTime(); }
  /** Set the ADC launch time.
   *
   * @param time Launch time.
   * @param valid Whether or not the launch time is valid
   * (optional, default is true).
   * @return This mDOM launch.
   */
  I3mDOMLaunch& SetTime(double time, bool valid=true) {
    launch_.SetTime(time, valid);
    return *this;
  }

  /** Return the local coincidence bit.
   * 
   * @return Local coincidence bit.
   */
  bool GetLCBit() const { return launch_.GetLCBit(); }
  /** Set the local coincidence bit.
   *
   * @param lc Local coincidence bit.
   * @return This mDOM launch.
   */
  I3mDOMLaunch& SetLCBit(bool lc) {
    launch_.SetLCBit(lc);
    return *this;
  }

  /** Return the trigger type.
   * 
   * @return Trigger type.
   */
  TriggerType GetTriggerType() const { return triggerType_; }
  /** Set the trigger type.
   *
   * @param triggerType Trigger type.
   * @return This mDOM launch.
   */
  I3mDOMLaunch& SetTriggerType(TriggerType triggerType) {
    triggerType_ = triggerType;
    return *this;
  }

  /** Return the ADC data/waveform.
   * 
   * @return ADC data.
   */
  const std::vector<int>& GetADCData() const { return launch_.GetADCData(); }
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
   * @return This mDOM launch.
   */
  I3mDOMLaunch& SetDeltaCompressedADCData(const std::vector<char>& adcData,
                                          unsigned int nSamples,
                                          bool lazy=true) {
    launch_.SetDeltaCompressedADCData(adcData, nSamples, lazy);
    return *this;
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
   * @return This mDOM launch.
   */
  I3mDOMLaunch& SetDeltaCompressedADCData(std::vector<char>&& adcData,
                                          unsigned int nSamples,
                                          bool lazy=true) {
    launch_.SetDeltaCompressedADCData(adcData, nSamples, lazy);
    return *this;
  }
  /** Set the ADC data/waveform.
   *
   * This is an alternative method to SetDeltaCompressedADCData that could be
   * used by for instance simulation. Input data is 'raw' that is uncompressed.
   * @param adcData ADC data.
   * @param lazy Compress the ADC data within this call if \p lazy is false.
   * If \p lazy is true, compress the ADC data later, when it is actually
   * needed (Optional, default is true).
   * @return This mDOM launch.
   */
  I3mDOMLaunch& SetADCData(const std::vector<int>& adcData,
                           bool lazy=true) {
    launch_.SetADCData(adcData, lazy);
    return *this;
  }
  /** Set the ADC data/waveform.
   *
   * This is an alternative method to SetDeltaCompressedADCData that could be
   * used by for instance simulation. Input data is 'raw' that is uncompressed.
   * @param adcData ADC data.
   * @param lazy Compress the ADC data within this call if \p lazy is false.
   * If \p lazy is true, compress the ADC data later, when it is actually
   * needed (Optional, default is true).
   * @return This mDOM launch.
   */
  I3mDOMLaunch& SetADCData(std::vector<int>&& adcData,
                           bool lazy=true) {
    launch_.SetADCData(adcData, lazy);
    return *this;
  }

  /** Return the number of samples recorded before trigger.
   * 
   * @return The number of pre-samples.
   */
  unsigned int GetNPreSamples() const { return launch_.GetNPreSamples(); }
  /** Set the number of samples recorded before trigger.
   *
   * @param nPreSamples The number of pre-samples.
   * @return This mDOM launch.
   */
  I3mDOMLaunch& SetNPreSamples(unsigned int nPreSamples) {
    launch_.SetNPreSamples(nPreSamples);
    return *this;
  }

  /** Return the baseline value/height.
   * 
   * @return Baseline value.
   */
  double GetBaselineValue() const { return launch_.GetBaselineValue(); }
  /** Return whether the baseline value/height is valid.
   * 
   * @return True, if the baseline value is valid.
   */
  bool HasValidBaselineValue() const { return launch_.HasValidBaselineValue(); }
  /** Return the baseline value/height, if valid.
   * 
   * @return Baseline value.
   * @throws runtime_error if the baseline value is invalid.
   */
  double GetValidBaselineValue() const { return launch_.GetValidBaselineValue(); }
  /** Set the baseline value/height.
   *
   * @param baselineValue Baseline value.
   * @param valid Whether or not the baseline value is valid
   * (optional, default is true).
   * @return This mDOM launch.
   */
  I3mDOMLaunch& SetBaselineValue(double baselineValue, bool valid=true) {
    launch_.SetBaselineValue(baselineValue, valid);
    return *this;
  }

  /** Get the FEX result.
   * 
   * @return The result.
   */
  I3XDOMLaunch::FEXResult GetFEXResult() const {
    return launch_.GetFEXResult();
  }
  /** Set the FEX result.
   * 
   * @param result The result.
   * @return This mDOM launch.
   */
  I3mDOMLaunch& SetFEXResult(I3XDOMLaunch::FEXResult result) {
    launch_.SetFEXResult(result);
    return *this;
  }

  /** Get the selection type.
   * 
   * @return The type.
   */
  I3XDOMLaunch::SelectionType GetSelectionType() const {
    return launch_.GetSelectionType();
  }
  /** Set the selection type.
   * 
   * @param result The type.
   * @return This mDOM launch.
   */
  I3mDOMLaunch& SetSelectionType(I3XDOMLaunch::SelectionType type) {
    launch_.SetSelectionType(type);
    return *this;
  }

  /** Return the TOT hits associated with discriminator waveform of this launch.
   *
   * @return The TOT hits.
   */
  std::vector<TOTHit>& GetTOTHits() { return totHits_; }
  /** Return the TOT hits associated with discriminator waveform of this launch.
   *
   * @return The TOT hits.
   */
  const std::vector<TOTHit>& GetTOTHits() const { return totHits_; }

  /** Compare if the launch is equal to another.
   *
   * @param rhs Another launch.
   * @return true if rhs is equal to this launch, false otherwise.
   */
  bool operator==(const I3mDOMLaunch& rhs) const;

  /** Compare if the launch is not equal to another.
   *
   * @param rhs Another launch.
   * @return true if rhs is not equal to this launch, false otherwise.
   */
  bool operator!=(const I3mDOMLaunch& rhs) const {
    return !(*this == rhs);
  }

 private:
  I3XDOMLaunch launch_;
  TriggerType triggerType_;
  std::vector<TOTHit> totHits_;


  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive& ar, unsigned int version);
};


/** Insert formatted mDOM launch data into an output stream.
 *
 * @param os The output stream.
 * @param launch A launch.
 * @return The output stream.
 */
std::ostream& operator<<(std::ostream& os, const I3mDOMLaunch& launch);


/** Insert formatted TOT hit data into an output stream.
 *
 * @param os The output stream.
 * @param hit A TOT hit.
 * @return The output stream.
 */
std::ostream& operator<<(std::ostream& os, const I3mDOMLaunch::TOTHit& hit);


/** Insert a trigger type by name into an output stream.
 *
 * @param os The output stream.
 * @param triggerType Trigger type.
 * @return The output stream.
 */
std::ostream& operator<<(std::ostream& os, const I3mDOMLaunch::TriggerType& triggerType);


I3_POINTER_TYPEDEFS(I3mDOMLaunch);
I3_CLASS_VERSION(I3mDOMLaunch, i3mdomlaunch_version_);

typedef std::vector<I3mDOMLaunch> I3mDOMLaunchSeries;
typedef I3Map<OMKey, I3mDOMLaunchSeries> I3mDOMLaunchSeriesMap;

I3_POINTER_TYPEDEFS(I3mDOMLaunchSeries);
I3_POINTER_TYPEDEFS(I3mDOMLaunchSeriesMap);

#endif  // I3MDOMLAUNCH_H_INCLUDED


