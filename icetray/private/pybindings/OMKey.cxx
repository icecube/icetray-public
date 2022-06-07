/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
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
