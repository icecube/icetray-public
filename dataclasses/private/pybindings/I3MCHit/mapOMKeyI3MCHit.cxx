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

void register_mapOMKeyI3MCHit()
{
  class_<std::map<OMKey, I3MCHit> >("map_OMKey_I3MCHit")
    .def(dataclass_suite<std::map<OMKey, I3MCHit> >())
    ;
}
