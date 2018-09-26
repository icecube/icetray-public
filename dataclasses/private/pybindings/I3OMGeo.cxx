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

#include <dataclasses/geometry/I3OMGeo.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

void register_I3OMGeo()
{

  //
  // I3OMGeo
  //
  {
    scope omg = class_<I3OMGeo, boost::shared_ptr<I3OMGeo> >("I3OMGeo")
      .add_property("direction", &I3OMGeo::GetDirection)
      #define I3OMGEOPROPS (position)(omtype)(orientation)(area) 
      BOOST_PP_SEQ_FOR_EACH(WRAP_RW_RECASE, I3OMGeo, I3OMGEOPROPS )
      #undef I3OMGEOPROPS
      .def(dataclass_suite<I3OMGeo>())
      ;

    enum_<I3OMGeo::OMType>("OMType")
      .value("UnknownType", I3OMGeo::UnknownType)
      .value("AMANDA", I3OMGeo::AMANDA)
      .value("IceCube", I3OMGeo::IceCube)
      .value("IceTop", I3OMGeo::IceTop)
      .value("Scintillator", I3OMGeo::Scintillator)
      .value("IceAct", I3OMGeo::IceAct)
      .value("PDOM", I3OMGeo::PDOM)
      .value("DEgg", I3OMGeo::DEgg)
      .value("mDOM", I3OMGeo::mDOM)
      .value("WOM", I3OMGeo::WOM)
      .value("FOM", I3OMGeo::FOM)
      .export_values()
      ;
    ;

  }
  def("identity", identity_<I3OMGeo::OMType>);

  class_<I3OMGeoMap, bases<I3FrameObject>, I3OMGeoMapPtr>("I3OMGeoMap")
    .def(dataclass_suite<I3OMGeoMap>())
    ;

  register_pointer_conversions<I3OMGeoMap>();
}
