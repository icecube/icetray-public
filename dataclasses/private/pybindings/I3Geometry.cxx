//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id$
//
//   This file is part of IceTray.
//
//   IceTray is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
//   IceTray is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <vector>

#include <dataclasses/geometry/I3Geometry.h>
#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

// This function is overloaded.  Define the two separately for pybindings.
I3TankGeo wrapper_GetTankGeo1(const I3Geometry &self, const OMKey &key) {
  return self.GetTankGeo(key);
}
I3TankGeo wrapper_GetTankGeo2(const I3Geometry &self, const TankKey &tankkey) {
  return self.GetTankGeo(tankkey);
}


void register_I3Geometry()
{
    
    //
    // I3Geometry
    //
    bp::class_<I3Geometry, bp::bases<I3FrameObject>, boost::shared_ptr<I3Geometry> >("I3Geometry")
    #define GEOMPROPS (omgeo)(stationgeo)(scintgeo)(antennageo)(iceactgeo)(startTime)(endTime)
    BOOST_PP_SEQ_FOR_EACH(WRAP_RW_RECASE, I3Geometry, GEOMPROPS )
    #undef GEOMPROPS
    .def("tankgeo", wrapper_GetTankGeo1, bp::arg("key"))
    .def("tankgeo", wrapper_GetTankGeo2, bp::arg("tankkey"))
    .def(bp::dataclass_suite<I3Geometry>())
    ;
    
    register_pointer_conversions<I3Geometry>();
}
