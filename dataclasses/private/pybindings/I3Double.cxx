//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id$
//
//   This file is part of IceTray.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//   
//   THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
//   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
//   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
//   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
//   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//   SUCH DAMAGE.
//   
//   SPDX-License-Identifier: BSD-2-Clause
//   
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
