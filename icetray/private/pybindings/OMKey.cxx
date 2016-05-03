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
#include <icetray/python/boost_serializable_pickle_suite.hpp>

using namespace boost::python;

inline static unsigned 
hash_omkey (const OMKey& key)
{
  return OMKey::hash()(key); 
}

typedef OMKey value_type;
// make OMKey iterable: string,om,pmt = OMKey
static object omkey_getitem(value_type const& x, int i) {
    if (i==0 || i==-3) return object(x.GetString());
    else if (i==1 || i==-2) return object(x.GetOM()); 
    else if (i==2 || i==-1) return object(x.GetPMT()); 
    else {
        PyErr_SetString(PyExc_IndexError,"Index out of range.");
        throw_error_already_set();
        return object(); // None
    }
}
// __len__ = 3
static int omkey_len(value_type const& x) { return 3; }

std::string repr(const OMKey& key){
  std::stringstream s;
  s << "OMKey(" << key.GetString() << "," << key.GetOM() << "," << static_cast<unsigned int>(key.GetPMT()) << ")";
  return s.str();
}

void
register_OMKey()
{
  class_<OMKey>("OMKey")
    .def(init<int,unsigned>())
    .def(init<int,unsigned,unsigned char>())
    PROPERTY(OMKey, string, String)
    PROPERTY(OMKey, om, OM)
    PROPERTY(OMKey, pmt, PMT)
    .def("is_inice", &OMKey::IsInIce)
    .def("is_icetop", &OMKey::IsIceTop)
    .def("is_scintillator", &OMKey::IsScintillator)
    .def("__str__", &OMKey::str)
    .def("__repr__", repr)
    .def("__hash__", hash_omkey)
    .def("__getitem__", omkey_getitem)
    .def("__len__", omkey_len)
    .def(self == self)
    .def(self != self)
    .def(self < self)
    .def_pickle(boost_serializable_pickle_suite<OMKey>())
    ;

  from_python_sequence<std::vector<OMKey>, variable_capacity_policy>();
}
