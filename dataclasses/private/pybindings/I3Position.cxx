//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id$
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

#include <dataclasses/I3Position.h>
#include <dataclasses/I3Direction.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/ostream_overloads.hpp>

using namespace boost::python;

tuple i3position_to_tuple(const I3Position& p)
{
  return make_tuple(p.GetX(), p.GetY(), p.GetZ());
}

unsigned i3position_len(const I3Position&) { return 3; }

double i3position_getitem(const I3Position& self, unsigned index) 
{ 
  switch(index)
    {
    case 0:
      return self.GetX();
    case 1:
      return self.GetY();
    case 2:
      return self.GetZ();
    default:
      throw std::out_of_range("index out of range");
    }
}

void i3position_setitem(I3Position& self, unsigned index, double value) 
{ 
  switch(index)
    {
    case 0:
      self.SetX(value);
      break;
    case 1:
      self.SetY(value);
      break;
    case 2:
      self.SetZ(value);
      break;
    default:
      throw std::out_of_range("index out of range");
    }
}

void register_I3Position()
{

  def("i3position_to_tuple", i3position_to_tuple);

  scope position_scope = 
    class_<I3Position, bases<I3FrameObject>, boost::shared_ptr<I3Position> >
    ("I3Position",
     "I3Position objects can subscripted like 3-element arrays (x, y, z) and converted to tuples and lists")
    .def(init<I3Direction>())
    .def(init<double,double,double>())
    .def(init<double,double,double,I3Position::RefFrame>())
    PROPERTY(I3Position, x, X)
    PROPERTY(I3Position, y, Y)
    PROPERTY(I3Position, z, Z)
#define RO_PROPERTIES (R)(Theta)(Phi)(Rho)
    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, I3Position, RO_PROPERTIES)
#undef  RO_PROPERTIES
    .def("rotate_x",&I3Position::RotateX)
    .def("rotate_y",&I3Position::RotateY)
    .def("rotate_z",&I3Position::RotateZ)
    .add_property("magnitude",&I3Position::Magnitude)
    .def("__abs__", &I3Position::Magnitude)
    .add_property("mag2",&I3Position::Mag2)
    .def("get_unit_vector",&I3Position::GetUnitVector)
    .def("normalize",&I3Position::Normalize)
    .def(-self)
    .def(self += self)    
#ifdef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
#endif
    .def(self -= self)
#ifdef __clang__
#pragma GCC diagnostic pop
#endif    
    .def(self + self)
    .def(self - self)
    .def(self * self)
    .def(self * I3Direction())
    .def(self *= double())
    .def(self /= double())
    .def(self * double())
    .def(double() * self)
    .def(self / double())
    .def(self == self)
    .def(self != self)
    .def("cross", (I3Position(I3Position::*)(const I3Direction&)const)&I3Position::Cross)
    .def("cross", (I3Position(I3Position::*)(const I3Position&)const)&I3Position::Cross)
    .def("__len__", i3position_len)
    .def("__getitem__", i3position_getitem)
    .def("__setitem__", i3position_setitem)
    .def("__repr__", &stream_to_string<I3Position>)
    .def(dataclass_suite<I3Position>())
    ;

  enum_<I3Position::RefFrame>("RefFrame")
    .value("car",I3Position::car)
    .value("sph",I3Position::sph)
    .value("cyl",I3Position::cyl)
    .export_values()
    ;

  register_pointer_conversions<I3Position>();

  implicitly_convertible<I3Position, tuple>();
  
}
