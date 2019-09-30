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

#include <dataclasses/I3Double.h>
#include <icetray/python/boost_serializable_pickle_suite.hpp>
#include <icetray/python/operator_suite.hpp>

using namespace boost::python;

static std::string 
i3double_prettyprint(const I3Double& d)
{
  std::ostringstream oss;
  oss << "I3Double(" << d.value << ")";
  return oss.str();
}

bool i3double_bool(const I3Double& d)
{
    return d.value != 0.0;
}

void register_I3Double()
{
  class_<I3Double, bases<I3FrameObject>, boost::shared_ptr<I3Double> >("I3Double",
    "A serializable float/double. Can compare directly with numeric types.\n\
Note that python assignment is by reference, creating two links to one object.")
    .def(init<>())
    .def(init<double>())
    .def(init<const I3Double&>())
    .def_readwrite("value", &I3Double::value)
    .def("__repr__",i3double_prettyprint)
    .def_pickle(boost_serializable_pickle_suite<I3Double>())
    .def(operator_suite<I3Double>())
    .def(operator_int_suite<I3Double>())
    .def(operator_float_suite<I3Double>())
    .def("__nonzero__", i3double_bool)
    .def("__bool__", i3double_bool)    
    .def( freeze() )
    ;

  register_pointer_conversions<I3Double>();
}
