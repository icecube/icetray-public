//
//   Copyright (c) 2004, 2005, 2006, 2007, 2008   Troy D. Straszheim  
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

#include <dataclasses/I3Map.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/ostream_overloads.hpp>

using namespace boost::python;

void register_I3MapStringStdMapStringInt()
{
  //
  //  TDS: big problems here.  the duplicate registration of pair<string,int>
  //  with the boost.python conversion registry is a warn when compiled 'release',
  //  and triggers an ugly assert in registry.cpp when compiled 'debug',
  //  due to the fact that we link against different versions of libboost_python[-d].so
  //
  //  AO (1/19/12) : This was "fixed" several years ago (2009-11-20).  When building the
  //  tools a patch is applied which manually comments out this specific assert.
  //  This could be a problem again when/if we switch to system build and/or
  //  giving people the option to build against whatever boost they want.
  //  In the meantime, people simply need to rebuild their tools.
  

  class_<std::map<std::string,int > >("std_map_string_int",no_init)
    .def(dataclass_suite<std::map<std::string,int > >())
    ;
}
