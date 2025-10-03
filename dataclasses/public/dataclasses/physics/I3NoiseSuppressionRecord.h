/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Definition of I3NoiseSuppressionRecord class
 *
 * @file I3NoiseSuppressionRecord.h
 * @date 2025-06-02
 * @author Schmidt
 * @author Stuttard
 *
 */

#ifndef I3NOISESUPPRESSIONRECORD_H_INCLUDED
#define I3NOISESUPPRESSIONRECORD_H_INCLUDED

// TODO: I am not sure, if we want to include the sizes of the exclusion
//       and local coincidence window. I understand, that these also fit
//       into the detector status only, no?
#define I3NOISESUPPRESSIONRECORD_INCLUDE_SIZES

#include <ostream>
#include <vector>

#include <dataclasses/I3Map.h>
#include <icetray/I3PointerTypedefs.h>
#include <icetray/OMKey.h>
#include <icetray/serialization.h>


static const unsigned int i3noisesuppressionrecord_version_ = 0;

/** The mDOM noise suppression record.
 * 
 * All physics hits in mDOM PMTs cannot be fully transmitted to the surface
 * due to excessive time correlated PMT dark noise at low temperatures.
 * Because of the time correlation, these excessive hits can be excluded
 * from readout with little dead time.
 * A correlated noise event is defined as two or more hits in a given
 * PMT channel within an exclusion time window, but without a corresponding
 * local-coincidence hit in a different PMT channel.
 * If a correlated noise event is detected, all subsequent hits within
 * the exclusion time window following the first hit are suppressed.
 * Only the time of the first hit, and number of discriminator rising edges
 * within the period are retained.
 */
class I3NoiseSuppressionRecord
{
 public:
  /** Default constructor.
   * 
   * It sets time & time window sizes to NAN and the number of noise hits to 0.
   */
  I3NoiseSuppressionRecord()
   : exclWindowStart_(NAN),
#ifdef I3NOISESUPPRESSIONRECORD_INCLUDE_SIZES
     exclWindowSize_(NAN), lcWindowSize_(NAN),
#endif  // I3NOISESUPPRESSIONRECORD_INCLUDE_SIZES
     n_(0)
  {}

  // implicit destructor, copy constructor, copy assignment,
  // move constructor and move assignment

  /** Print the hit.
   *
   * Inserts formatted hit data into \p os.
   * @param os The output stream.
   * @return The output stream.
   */
  std::ostream& Print(std::ostream& os) const;

  /** Return the start time of the exclusion window.
   * 
   * @return Start time.
   */
  double GetTime() const { return exclWindowStart_; }
  /** Set the start time of the exclusion window.
   *
   * @param tm Start time.
   * @return This record.
   */
  I3NoiseSuppressionRecord& SetTime(double tm) {
    exclWindowStart_ = tm;
    return *this;
  }

#ifdef I3NOISESUPPRESSIONRECORD_INCLUDE_SIZES
  /** Return the size of the exclusion window.
   * 
   * @return Size.
   */
  double GetExclusionWindowSize() const { return exclWindowSize_; }
  /** Set the size of the exclusion window.
   *
   * @param tm Size.
   * @return This record.
   */
  I3NoiseSuppressionRecord& SetExclusionWindowSize(double tm) {
    exclWindowSize_ = tm;
    return *this;
  }

  /** Return the size of the local coincidence window.
   * 
   * @return Size.
   */
  double GetLCWindowSize() const { return lcWindowSize_; }
  /** Set the size of the local coincidence window.
   *
   * @param tm Size.
   * @return This record.
   */
  I3NoiseSuppressionRecord& SetLCWindowSize(double tm) {
    lcWindowSize_ = tm;
    return *this;
  }
#endif  // I3NOISESUPPRESSIONRECORD_INCLUDE_SIZES

  /** Return the 'number of noise hits' within the exclusion window.
   * 
   * @return 'Number of noise hits'.
   */
  unsigned int GetNNoiseHits() const { return n_; }
  /** Set the 'number of noise hits' within the exclusion window.
   *
   * @param n 'Number of noise hits'.
   * @return This record.
   */
  I3NoiseSuppressionRecord& SetNNoiseHits(unsigned int n) {
    n_ = n;
    return *this;
  }

  /** Compare if the hit is equal to another.
   *
   * @param rhs Another hit.
   * @return true if rhs is equal to this hit, false otherwise.
   */
  bool operator==(const I3NoiseSuppressionRecord& rhs) const;

  /** Compare if the hit is not equal to another.
   *
   * @param rhs Another hit.
   * @return true if rhs is not equal to this hit, false otherwise.
   */
  bool operator!=(const I3NoiseSuppressionRecord& rhs) const {
    return !(*this == rhs);
  }

 private:
  double exclWindowStart_;
#ifdef I3NOISESUPPRESSIONRECORD_INCLUDE_SIZES
  double exclWindowSize_;
  double lcWindowSize_;
#endif  // I3NOISESUPPRESSIONRECORD_INCLUDE_SIZES
  unsigned int n_;


  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive& ar, unsigned int version);
};


/** Insert formatted noise suppression record into an output stream.
 *
 * @param os The output stream.
 * @param hit A hit.
 * @return The output stream.
 */
std::ostream& operator<<(std::ostream& os,
                         const I3NoiseSuppressionRecord& noise);


I3_POINTER_TYPEDEFS(I3NoiseSuppressionRecord);
I3_CLASS_VERSION(I3NoiseSuppressionRecord, i3noisesuppressionrecord_version_);

typedef std::vector<I3NoiseSuppressionRecord> I3NoiseSuppressionRecordSeries;
typedef I3Map<OMKey, I3NoiseSuppressionRecordSeries>
  I3NoiseSuppressionRecordSeriesMap;

I3_POINTER_TYPEDEFS(I3NoiseSuppressionRecordSeries);
I3_POINTER_TYPEDEFS(I3NoiseSuppressionRecordSeriesMap);

#endif  // I3NOISESUPPRESSIONRECORD_H_INCLUDED


