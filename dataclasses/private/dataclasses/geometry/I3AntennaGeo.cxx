#include <dataclasses/geometry/I3AntennaGeo.h>

I3AntennaGeo::~I3AntennaGeo() { }


template <class Archive>
void
I3AntennaGeo::serialize(Archive& ar, unsigned version) {
  if (version > i3antennageo_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3AntennaGeo class.", version, i3antennageo_version_);

  ar & make_nvp("Position", position_);
  ar & make_nvp("Orientation", orientation_);
  ar & make_nvp("AntennaType", antennaType_);
  ar & make_nvp("HeightAboveSnow", heightAboveSnow_);
  ar & make_nvp("AntennaName", antennaName_);
  ar & make_nvp("CableLength", cableLength_);
}

I3_SERIALIZABLE(I3AntennaGeo);
I3_SERIALIZABLE(I3AntennaGeoMap);

std::ostream& I3AntennaGeo::Print(std::ostream& os) const {
  os << "[I3AntennaGeo Position: " << position_ << '\n'
     << "           Orientation: " << orientation_ << '\n'
     << "           AntennaType: " << antennaType_ << '\n'
     << "           AntennaName: " << antennaName_ << '\n'
     << "       HeightAboveSnow: " << heightAboveSnow_ << '\n'
     << "           CableLength: " << cableLength_ << " ]";
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3AntennaGeo& g) {
  return (g.Print(os));
}

