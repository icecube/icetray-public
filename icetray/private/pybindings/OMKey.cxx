/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */
#include <icetray/OMKey.h>

using namespace boost::python;

inline static unsigned 
hash_omkey (const OMKey& key)
{
  return OMKey::hash()(key); 
}

void
register_OMKey()
{
  class_<OMKey>("OMKey")
    //    .def(init<>())
    .def(init<int,unsigned>())
    .def("GetString", &OMKey::GetString)
    .def("SetString", &OMKey::SetString)
    .def("GetOM", &OMKey::GetOM)
    .def("SetOM", &OMKey::SetOM)
    .def("__str__", &OMKey::str)
    .def("__repr__", &OMKey::str)
    .def("__hash__", hash_omkey)
    .def(self == self)
    .def(self < self)
    ;

  from_python_sequence<std::vector<OMKey>, variable_capacity_policy>();
}
