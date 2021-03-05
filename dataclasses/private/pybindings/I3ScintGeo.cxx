#include <dataclasses/geometry/I3ScintGeo.h>

#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

void register_I3ScintGeo() {

  {
    bp::scope scintgeo = bp::class_<I3ScintGeo, boost::shared_ptr<I3ScintGeo> >("I3ScintGeo")
                         .def(bp::init<>())
                         .add_property("position", &I3ScintGeo::GetPosition, &I3ScintGeo::SetPosition)
                         .add_property("orientation", &I3ScintGeo::GetOrientation, &I3ScintGeo::SetOrientation)
                         .add_property("heightAboveSnow", &I3ScintGeo::GetHeightAboveSnow, &I3ScintGeo::SetHeightAboveSnow)
                         .add_property("scintName", &I3ScintGeo::GetScintName, &I3ScintGeo::SetScintName)
                         .add_property("scintType", &I3ScintGeo::GetScintType, &I3ScintGeo::SetScintType)
                         .def(bp::dataclass_suite<I3ScintGeo>())
                         ;

    bp::enum_<I3ScintGeo::ScintType>("ScintType")
    .value("Unknown", I3ScintGeo::Unknown)
    .value("KIT", I3ScintGeo::KIT)
    .value("MAD", I3ScintGeo::MAD)
    .value("MADKIT", I3ScintGeo::MADKIT)

    ;
  }

  bp::class_<I3ScintGeoMap, bp::bases<I3FrameObject>, I3ScintGeoMapPtr>("I3ScintGeoMap")
  .def(bp::dataclass_suite<I3ScintGeoMap>())
  ;

  register_pointer_conversions<I3ScintGeoMap>();
}
