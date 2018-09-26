#include <icetray/serialization.h>
#include <dataclasses/geometry/I3OMGeo.h>

I3OMGeo::~I3OMGeo() { }

namespace {
    enum OldOrientation__ {Unspecified=0, Up=1, Down=-1};
}

template <class Archive>
void 
I3OMGeo::serialize(Archive& ar, unsigned version)
{
    if (version>i3omgeo_version_)
        log_fatal("Attempting to read version %u from file but running version %u of I3OMGeo class.",version,i3omgeo_version_);
    
    ar & make_nvp("Position", position);
    if (version==0) {
        OldOrientation__ orientation_=Unspecified;
        ar & make_nvp("Orientation", orientation_);
        
        // convert orientation enum to direction
        switch (orientation_) {
            case Unspecified: orientation.ResetOrientation(); break;
            case Up: orientation.SetOrientation(0.,0.,1., 1.,0.,0.); break;
            case Down: orientation.SetOrientation(0.,0.,-1., 1.,0.,0.); break;
            default: orientation.ResetOrientation(); break;
        } 
    } else {
        ar & make_nvp("Orientation", orientation);
    }
    ar & make_nvp("OMType", omtype);
    ar & make_nvp("Area", area);
    if (version==0) {
        double aziangle=NAN;
        // this has never been used for anything
        // we could have used this to fill the orientation class,
        // but the value ended up to mostly be un-initialized.
        // Ignore it when reading older files.
        ar & make_nvp("AziAngle", aziangle);
    }
}

I3_SERIALIZABLE(I3OMGeo);
I3_SERIALIZABLE(I3OMGeoMap);

std::ostream& I3OMGeo::Print(std::ostream& os) const{
    os << "[I3OMGeo Position: " << position << '\n'
       << "      Orientation: " << orientation << '\n'
       << "           OMType: " << omtype << '\n'
       << "             Area: " << area << " ]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const I3OMGeo& g){
    return(g.Print(os));
}
