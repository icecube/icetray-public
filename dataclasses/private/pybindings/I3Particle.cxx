//
//   Copyright (c) 2020 The IceCube Collaboration
//   
//   $Id$
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

#include <dataclasses/physics/I3Particle.h>
#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/cat.hpp>
#include <dataclasses/ostream_overloads.hpp>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

bool I3ParticleVect_eq(const std::vector<I3Particle> &lhs,bp::list &rhs)
{
    unsigned long n = bp::len(rhs);
    if (lhs.size() != n)
        return false;
    for(unsigned long i=0;i<n;i++)
    {
        try
        {
            extract<I3Particle> extractor(rhs[i]);
            if (!extractor.check() || lhs[i] != extractor())
                return false;
        }
        catch (int e)
        {
            std::cout << "cannot convert to I3Particle at position " << i << std::endl;
            return false;
        }
    }
    return true;
}

I3Position shift_along_track(const I3Particle& p, double distance){
  std::cout<<"I3Particle.shift_along_track is DEPRECATED: It's trivial to shift yourself."<<std::endl;
  return p.GetPos() + distance * p.GetDir();
}

#define ENUM_DEF(r,data,T) .value(BOOST_PP_STRINGIZE(T), data::T)

void register_I3Particle()
{
    class_<I3Particle, bases<I3FrameObject>, boost::shared_ptr<I3Particle> > particle =
      class_<I3Particle, bases<I3FrameObject>, boost::shared_ptr<I3Particle> >("I3Particle")
      #define RO_PROPERTIES (MajorID)(MinorID)(ID)(Mass)
      #define PROPERTIES (Time)(Energy)(TotalEnergy)(KineticEnergy)(Shape)(Type)(PdgEncoding)(Length)(Speed)(FitStatus)(LocationType) \
                         (ShapeString)(TypeString)(FitStatusString)(LocationTypeString)
      #define CONVENIENCE_BOOLS (IsTrack)(IsCascade)(IsTopShower)(IsNeutrino)(HasMass)
      BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, I3Particle, RO_PROPERTIES)
      BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, I3Particle, PROPERTIES)
      BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_BOOL, I3Particle, CONVENIENCE_BOOLS)
      #undef RO_PROPERTIES
      #undef PROPERTIES
      #undef CONVENIENCE_BOOLS
      
      .add_property("pos", make_function( (const I3Position& (I3Particle::*)()) &I3Particle::GetPos, return_internal_reference<1>() ),
                                          (void (I3Particle::*)(const I3Position&)) &I3Particle::SetPos )
      .add_property("dir", make_function( (const I3Direction& (I3Particle::*)()) &I3Particle::GetDir, return_internal_reference<1>() ),
                                          (void (I3Particle::*)(const I3Direction&)) &I3Particle::SetDir )
      .def("shift_along_track", 
           shift_along_track,
           (arg("dist")),
           "get the position of the point at that distance of the vertex along the track in forward direction")
      .def("shift_time_track", 
           &I3Particle::ShiftTimeTrack,
           (arg("time")),
           "get the position of the particle at this time")
      .def("mass_for_type", &I3Particle::GetMassForType).staticmethod("mass_for_type")
      .def("clone", &I3Particle::Clone)
      .def(dataclass_suite<I3Particle>())
      ;
    {
    scope particle_scope(particle);

    enum_<I3Particle::FitStatus>("FitStatus")
      BOOST_PP_SEQ_FOR_EACH(ENUM_DEF,I3Particle,I3PARTICLE_H_I3Particle_FitStatus)
      .export_values()
      ;

    enum_<I3Particle::LocationType>("LocationType")
      BOOST_PP_SEQ_FOR_EACH(ENUM_DEF,I3Particle,I3PARTICLE_H_I3Particle_LocationType)
      .export_values()
      ;

    enum_<I3Particle::ParticleType>("ParticleType")
      BOOST_PP_SEQ_FOR_EACH(ENUM_DEF,I3Particle,I3PARTICLE_H_I3Particle_ParticleType)
      .export_values()
      ;

    class_<std::vector<I3Particle::ParticleType> >("ParticleTypeVect")
      .def(dataclass_suite<std::vector<I3Particle::ParticleType> >());

    enum_<I3Particle::ParticleShape>("ParticleShape")
      BOOST_PP_SEQ_FOR_EACH(ENUM_DEF,I3Particle,I3PARTICLE_H_I3Particle_ParticleShape)
      .export_values()
      ;
  }
  particle.def(init<I3Particle::ParticleShape, 
                    I3Particle::ParticleType>((arg("shape")=I3Particle::Null, arg("type")=I3Particle::unknown), 
    "Constructor for a simple particle with generated ID"));

  particle.def(init<const I3Position, const I3Direction, const double, I3Particle::ParticleShape, double>((arg("pos"), 
    arg("dir"), arg("vertextime"), arg("shape")=I3Particle::Null, arg("length")=NAN), 
    "Constructor for a track or ray"));
  
  bp::def("identity", identity_<I3Particle::FitStatus>);
  bp::def("identity", identity_<I3Particle::LocationType>);
  bp::def("identity", identity_<I3Particle::ParticleType>);
  bp::def("identity", identity_<I3Particle::ParticleShape>);
  
  bp::implicitly_convertible<I3Particle,I3ParticleID>();

  class_<std::vector<I3Particle> >("ListI3Particle")
    .def(dataclass_suite<std::vector<I3Particle> >())
    ;

  register_pointer_conversions<I3Particle>();
}
