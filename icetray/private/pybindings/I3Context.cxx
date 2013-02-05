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
    .def("__len__", &context_len)
    .def("__contains__", (bool (I3Context::*)(const std::string &) const)&I3Context::Has)
    .def("__getitem__", (object (I3Context::*)(const std::string &) const)&I3Context::Get)
    .def("__setitem__", (bool (I3Context::*)(const std::string &, object))&I3Context::Put)
    .def("__iter__", boost::python::range(&I3Context::begin, &I3Context::end))
    ;
}
