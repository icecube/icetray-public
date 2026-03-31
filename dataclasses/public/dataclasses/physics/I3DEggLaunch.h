/**
 * Copyright  (C) 2025 the icecube collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef I3DEGGLAUNCH_H_INCLUDED
#define I3DEGGLAUNCH_H_INCLUDED

#include <ostream>
#include <vector>

#include <dataclasses/I3Map.h>
#include <dataclasses/physics/detail/UpgradeLCFlags.h>
#include <dataclasses/physics/detail/I3XDOMLaunch.h>
#include <icetray/I3PointerTypedefs.h>
#include <icetray/OMKey.h>
#include <icetray/serialization.h>
#include <dataclasses/calibration/I3DEggCal.h>


static const unsigned int i3degglaunch_version_ = 0;

/** Direct readout of ADC data/a waveform from a DEgg (which share the same frontend electronics).
 * 
 * Waveforms that cannot be feature extracted (cp. class I3XDOMHit) are stored
 * in an instance of I3DEggLaunch.
 */
class I3DEggLaunch
{
 public:

  /** Default constructor.
   */
  I3DEggLaunch() : launch_() {}

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
   * @return This DEgg launch.
   */
  I3DEggLaunch& SetTime(double time, bool valid=true) {
    launch_.SetTime(time, valid);
    return *this;
  }

  /** Return the local coincidence bit.
   * 
   * @return Local coincidence bit.
   */
  UpgradeLCFlags GetLCFlags() const {
    return launch_.GetLCFlags();
  }
  /** Set the local coincidence bits.
   *
   * @param lc Local coincidence bits.
   * @return This xDOM launch.
   */
  I3DEggLaunch& SetLCFlags(UpgradeLCFlags lc) {
    launch_.SetLCFlags(lc);
    return *this;
  }
  /** Append local coincidence bits to the current bits
   *
   * @param lc Local coincidence bits.
   * @return This xDOM launch.
   */
  I3DEggLaunch& AddLCFlags(UpgradeLCFlags lc) {
    launch_.AddLCFlags(lc);
    return *this;
  }

  /** Return the ADC data/waveform.
   * 
   * @return ADC data.
   */
  const std::vector<int>& GetADCData() const { return launch_.GetADCData(); }
  /** Return the time for each ADC sample
   * 
   * @return time vector (one element per sample).
   */
  std::vector<double> GetADCSampleTimes() const { return launch_.GetADCSampleTimes(1./I3DEggCal::sampleRate); }
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
   * @return This DEgg launch.
   */
  I3DEggLaunch& SetDeltaCompressedADCData(const std::vector<char>& adcData,
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
   * @return This DEgg launch.
   */
  I3DEggLaunch& SetDeltaCompressedADCData(std::vector<char>&& adcData,
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
   * @return This DEgg launch.
   */
  I3DEggLaunch& SetADCData(const std::vector<int>& adcData,
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
   * @return This DEgg launch.
   */
  I3DEggLaunch& SetADCData(std::vector<int>&& adcData,
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
   * @return This DEgg launch.
   */
  I3DEggLaunch& SetNPreSamples(unsigned int nPreSamples) {
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
   * @return This DEgg launch.
   */
  I3DEggLaunch& SetBaselineValue(double baselineValue, bool valid=true) {
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
   * @return This DEgg launch.
   */
  I3DEggLaunch& SetFEXResult(I3XDOMLaunch::FEXResult result) {
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
   * @return This DEgg launch.
   */
  I3DEggLaunch& SetSelectionType(I3XDOMLaunch::SelectionType type) {
    launch_.SetSelectionType(type);
    return *this;
  }

  /** Compare if the launch is equal to another.
   *
   * @param rhs Another launch.
   * @return true if rhs is equal to this launch, false otherwise.
   */
  bool operator==(const I3DEggLaunch& rhs) const { return launch_ == rhs.launch_; }

  /** Compare if the launch is not equal to another.
   *
   * @param rhs Another launch.
   * @return true if rhs is not equal to this launch, false otherwise.
   */
  bool operator!=(const I3DEggLaunch& rhs) const { return launch_ != rhs.launch_; }

 private:
  I3XDOMLaunch launch_;


  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive& ar, unsigned int version);
};


/** Insert formatted DEgg launch data into an output stream.
 *
 * @param os The output stream.
 * @param launch A launch.
 * @return The output stream.
 */
std::ostream& operator<<(std::ostream& os, const I3DEggLaunch& launch);


I3_POINTER_TYPEDEFS(I3DEggLaunch);
I3_CLASS_VERSION(I3DEggLaunch, i3degglaunch_version_);

typedef std::vector<I3DEggLaunch> I3DEggLaunchSeries;
typedef I3Map<OMKey, I3DEggLaunchSeries> I3DEggLaunchSeriesMap;

I3_POINTER_TYPEDEFS(I3DEggLaunchSeries);
I3_POINTER_TYPEDEFS(I3DEggLaunchSeriesMap);


// Make typedefs with PDOM instead of DEgg in the name for convenience
typedef I3DEggLaunch I3PDOMLaunch;
typedef I3DEggLaunchSeries I3PDOMLaunchSeries;
typedef I3DEggLaunchSeriesMap I3PDOMLaunchSeriesMap;
I3_POINTER_TYPEDEFS(I3PDOMLaunch);
I3_POINTER_TYPEDEFS(I3PDOMLaunchSeries);
I3_POINTER_TYPEDEFS(I3PDOMLaunchSeriesMap);

#endif  // I3DEGGLAUNCH_H_INCLUDED
