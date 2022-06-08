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

#include <icetray/I3Context.h>
#include <boost/make_shared.hpp>

using namespace boost::python;

static
int context_len(I3ContextPtr context)
{
  return context->keys().size();
}

static
list context_items(I3ContextPtr context)
{
  std::vector<std::string> keys = context->keys();

  list t;
  BOOST_FOREACH(const std::string &key, keys) {
      boost::python::object obj = context->Get(key);
      t.append(boost::python::make_tuple(key, obj));
  }
  return t;
}

static
list context_values(I3ContextPtr context)
{
  std::vector<std::string> keys = context->keys();

  list t;
  BOOST_FOREACH(const std::string &key, keys) {
      boost::python::object obj = context->Get(key);
      t.append(obj);
  }
  return t;
}

void register_I3Context()
{
  class_<std::map<std::string, object> >("map_string_pyobject")
    .def(map_indexing_suite<std::map<std::string, object>, true>())
    ;

  class_<I3Context, I3ContextPtr, boost::noncopyable> ("I3Context")
    .def("keys", &I3Context::keys)
    .def("values", &context_values)
    .def("items", &context_items)
    .def("__str__", &I3Context::dump)
    .def("__len__", &context_len)
    .def("__contains__", (bool (I3Context::*)(const std::string &) const)&I3Context::Has)
    .def("__getitem__", (object (I3Context::*)(const std::string &) const)&I3Context::Get)
    .def("__setitem__", (bool (I3Context::*)(const std::string &, object))&I3Context::Put)
    .def("__iter__", boost::python::range(&I3Context::begin, &I3Context::end))
    ;
}
