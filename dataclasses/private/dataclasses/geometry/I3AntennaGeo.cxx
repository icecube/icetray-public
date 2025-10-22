// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dataclasses/geometry/I3AntennaGeo.h>
#include <dataclasses/calibration/I3AntennaCalibration.h>

I3AntennaGeo::~I3AntennaGeo() { }


template <class Archive>
void
I3AntennaGeo::serialize(Archive& ar, unsigned version) {
  if (version > i3antennageo_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3AntennaGeo class.", version, i3antennageo_version_);

  ar & make_nvp("Position", position_);
  ar & make_nvp("Orientation", orientation_);
  if (version == 0) {
    int dummyAntennaType = 0;
    ar & make_nvp("AntennaType", dummyAntennaType);
  }
  ar & make_nvp("HeightAboveSnow", heightAboveSnow_);
  ar & make_nvp("AntennaName", antennaName_);
  if (version == 0) {
    double dummyCableLength = 0.;
    ar & make_nvp("CableLength", dummyCableLength);
  }
}

I3_SERIALIZABLE(I3AntennaGeo);
I3_SERIALIZABLE(I3AntennaGeoMap);

std::ostream& I3AntennaGeo::Print(std::ostream& os) const {
  os << "[I3AntennaGeo Position: " << position_ << '\n'
     << "           Orientation: " << orientation_ << '\n'
     << "           AntennaName: " << antennaName_ << '\n'
     << "       HeightAboveSnow: " << heightAboveSnow_ << " ]";
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3AntennaGeo& g) {
  return (g.Print(os));
}

