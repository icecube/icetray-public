/**
 *  $Id$
 *  
 *  Copyright (C) 2012
 *  the IceCube Collaboration <http://www.icecube.wisc.edu>
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
#include <dataclasses/ModuleKey.h>
#include <icetray/python/boost_serializable_pickle_suite.hpp>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

inline static unsigned 
hash_modulekey (const ModuleKey& key)
{
  return ModuleKey::hash()(key); 
}

typedef ModuleKey value_type;
// make ModuleKey iterable: string,om,pmt = ModuleKey
static object modulekey_getitem(value_type const& x, int i) {
    if (i==0 || i==-2) return object(x.GetString());
    else if (i==1 || i==-1) return object(x.GetOM()); 
    else {
        PyErr_SetString(PyExc_IndexError,"Index out of range.");
        throw_error_already_set();
        return object(); // None
    }
}
// __len__ = 2
static int modulekey_len(value_type const& x) { return 2; }

std::string repr(const ModuleKey& key){
  std::stringstream s;
  s << "ModuleKey(" << key.GetString() << "," << key.GetOM() << ")";
  return s.str();
}

void
register_ModuleKey()
{
  class_<ModuleKey>("ModuleKey")
    .def(init<int,unsigned>())
    PROPERTY(ModuleKey, string, String)
    PROPERTY(ModuleKey, om, OM)
    .def("__repr__", repr)
    .def("__hash__", hash_modulekey)
    .def("__getitem__", modulekey_getitem)
    .def("__len__", modulekey_len)
    .def_pickle(boost_serializable_pickle_suite<ModuleKey>())
    .def(copy_suite<ModuleKey>())
    .def(operator_suite<ModuleKey>())
    .def(freeze())
    ;

  from_python_sequence<std::vector<ModuleKey>, variable_capacity_policy>();

    
  class_<I3MapModuleKeyString, bases<I3FrameObject>, I3MapModuleKeyStringPtr>("I3MapModuleKeyString")
    .def(dataclass_suite<I3MapModuleKeyString>())
    ;
  register_pointer_conversions<I3MapModuleKeyString>();

}
