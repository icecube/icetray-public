#include <icetray/serialization.h>
#include <dataclasses/geometry/I3TankGeo.h>

I3TankGeo::~I3TankGeo() {}

template <class Archive>
  void I3TankGeo::serialize(Archive& ar, unsigned version)
  {
    if (version>i3tankgeo_version_)
      log_fatal("Attempting to read version %u from file but running version %u of I3TankGeo class.",version,i3tankgeo_version_);

    ar & make_nvp("Position", position);
    ar & make_nvp("Orientation", orientation);
    ar & make_nvp("TankRadius", tankradius);
    ar & make_nvp("TankHeight", tankheight);
    ar & make_nvp("FillHeight", fillheight);
    if (version > 0)
      ar & make_nvp("OMKeyList", omKeyList_);
    if (version > 1){
      ar & make_nvp("SnowHeight", snowheight);
      ar & make_nvp("TankType", tanktype);
    }
  }

I3_SERIALIZABLE(I3TankGeo);
I3_SERIALIZABLE(I3StationGeo);
I3_SERIALIZABLE(I3StationGeoMap);

std::ostream& I3TankGeo::Print(std::ostream& os) const{
  os << "[I3TankGo:\n"
     << "     Position: " << position << '\n'
     << "  Orientation: " << orientation << '\n'
     << "  Tank Radius: " << tankradius << '\n'
     << "  Tank Height: " << tankheight << '\n'
     << "  Fill Height: " << fillheight << '\n'
     << "          OMs: " << omKeyList_ << '\n'
     << "  Snow Height: " << snowheight << '\n'
     << "    Tank Type: " << tanktype << ']';
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3TankGeo& tg){
  return(tg.Print(os));
}
