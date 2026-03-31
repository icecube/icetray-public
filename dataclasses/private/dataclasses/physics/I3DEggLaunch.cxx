/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#include <dataclasses/external/CompareFloatingPoint.h>
#include <dataclasses/physics/I3DEggLaunch.h>
#include <icetray/I3Units.h>

template <class Archive>
void I3DEggLaunch::serialize(Archive& ar, unsigned int version) {
  if (version > i3degglaunch_version_) {
    log_fatal("Attempting to read version %u from file"
              " but running version %u of I3DEggLaunch class.",
              version, i3degglaunch_version_);
  }
  ar & make_nvp("launch", launch_);
}

std::ostream& I3DEggLaunch::Print(std::ostream& os) const {
  os << "I3DEggLaunch(launch=" << launch_ << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3DEggLaunch& launch) {
  return(launch.Print(os));
}


I3_SERIALIZABLE(I3DEggLaunch);
I3_SERIALIZABLE(I3DEggLaunchSeriesMap);
