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

#include <dataclasses/geometry/I3TankGeo.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

void register_I3TankGeo()
{

  //
  // I3TankGeo
  //
  {
    scope tankgeo = class_<I3TankGeo, boost::shared_ptr<I3TankGeo> >("I3TankGeo")
      #define TANKGEOPROPS (position)(orientation)(tankradius)(tankheight)(fillheight)(snowheight)(tanktype)
      BOOST_PP_SEQ_FOR_EACH(WRAP_RW_RECASE, I3TankGeo, TANKGEOPROPS )
      .def_readwrite("omkey_list",&I3TankGeo::omKeyList_)
      #undef TANKGEOPROPS
      .def(dataclass_suite<I3TankGeo>())
      ;

    enum_<I3TankGeo::TankType>("TankType")
      .value("NotSet", I3TankGeo::NotSet)
      .value("Tyvek_Lined", I3TankGeo::Tyvek_Lined)
      .value("Zirconium_Lined", I3TankGeo::Zirconium_Lined)
      .export_values()
      ; 
  }
  def("identity", identity_<I3TankGeo::TankType>);

  class_<I3StationGeo, bases<I3FrameObject>, I3StationGeoPtr>("I3StationGeo")
    .def(dataclass_suite<I3StationGeo>())
    ;

  class_<I3StationGeoMap, bases<I3FrameObject>, I3StationGeoMapPtr>("I3StationGeoMap")
    .def(dataclass_suite<I3StationGeoMap>())
    ;

  register_pointer_conversions<I3StationGeo>();
  register_pointer_conversions<I3StationGeoMap>();
}
