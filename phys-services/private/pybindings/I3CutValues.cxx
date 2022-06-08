//
//   Copyright (c) 2008   Troy D. Straszheim and the IceCube Collaboration 
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
