//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id$
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
