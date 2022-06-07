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
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

#define INICECONFIGS (IC_UNKNOWN) (IC_GUESS) (IC_EMPTY) (IC9) (IC22) \
  (IC40) (IC59) (IC79) (IC80) (IC86)
#define ICETOPCONFIGS (IT_UNKNOWN) (IT_GUESS) (IT_EMPTY) (IT16) (IT26) \
  (IT40) (IT59) (IT73) (IT80) (IT81)

void register_I3ScaleCalculator()
{

  scope scalecalculator_scope = 
    class_<I3ScaleCalculator>("I3ScaleCalculator", init<I3GeometryConstPtr,optional<I3ScaleCalculator::IceCubeConfig,I3ScaleCalculator::IceTopConfig> >
			      ("Args : I3Geometry, IceCubeConfig, IceTopConfig"))
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

}
