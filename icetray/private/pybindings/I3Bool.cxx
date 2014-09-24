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

#include <icetray/I3Bool.h>
#include <icetray/python/boost_serializable_pickle_suite.hpp>
#include <icetray/python/operator_suite.hpp>

using namespace boost::python;

static std::string
i3bool_prettyprint(const I3Bool& b)
{
    if(b.value) return "I3Bool(True)";
    else        return "I3Bool(False)";
}

void register_I3Bool()
{
  class_<I3Bool, bases<I3FrameObject>, boost::shared_ptr<I3Bool> >("I3Bool",
    "A serializable bool. Can compare directly with bool types.\n\
Note that python assignment is by reference, creating two links to one object.")
    .def(init<>())
    .def(init<bool>())
    .def_readwrite("value", &I3Bool::value)
    .def("__repr__",i3bool_prettyprint)
    .def_pickle(boost_serializable_pickle_suite<I3Bool>())
    .def(operator_suite<I3Bool>())
    .def(operator_bool_suite<I3Bool>())
    ;

  register_pointer_conversions<I3Bool>();
}
