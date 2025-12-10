/**
 * Copyright  (C) 2025 the icecube collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef I3LOMLAUNCH_H_INCLUDED
#define I3LOMLAUNCH_H_INCLUDED

#include <ostream>
#include <vector>

#include <dataclasses/I3Map.h>
#include <icetray/I3PointerTypedefs.h>
#include <icetray/OMKey.h>
#include <icetray/serialization.h>


static const unsigned int i3lomlaunch_version_ = 0;

/** Parsed/decoded payload data structure for LOMs
 */
class I3LOMLaunch
{
 public:
  /** Default constructor.
   */
  I3LOMLaunch() : time_(NAN, false), adcDataHighGain_(), adcDataLowGain_() {}

  // implicit destructor, copy constructor, copy assignment,
  // move constructor and move assignment

  /** Print the launch.
   *
   * Inserts formatted launch data into \p os.
   * @param os The output stream.
   * @return The output stream.
   */
  std::ostream& Print(std::ostream& os) const;


  /** Return the launch time.
   *
   * @return Launch time.
   */
  double GetTime() const { return time_.first; }
  /** Return whether the launch time is valid.
   *
   * @return True, if the launch time is valid.
   */
  bool HasValidTime() const { return time_.second; }
  /** Return the launch time, if valid.
   *
   * @return Launch time.
   * @throws runtime_error if the launch time is invalid.
   */
  double GetValidTime() const {
    if (!time_.second) {
      throw std::runtime_error("I3LOMLaunch: invalid launch time");
    }
    return time_.first;
  }
  /** Set the launch time.
   *
   * @param time Launch time.
   * @param valid Whether or not the launch time is valid
   * (optional, default is true).
   * @return This LOM launch.
   */
  I3LOMLaunch& SetTime(double time, bool valid=true) {
    if (!std::isfinite(time) && valid) {
      throw std::runtime_error("infinite or NAN is not a valid time");
    }
    time_ = std::make_pair(time, valid);
    return *this;
  }



  /** Return the waveforms. Versions for both low and high gain channels.
   *
   * @return wavform data.
   */
  const std::vector<int>& GetADCDataHighGain() const { return adcDataHighGain_; }
  const std::vector<int>& GetADCDataLowGain() const { return adcDataLowGain_; }


  /** Return the waveform length. is a single length for both high and low
   * gain channels, which record the same length.
   *
   * @return waveform length.
   */
  unsigned int GetADCDataLength() const;


  /** Check if the waveform vectors have been filled
   *
   * @return bool (true means "waveforms are filled")
   */
  bool IsADCDataFilled() const;


  /** Set the waveforms (both low and high gain).
   *
   * @return This LOM launch.
   */
  I3LOMLaunch& SetADCData(std::vector<int>& adcHG, std::vector<int>& adcLG);

  /** Set the high gain waveform.
   *  This performs no checks, recommended to use SetADCData to simultaneously set both low and high gain.
   *
   * @return This LOM launch.
   */
  I3LOMLaunch& SetADCDataHighGain(std::vector<int>& adcHG) { adcDataHighGain_ = adcHG; return *this; }

  /** Set the low gain waveform.
   *  This performs no checks, recommended to use SetADCData to simultaneously set both low and high gain.
   *
   * @return This LOM launch.
   */
  I3LOMLaunch& SetADCDataLowGain(std::vector<int>& adcLG) { adcDataLowGain_ = adcLG; return *this; }


  /** Compare if the launch is equal to another.
   *
   * @param rhs Another launch.
   * @return true if rhs is equal to this launch, false otherwise.
   */
  bool operator==(const I3LOMLaunch& rhs) const;

  /** Compare if the launch is not equal to another.
   *
   * @param rhs Another launch.
   * @return true if rhs is not equal to this launch, false otherwise.
   */
  bool operator!=(const I3LOMLaunch& rhs) const {
    return !(*this == rhs);
  }


 private:

  bool CheckADCData(const std::vector<int>& adcHG, const std::vector<int>& adcLG) const;

  std::pair<double, bool> time_; // Launch time
  std::vector<int> adcDataHighGain_;
  std::vector<int> adcDataLowGain_;

  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive& ar, unsigned int version);
};


/** Insert formatted launch data into an output stream.
 *
 * @param os The output stream.
 * @param launch A launch.
 * @return The output stream.
 */
std::ostream& operator<<(std::ostream& os, const I3LOMLaunch& launch);


I3_POINTER_TYPEDEFS(I3LOMLaunch);
I3_CLASS_VERSION(I3LOMLaunch, i3lomlaunch_version_);

typedef std::vector<I3LOMLaunch> I3LOMLaunchSeries;
typedef I3Map<OMKey, I3LOMLaunchSeries> I3LOMLaunchSeriesMap;

I3_POINTER_TYPEDEFS(I3LOMLaunchSeries);
I3_POINTER_TYPEDEFS(I3LOMLaunchSeriesMap);

#endif  // I3LOMLAUNCH_H_INCLUDED
