//
//   Copyright (c) 2012   the IceCube collaboration
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

#include <dataclasses/geometry/I3ModuleGeo.h>
#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

#define ENUM_DEF(r,data,T) .value(BOOST_PP_STRINGIZE(T), data::T)

void register_I3ModuleGeo()
{
    {
        bp::scope i3modulegeo_scope = 
        bp::class_<I3ModuleGeo, boost::shared_ptr<I3ModuleGeo> >("I3ModuleGeo")
        .def(bp::init<>() )
#define PROPERTIES (ModuleType)(Radius)
#define RO_PROPERTIES (Dir)
        BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, I3ModuleGeo, PROPERTIES)
        BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, I3ModuleGeo, RO_PROPERTIES)
#undef PROPERTIES
#undef RO_PROPERTIES
        .add_property("pos",
                      make_function((const I3Position& (I3ModuleGeo::*)()) &I3ModuleGeo::GetPos,
                                    bp::return_internal_reference<1>()
                                   ),
                      (void (I3ModuleGeo::*)(const I3Position&)) &I3ModuleGeo::SetPos
                     ) 
        .add_property("orientation",
                      make_function((const I3Orientation& (I3ModuleGeo::*)()) &I3ModuleGeo::GetOrientation,
                                    bp::return_internal_reference<1>()
                                    ),
                      (void (I3ModuleGeo::*)(const I3Orientation&)) &I3ModuleGeo::SetOrientation
                      )
        .def(bp::dataclass_suite<I3ModuleGeo>())
        ;

        bp::enum_<I3ModuleGeo::ModuleType>("ModuleType")
        BOOST_PP_SEQ_FOR_EACH(ENUM_DEF,I3ModuleGeo,I3MODULEGEO_H_I3ModuleGeo_ModuleType)
        .export_values()
        ;
    }
    bp::def("identity", identity_<I3ModuleGeo::ModuleType>);

    bp::class_<I3ModuleGeoMap, bp::bases<I3FrameObject>, I3ModuleGeoMapPtr>("I3ModuleGeoMap")
    .def(bp::dataclass_suite<I3ModuleGeoMap>())
    ;

    register_pointer_conversions<I3ModuleGeoMap>();
}
