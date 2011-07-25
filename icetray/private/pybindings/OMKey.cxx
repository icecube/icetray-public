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

typedef OMKey value_type;
// make OMKey iterable: string,om = OMKey
static object pair_getitem(value_type const& x, int i) {
    if (i==0 || i==-2) return object(x.GetString());
    else if (i==1 || i==-1) return object(x.GetOM()); 
    else {
        PyErr_SetString(PyExc_IndexError,"Index out of range.");
        throw_error_already_set();
        return object(); // None
    }
}
// __len__ std::pair = 2
static int pair_len(value_type const& x) { return 2; }

void
register_OMKey()
{
  class_<OMKey>("OMKey")
    //    .def(init<>())
    .def(init<int,unsigned>())
    PROPERTY(OMKey, string, String)
    PROPERTY(OMKey, om, OM)
    .def("__str__", &OMKey::str)
    .def("__repr__", &OMKey::str)
    .def("__hash__", hash_omkey)
    .def("__getitem__", pair_getitem)
    .def("__len__", pair_len)
    .def(self == self)
    .def(self < self)
    ;

  from_python_sequence<std::vector<OMKey>, variable_capacity_policy>();
}
