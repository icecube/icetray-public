/**
 *  $Id$
 *  
 *  Copyright (C) 2007
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
#include <dataclasses/TankKey.h>
#include <dataclasses/I3Vector.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

static unsigned
hash_tankkey(const TankKey &key)
{
  return 10*key.string + key.tank;
}

static std::string repr(const TankKey &key)
{
  std::stringstream s;
  s << "TankKey(" << key.string << ",TankKey.";
  switch (key.tank) {
  case TankKey::TankA: s << "TankA";
    break;
  case TankKey::TankB: s << "TankB";
    break;
  }
  s << ")";
  return s.str();
}

void register_TankKey()
{
  scope tankkey_scope = 
    class_<TankKey, boost::shared_ptr<TankKey> >("TankKey")
    .def(init<int,TankKey::TankID>())
    .def(init<OMKey>())
    .def(dataclass_suite<TankKey>())
    .def_readwrite("string", &TankKey::string)
    .def_readwrite("tank", &TankKey::tank)
    .add_property("default_omkey", &TankKey::GetDefaultOMKey)
    .def("__repr__", &repr)
    .def("__hash__", &hash_tankkey)
    .def(self == self)
    .def(self < self)
    ;
  enum_<TankKey::TankID>("TankID")
    .value("TankA", TankKey::TankA)
    .value("TankB", TankKey::TankB)
    .export_values()
    ;

  class_<I3VectorTankKey, bases<I3FrameObject>, boost::shared_ptr<I3VectorTankKey> >("I3VectorTankKey")
    .def(dataclass_suite<I3VectorTankKey >())
    ;
  register_pointer_conversions<I3VectorTankKey>();
}
    
