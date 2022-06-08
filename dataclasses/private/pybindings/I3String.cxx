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
