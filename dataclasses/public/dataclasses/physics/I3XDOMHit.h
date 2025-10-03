/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Definition of I3XDOMHit class
 *
 * @file I3XDOMHit.h
 * @date 2025-06-02
 * @author Schmidt
 * @author Stuttard
 *
 */

#ifndef I3XDOMHIT_H_INCLUDED
#define I3XDOMHIT_H_INCLUDED

#include <ostream>
#include <vector>

#include <dataclasses/I3Map.h>
#include <icetray/I3PointerTypedefs.h>
#include <icetray/OMKey.h>
#include <icetray/serialization.h>


static const unsigned int i3xdomhit_version_ = 0;

/** Direct readout of an extracted hit from an xDOM such as DEgg or mDOM.
 * 
 * The vast majority of PMT hits are single photoelectrons. These hits
 * can be feature extracted in an xDOM, yielding only a time & charge.
 */
class I3XDOMHit
{
 public:
  /** Default constructor.
   * 
   * It sets time and charge to NAN and local coincidence to false.
   */
  I3XDOMHit() : time_(NAN), lc_(false), charge_(NAN) {}

  // implicit destructor, copy constructor, copy assignment,
  // move constructor and move assignment

  /** Print the hit.
   *
   * Inserts formatted hit data into \p os.
   * @param os The output stream.
   * @return The output stream.
   */
  std::ostream& Print(std::ostream& os) const;

  /** Return the hit time.
   * 
   * @return Hit time.
   */
  double GetTime() const { return time_; }
  /** Set the hit time.
   *
   * @param time Hit time.
   * @return This xDOM hit.
   */
  I3XDOMHit& SetTime(double time) {
    time_ = time;
    return *this;
  }

  /** Return local coincidence bit.
   * 
   * @return Local coincidence bit.
   */
  bool GetLCBit() const { return lc_; }
  /** Set the local coincidence bit.
   *
   * @param lc Local coincidence bit.
   * @return This xDOM hit.
   */
  I3XDOMHit& SetLCBit(bool lc) {
    lc_ = lc;
    return *this;
  }

  /** Return the hit charge.
   * 
   * @return Hit charge.
   */
  double GetCharge() const { return charge_; }
  /** Set the hit charge.
   *
   * @param charge Hit charge.
   * @return This xDOM hit.
   */
  I3XDOMHit& SetCharge(double charge) {
    charge_ = charge;
    return *this;
  }

  /** Compare if the hit is equal to another.
   *
   * @param rhs Another hit.
   * @return true if rhs is equal to this hit, false otherwise.
   */
  bool operator==(const I3XDOMHit& rhs) const;

  /** Compare if the hit is not equal to another.
   *
   * @param rhs Another hit.
   * @return true if rhs is not equal to this hit, false otherwise.
   */
  bool operator!=(const I3XDOMHit& rhs) const { return !(*this == rhs); }

 private:
  double time_;
  bool lc_;
  double charge_;


  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive& ar, unsigned int version);
};


/** Insert formatted xDOM hit data into an output stream.
 *
 * @param os The output stream.
 * @param hit A hit.
 * @return The output stream.
 */
std::ostream& operator<<(std::ostream& os, const I3XDOMHit& hit);


I3_POINTER_TYPEDEFS(I3XDOMHit);
I3_CLASS_VERSION(I3XDOMHit, i3xdomhit_version_);

typedef std::vector<I3XDOMHit> I3XDOMHitSeries;
typedef I3Map<OMKey, I3XDOMHitSeries> I3XDOMHitSeriesMap;

I3_POINTER_TYPEDEFS(I3XDOMHitSeries);
I3_POINTER_TYPEDEFS(I3XDOMHitSeriesMap);

#endif  // I3XDOMHIT_H_INCLUDED


