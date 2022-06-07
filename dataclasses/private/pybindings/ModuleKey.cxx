/**
 *  $Id$
 *  
 *  Copyright (C) 2012
 *  the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *  
 *  SPDX-License-Identifier: BSD-2-Clause
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
  s << key;
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
