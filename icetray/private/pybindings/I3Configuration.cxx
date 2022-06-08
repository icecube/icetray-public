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

#include <map>
#include <vector>

#include <icetray/I3Configuration.h>
#include <boost/make_shared.hpp>
#include <icetray/python/boost_serializable_pickle_suite.hpp>
#include <icetray/python/std_map_indexing_suite.hpp>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

typedef std::map<std::string, I3ConfigurationPtr> I3ConfigurationMap;

std::ostream& operator<<(std::ostream& os, const I3ConfigurationMap& m) {
    os << "map_string_I3Configuration {\n";
    for(auto p : m) {
        os << "  " << p.first << " : " << *p.second << "\n";
    }
    os << "}";
    return os;
}

list configuration_values(I3ConfigurationPtr config) {
    list ret;
    for(auto k : config->keys()) {
        ret.append(config->Get(k));
    }
    return ret;
}

size_t configuration_len(I3ConfigurationPtr config) {
    return config->keys().size();
}

static object configuration_iter(I3ConfigurationPtr config) {
    list keys;
    for(auto k : config->keys()) {
        keys.append(k);
    }
    handle<> key_iter(PyObject_GetIter(keys.ptr()));
    return object(key_iter);
}

static dict make_proxy(I3ConfigurationPtr config) {
	dict ret;
	for(auto k : config->keys()) {
	    ret[k] = config->GetDescription(k);
	}
	return ret;
}

void register_I3Configuration()
{

  class_<I3ConfigurationMap >
    ("map_string_I3Configuration")
    .def(copy_suite<I3ConfigurationMap >())
    // the 'true' here is to turn off proxying, and also why we don't use dataclass_suite
    .def(std_map_indexing_suite<I3ConfigurationMap, true>())
    .def("__str__", &stream_to_string<I3ConfigurationMap >)
    .def(operator_suite<I3ConfigurationMap >())
    .def(freeze())
    ;

  class_<I3Configuration, I3ConfigurationPtr> ("I3Configuration")
    .def("keys", &I3Configuration::keys)
    .def("values", configuration_values)
    .def("__setitem__", &I3Configuration::Set)
    .def("__getitem__",
       (object (I3Configuration::*)(const std::string&) const)
       &I3Configuration::Get)
    .def("__contains__", &I3Configuration::Has)
    .def("__iter__", configuration_iter)
    .def("__len__", configuration_len)
    .add_property("descriptions", &make_proxy)
    .add_property("ClassName", 
        (std::string (I3Configuration::*)() const) &I3Configuration::ClassName,
        (void (I3Configuration::*)(const std::string&)) &I3Configuration::ClassName)
    .add_property("InstanceName", 
        (std::string (I3Configuration::*)() const) &I3Configuration::InstanceName)
    .def(copy_suite<I3Configuration>())
    .def_pickle(boost_serializable_pickle_suite<I3Configuration>())
    .def("__str__", &stream_to_string<I3Configuration>)
    .def(operator_suite<I3Configuration>())
    .def(freeze())
    ;

  boost::python::implicitly_convertible<I3ConfigurationPtr, I3ConfigurationConstPtr >();
}
