// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dataclasses/geometry/I3AntennaGeo.h>

#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

void register_I3AntennaGeo() {

  {
    bp::scope antgeo = bp::class_<I3AntennaGeo, boost::shared_ptr<I3AntennaGeo> >("I3AntennaGeo")
                       .def(bp::init<>())
                       .add_property("position", &I3AntennaGeo::GetPosition, &I3AntennaGeo::SetPosition)
                       .add_property("orientation", &I3AntennaGeo::GetOrientation, &I3AntennaGeo::SetOrientation)
                       .add_property("heightAboveSnow", &I3AntennaGeo::GetHeightAboveSnow, &I3AntennaGeo::SetHeightAboveSnow)
                       .add_property("antennaName", &I3AntennaGeo::GetAntennaName, &I3AntennaGeo::SetAntennaName)
                       .def(bp::dataclass_suite<I3AntennaGeo>())
                       ;
  }

  bp::class_<I3AntennaGeoMap, bp::bases<I3FrameObject>, I3AntennaGeoMapPtr>("I3AntennaGeoMap")
  .def(bp::dataclass_suite<I3AntennaGeoMap>())
  ;

  register_pointer_conversions<I3AntennaGeoMap>();
}
