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
