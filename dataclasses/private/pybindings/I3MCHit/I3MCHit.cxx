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

#include <dataclasses/physics/I3MCHit.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

std::string to_str(const I3MCHit theHit){
    std::ostringstream oss;
    oss << theHit << std::flush;
    return oss.str();
}

void register_I3MCHit()
{
  {
    scope mchit_scope =

      class_<I3MCHit, boost::shared_ptr<I3MCHit> >("I3MCHit")
      .def(init<uint64_t,int>() )
      #define PROPERTIES (Time)(HitID)(CherenkovDistance)(HitSource)(Charge)(NPE)
      #define RO_PROPERTIES (ParticleMajorID)(ParticleMinorID)
      BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, I3MCHit, PROPERTIES)
      BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, I3MCHit, RO_PROPERTIES)
      #undef PROPERTIES
      #undef RO_PROPERTIES
      .def(dataclass_suite<I3MCHit>())
      .def("__str__", to_str)
      ;

    enum_<I3MCHit::HitSource>("I3MCHitSource")
      .value("UNKNOWN", I3MCHit::UNKNOWN)
      .value("SPE", I3MCHit::SPE)
      .value("RANDOM", I3MCHit::RANDOM)
      .value("AFTER_PULSE", I3MCHit::AFTER_PULSE)
      .value("PRE_PULSE", I3MCHit::PRE_PULSE)
      .value("ELASTIC_LATE_PULSE", I3MCHit::ELASTIC_LATE_PULSE)
      .value("INELASTIC_LATE_PULSE", I3MCHit::INELASTIC_LATE_PULSE)
      .value("EARLY_AFTER_PULSE", I3MCHit::EARLY_AFTER_PULSE)
      .export_values()
      ;
    def("identity", identity_<I3MCHit::HitSource>);
  }
}
