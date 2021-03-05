#include <dataclasses/geometry/I3IceActGeo.h>

#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

void register_I3IceActGeo() {

  {
    bp::scope iceactgeo = bp::class_<I3IceActGeo, boost::shared_ptr<I3IceActGeo> >("I3IceActGeo")
                          .def(bp::init<>())
                          .add_property("position", &I3IceActGeo::GetPosition, &I3IceActGeo::SetPosition)
                          .add_property("orientation", &I3IceActGeo::GetOrientation, &I3IceActGeo::SetOrientation)
                          .add_property("iceactName", &I3IceActGeo::GetIceActName, &I3IceActGeo::SetIceActName)
                          .add_property("iceactType", &I3IceActGeo::GetIceActType, &I3IceActGeo::SetIceActType)
                          .add_property("numberofPixels", &I3IceActGeo::GetNumberofPixels, &I3IceActGeo::SetNumberofPixels)
                          .add_property("pixelSize", &I3IceActGeo::GetPixelSize, &I3IceActGeo::SetPixelSize)
                          .add_property("pixelPositions", &I3IceActGeo::GetPixelsPositions, &I3IceActGeo::SetPixelsPositions)
                          .add_property("pixelstoChannels", &I3IceActGeo::GetPixelstoChannels, &I3IceActGeo::SetPixelstoChannels)
                          .def(bp::dataclass_suite<I3IceActGeo>())
                          ;
    bp::enum_<I3IceActGeo::IceActType>("IceActType")
    .value("Unknown", I3IceActGeo::Unknown)
    .value("drs_2019", I3IceActGeo::drs_2019)
    .value("target_C", I3IceActGeo::target_C)
    .value("drs_2019_64", I3IceActGeo::drs_2019_64)
    .value("simulation", I3IceActGeo::simulation)
    ;
  }

  bp::class_<I3IceActGeoMap, bp::bases<I3FrameObject>, I3IceActGeoMapPtr>("I3IceActGeoMap")
  .def(bp::dataclass_suite<I3IceActGeoMap>())
  ;

  register_pointer_conversions<I3IceActGeoMap>();
}

void register_I3IceActPixelPositionMap() {

  bp::class_<I3IceActPixelPositionMap, bp::bases<I3FrameObject>, I3IceActPixelPositionMapPtr>("I3IceActPixelPositionMap")
  .def(bp::dataclass_suite<I3IceActPixelPositionMap>());

  register_pointer_conversions<I3IceActPixelPositionMap>();
}
