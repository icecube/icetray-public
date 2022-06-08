//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//
//   This file is part of IceTray.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//   
//   THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
//   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
//   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
//   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
//   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//   SUCH DAMAGE.
//   
//   SPDX-License-Identifier: BSD-2-Clause
//   
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
