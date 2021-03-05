#include <dataclasses/geometry/I3ScintGeo.h>

I3ScintGeo::~I3ScintGeo() { }


template <class Archive>
void
I3ScintGeo::serialize(Archive& ar, unsigned version) {
  if (version > i3scintgeo_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3ScintGeo class.", version, i3scintgeo_version_);

  ar & make_nvp("Position", position);
  ar & make_nvp("Orientation", orientation);
  ar & make_nvp("ScintType", scintType);
  ar & make_nvp("HeightAboveSnow", heightAboveSnow);
  ar & make_nvp("ScintName", scintName);
}

I3_SERIALIZABLE(I3ScintGeo);
I3_SERIALIZABLE(I3ScintGeoMap);

std::ostream& I3ScintGeo::Print(std::ostream& os) const {
  os << "[I3ScintGeo Position: " << position << '\n'
     << "         Orientation: " << orientation << '\n'
     << "           ScintType: " << scintType << '\n'
     << "     HeightAboveSnow: " << heightAboveSnow << '\n'
     << "           ScintName: " << scintName << " ]";
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3ScintGeo& g) {
  return (g.Print(os));
}

