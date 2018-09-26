/**
 *  $Id$
 *  
 *  Copyright (C) 2007
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
    
