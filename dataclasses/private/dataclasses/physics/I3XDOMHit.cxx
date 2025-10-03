/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#include <dataclasses/external/CompareFloatingPoint.h>
#include <dataclasses/physics/I3XDOMHit.h>


template <class Archive>
void I3XDOMHit::serialize(Archive& ar, unsigned int version) {
  if (version > i3xdomhit_version_) {
    log_fatal("Attempting to read version %u from file"
              " but running version %u of I3XDOMHit class.",
              version, i3xdomhit_version_);
  }
  ar & make_nvp("time", time_);
  ar & make_nvp("lc", lc_);
  ar & make_nvp("charge", charge_);
}

std::ostream& I3XDOMHit::Print(std::ostream& os) const {
  os << "I3XDOMHit(time=" << time_
     << ", lc=" << lc_
     << ", charge=" << charge_ << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3XDOMHit& hit) {
  return hit.Print(os);
}

bool
I3XDOMHit::operator==(const I3XDOMHit& rhs) const {
  return (CompareFloatingPoint::Compare(time_, rhs.time_) &&
          lc_ == rhs.lc_ &&
          CompareFloatingPoint::Compare(charge_, rhs.charge_));
}


I3_SERIALIZABLE(I3XDOMHit);
I3_SERIALIZABLE(I3XDOMHitSeriesMap);
