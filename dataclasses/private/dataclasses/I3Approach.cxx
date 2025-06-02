// SPDX-FileCopyrightText: 2025 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dataclasses/I3Approach.h>
#include <icetray/I3Logging.h>

std::ostream& operator<<(std::ostream &os, const I3Approach &approach) {
  os << "I3Approach(" << approach.GetDistance() << "," << approach.GetPosition() << ")";
  return os;
}

template <class Archive>
void
I3Approach::serialize(Archive& ar, unsigned version) {
  if (version > i3approach_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3Approach class.",
              version, i3approach_version_);

  ar & make_nvp("Distance", distance_);
  ar & make_nvp("Position", position_);
}

I3_SERIALIZABLE(I3Approach);
