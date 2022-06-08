//
//   Copyright (c) 2004, 2005, 2006, 2007, 2008   Troy D. Straszheim  
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

#ifndef I3VECTORS_H_PYBINDINGS
#define I3VECTORS_H_PYBINDINGS 

#include <icetray/ostream_pair.hpp>
#include <dataclasses/I3Vector.h>
#include <dataclasses/ostream_overloads.hpp>
#include <icetray/python/dataclass_suite.hpp>
#include <vector>
#include <sstream>

using namespace boost::python;

using std::vector;

template <typename T>
void 
register_i3vector_of(const std::string& s)
{

  class_<std::vector<T> >((std::string("List") + s).c_str())
    .def(dataclass_suite<std::vector<T> >())
    ;

  typedef I3Vector<T> vec_t;
  class_<vec_t, bases<I3FrameObject, std::vector<T> >, boost::shared_ptr<vec_t> > 
    ((std::string("I3Vector") + s).c_str())
    .def(dataclass_suite<vec_t>())
    ;
  register_pointer_conversions<vec_t>();
}

template <typename T, typename U>
std::pair< T, U >
py_make_pair( T t, U u)
{
    return std::make_pair(t,u);
}

template <typename T, typename U>
void
register_std_pair(const char* s)
{
  typedef std::pair<T, U> type_t;

  class_<type_t>(s)
    .def_readwrite("first", &type_t::first)
    .def_readwrite("second", &type_t::second)
    .def(dataclass_suite<type_t>())
    ;
  def("make_pair", &py_make_pair<T, U>);
}

#endif //I3VECTORS_H_PYBINDINGS
