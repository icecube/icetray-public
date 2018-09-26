//
//   Copyright (c) 2012   the IceCube collaboration
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
