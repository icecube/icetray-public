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

#include <phys-services/I3Cuts.h>
#include "dataclasses/physics/I3Particle.h"
//#include "dataclasses/physics/I3RecoPulse.h"
//#include "dataclasses/geometry/I3Geometry.h"
//#include "dataclasses/I3Position.h"


void register_I3Cuts()
{
  // map the I3Cuts namespace to a sub-module  
  // make "from phys-services import <whatever>" work  
  bp::object I3CutsModule(bp::handle<>(bp::borrowed(PyImport_AddModule("phys-services.I3Cuts"))));  
  // make "from phys-services import I3Cuts" work  
  bp::scope().attr("I3Cuts") = I3CutsModule;  
  // set the current scope to the new sub-module  
  bp::scope I3Cuts_scope = I3CutsModule;  
  // export stuff in the I3Cuts namespace  
  def("containment_area_size", I3Cuts::ContainmentAreaSize,
      "I3Cuts::ContainmentAreaSize(const I3Particle &track, std::vector< double > x, std::vector< double > y, double z)");
  def("containment_volume_size", I3Cuts::ContainmentVolumeSize,
      "I3Cuts::ContainmentVolumeSize(const I3Particle &track, std::vector< double > x, std::vector< double > y, double zhigh, double zlow)");
  def("cynlinder_size", I3Cuts::CylinderSize,
      "I3Cuts::CylinderSize(const I3Particle &track, double H0, double R0, double center)");
  // Given the heavy weight of I3Cuts/I3CutsCalc and the current impl,
  //   wrapping more of this namespace is not practical.  New tools are
  //   better idea.

}
