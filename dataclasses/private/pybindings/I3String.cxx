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

#include <vector>

#include <dataclasses/I3String.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/ostream_overloads.hpp>

using namespace boost::python;

size_t 
i3string_get_length(const I3String& s)
{
  return s.value.size();
}
bool
eq_string(const I3String& lhs, const std::string rhs)
{
  return lhs == rhs;
}
bool
ne_string(const I3String& lhs, const std::string rhs)
{
  return lhs != rhs;
}

void register_I3String()
{
  class_<I3String, bases<I3FrameObject>, boost::shared_ptr<I3String> >("I3String",
    "A serializable string. Can compare directly with strings.\n\
Note that python assignment is by reference, creating two links to one object.") 
    .def(init<>())
    .def(init<const std::string&>())
    .def(init<const I3String&>())
    .def_readwrite("value", &I3String::value)
    .def("__repr__",&stream_to_string<I3String>)
    .def("__len__",i3string_get_length)
    .def("__eq__",eq_string)
    .def("__ne__",ne_string)
    .def(dataclass_suite<I3String>())
    ;

  register_pointer_conversions<I3String>();
}
