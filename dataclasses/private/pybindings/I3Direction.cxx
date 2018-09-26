//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
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

#include <dataclasses/I3Direction.h>
#include <dataclasses/I3Position.h>
#include <dataclasses/ostream_overloads.hpp>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

void register_I3Direction()
{

  class_<I3Direction, bases<I3FrameObject>, boost::shared_ptr<I3Direction> >("I3Direction")
    .def(init<I3Position>())
    .def(init<double,double>())
    .def(init<double,double,double>())
    .def("set_theta_phi", &I3Direction::SetThetaPhi)
    .def("rotate_x", &I3Direction::RotateX)
    .def("rotate_y", &I3Direction::RotateY)
    .def("rotate_z", &I3Direction::RotateZ)
    .def("cross", (I3Direction(I3Direction::*)(const I3Direction&)const)&I3Direction::Cross)
    .def("cross", (I3Position(I3Direction::*)(const I3Position&)const)&I3Direction::Cross)
    .def(-self)
    .def(self * self)
    .def(self * I3Position())
    .def(self * double())
    .def(double() * self)
    .def(self / double())
    .def(self == self)
    .def(self != self)
    .def("angle", &I3Direction::Angle)
    #define RO_PROPERTIES (Zenith)(Azimuth)(X)(Y)(Z)
    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, I3Direction, RO_PROPERTIES)
    #undef  RO_PROPERTIES
    .add_property("theta", &I3Direction::CalcTheta)
    .add_property("phi", &I3Direction::CalcPhi)
    .def(self == self)
    .def(dataclass_suite<I3Direction>())
    ;
  register_pointer_conversions<I3Direction>();
}
