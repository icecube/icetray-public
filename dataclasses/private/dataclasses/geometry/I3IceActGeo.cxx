#include <dataclasses/geometry/I3IceActGeo.h>

I3IceActGeo::~I3IceActGeo() { }


template <class Archive>
void
I3IceActGeo::serialize(Archive& ar, unsigned version) {
  if (version > i3iceactgeo_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3IceActGeo class.", version, i3iceactgeo_version_);

  ar & make_nvp("Position", position);
  ar & make_nvp("Orientation", orientation);
  ar & make_nvp("IceActType", iceactType);
  ar & make_nvp("IceActName", iceactName);
  ar & make_nvp("NumberofPixels", numberofPixels);
  ar & make_nvp("PixelSize", pixelSize);
  ar & make_nvp("PixelstoChannels", pixelstoChannels);
  ar & make_nvp("PixelPositions", pixelPositions);
}

I3_SERIALIZABLE(I3IceActGeo);
I3_SERIALIZABLE(I3IceActGeoMap);
I3_SERIALIZABLE(I3IceActPixelPositionMap);

std::ostream& I3IceActGeo::Print(std::ostream& os) const {
  os << "[I3IceActGeo Position: " << position << '\n'
     << "          Orientation: " << orientation << '\n'
     << "           IceActType: " << iceactType << '\n'
     << "           IceActName: " << iceactName << "\n"
     << "       NumberofPixels: " << numberofPixels << "\n"
     << "            PixelSize: " << pixelSize << "]";
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3IceActGeo& g) {
  return (g.Print(os));
}

