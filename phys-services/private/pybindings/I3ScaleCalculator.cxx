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

#include <phys-services/I3ScaleCalculator.h>

#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/geometry/I3Geometry.h>

#include <vector>

#include <dataclasses/physics/I3Particle.h>
#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/cat.hpp>

#ifndef ENUM_DEF
#define ENUM_DEF(r,data,T) .value(BOOST_PP_STRINGIZE(T), data::T)
#endif

// After retiring some configurations and instroducing some new ones:
// Includes the old (but ambiguous) "IC86" and "IT81" configs
#define INICECONFIGS (IC_UNKNOWN) (IC_GUESS) (IC_EMPTY) (IC_CUSTOM) \
  (IC79) (IC86) (IC79_SMOOTH) (IC79_STRICT) (IC86_SMOOTH) (IC86_STRICT) \
  (DEEPCORE_ALL) (DEEPCORE_BELOWDUST)
#define ICETOPCONFIGS (IT_UNKNOWN) (IT_GUESS) (IT_EMPTY) (IT_CUSTOM) \
  (IT73) (IT81) (IT73_SMOOTH) (IT73_STRICT) (IT81_SMOOTH) (IT81_STRICT) \
  (IT_INFILL_STA2_STRICT) (IT_INFILL_STA2_BIGOVAL) (IT_INFILL_TRIANGLE)

void register_I3ScaleCalculator()
{

  scope scalecalculator_scope = 
    class_<I3ScaleCalculator>("I3ScaleCalculator", init<I3GeometryConstPtr,optional<I3ScaleCalculator::IceCubeConfig,I3ScaleCalculator::IceTopConfig,
                              std::vector<int>,std::vector<int>,int,int> >
			      ("Args : I3Geometry, IceCubeConfig, IceTopConfig, Strings, Stations, TopDOMID, BottomDOMID"))
    ;
  
  enum_<I3ScaleCalculator::IceCubeConfig>("IceCubeConfig")
    BOOST_PP_SEQ_FOR_EACH(ENUM_DEF,I3ScaleCalculator,INICECONFIGS)
    .export_values()
    ;
  
  enum_<I3ScaleCalculator::IceTopConfig>("IceTopConfig")
     BOOST_PP_SEQ_FOR_EACH(ENUM_DEF,I3ScaleCalculator,ICETOPCONFIGS)
     .export_values()
     ;
  
  def("scale_inice", &I3ScaleCalculator::ScaleInIce,
      "Calculate the factor by which the border polygon of the IceCube volume needs to be scaled to exactly contain the track/vertex",
      arg("particle"));
  def("scale_xy", &I3ScaleCalculator::ScaleIceCubeDetectorPolygon,
      "Calculate the factor by which the area of the border polygon of the IceCube volume needs to be scaled to exactly contain a cascade vertex",
      arg("particle"));
  def("scale_icetop", &I3ScaleCalculator::ScaleIceTop,
      "Calculate the factor by which the border polygon of the IceTop surface are needs to be scaled to exactly contain the track/vertex",
      arg("particle"));
  def("vertex_is_inside", &I3ScaleCalculator::VertexIsInside,
      "Is the vertex position inside the IceCube volume?",
      arg("particle"));

  // For checking the boundary selection
  def("get_outer_strings", &I3ScaleCalculator::GetOuterStrings,
      "Vector of string numbers defining the boundary");
  def("get_outer_stations", &I3ScaleCalculator::GetOuterStations,
      "Vector of station numbers defining the boundary");

}
