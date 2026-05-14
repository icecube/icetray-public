// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <cmath>
#include <icetray/serialization.h>
#include <dataclasses/geometry/I3OMGeo.h>
#include <boost/preprocessor/seq/for_each.hpp>

namespace {
    enum OldOrientation__ {Unspecified=0, Up=1, Down=-1};
}

#define MAKE_OMTYPE_TO_STRING_CASE_LINE(r, namespace, t) case I3OMGeo::t : return BOOST_PP_STRINGIZE(t);
std::string to_string(const I3OMGeo::OMType& om_type)
{
  switch (om_type) {
      BOOST_PP_SEQ_FOR_EACH(MAKE_OMTYPE_TO_STRING_CASE_LINE, ~, I3OMGEO_H_I3OMGeo_OMType)
  }
  return(boost::lexical_cast<std::string>( om_type ));
}
#undef MAKE_OMTYPE_TO_STRING_CASE_LINE

std::ostream& operator<<(std::ostream& os, const I3OMGeo::OMType& om_type){
     os << to_string(om_type);
     return os;
}

#define MAKE_PMTTYPE_TO_STRING_CASE_LINE(r, namespace, t) case I3OMGeo::PMTType::t : return BOOST_PP_STRINGIZE(t);
std::string  to_string(const I3OMGeo::PMTType& pmt_type)
{
  switch (pmt_type) {
      BOOST_PP_SEQ_FOR_EACH(MAKE_PMTTYPE_TO_STRING_CASE_LINE, ~, I3OMGEO_H_I3OMGeo_PMTType)
  }
  return(boost::lexical_cast<std::string>( pmt_type ));
}
#undef MAKE_PMTTYPE_TO_STRING_CASE_LINE

std::ostream& operator<<(std::ostream& os, const I3OMGeo::PMTType& pmt_type){
     os << to_string(pmt_type);
     return os;
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
    if (version >= 2) {
        ar & make_nvp("PMTType", pmttype);
    }
}

I3_SERIALIZABLE(I3OMGeo);
I3_SERIALIZABLE(I3OMGeoMap);

std::ostream& I3OMGeo::Print(std::ostream& os) const{
    os << "[I3OMGeo Position: " << position << '\n'
       << "      Orientation: " << orientation << '\n'
       << "           OMType: " << omtype << '\n'
       << "          PMTType: " << pmttype << '\n'
       << "             Area: " << area << " ]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const I3OMGeo& g){
    return(g.Print(os));
}

double I3OMGeo::GetPMTBeta() const {
  // from NEXTGEN/om.conf in ice-models
	switch (omtype) {
  case I3OMGeo::AMANDA:
	case I3OMGeo::IceCube:
	case I3OMGeo::PDOM:
		return 0.49;
  case I3OMGeo::mDOM:
    return 0.95;
  case I3OMGeo::DEgg:
    return 0.5;
  case I3OMGeo::LOM:
  case I3OMGeo::LOM18:
    return 1.;
  case I3OMGeo::LOM16:
    return 0.97;
  case I3OMGeo::WOM:
    // Assuming (cross sectional) area is 2rh and surface area 2pirh, solve for beta that gives the expected direcionally averaged area: pi rh / 2
    return -2. / M_PI - 1.;
  case I3OMGeo::FOM:
    // placeholder for now, angsens calculation needs to be updated still
    return 1.;
  default:
    return 0.;
  }
}

double I3OMGeo::GetAverageArea() const {
  return std::fabs(area / ( 2. * (1. + I3OMGeo::GetPMTBeta())));
}

double I3OMGeo::GetCurvedArea() const {
  return 4. * GetAverageArea();
}
