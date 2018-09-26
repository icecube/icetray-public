//
//   Copyright (c) 2008   Troy D. Straszheim and the IceCube Collaboration 
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

using namespace boost::python;
namespace bp = boost::python;

#include <phys-services/I3CutValues.h>
#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/physics/I3RecoPulse.h>

void register_I3CutValues()
{
  class_<I3CutValues, bases<I3FrameObject>, boost::shared_ptr<I3CutValues> >("I3CutValues")
    .def("calculate", &I3CutValues::Calculate)
    .def_readwrite("nchan", &I3CutValues::Nchan)
    .def_readwrite("nhit", &I3CutValues::Nhit)
    .def_readwrite("nString", &I3CutValues::Nstring)
    .def_readwrite("ndir", &I3CutValues::Ndir)
    .def_readwrite("ldir", &I3CutValues::Ldir)
    .def_readwrite("sdir", &I3CutValues::Sdir)
    .def_readwrite("sall", &I3CutValues::Sall)
    .def_readwrite("cog", &I3CutValues::cog)
    ;

  register_pointer_conversions<I3CutValues>();
}
