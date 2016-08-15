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

#include <icetray/I3Configuration.h>
#include <boost/make_shared.hpp>
#include <icetray/python/boost_serializable_pickle_suite.hpp>

using namespace boost::python;

struct I3ConfigurationDescriptionProxy {
	I3ConfigurationConstPtr target;
	I3ConfigurationDescriptionProxy(I3ConfigurationConstPtr config) : target(config) {};
	std::string Get(const std::string &name) {
		return target->GetDescription(name);
	}
};

static I3ConfigurationDescriptionProxy make_proxy(I3ConfigurationPtr config) {
	return I3ConfigurationDescriptionProxy(config);
}

void register_I3Configuration()
{

  class_<std::map<std::string, I3ConfigurationPtr> >
    ("map_string_I3Configuration")
    // the 'true' here is to turn off proxying
    .def(map_indexing_suite<std::map<std::string, I3ConfigurationPtr>, true>())
    ;

  scope outer = 
  class_<I3Configuration, I3ConfigurationPtr> ("I3Configuration")
    .def("keys", &I3Configuration::keys)
    .def("__setitem__", &I3Configuration::Set)
    .def("__getitem__", 
	 (object (I3Configuration::*)(const std::string&) const)
	 &I3Configuration::Get)
    .add_property("descriptions", &make_proxy)
    .add_property("ClassName", 
		  (std::string (I3Configuration::*)() const) &I3Configuration::ClassName,
		  (void (I3Configuration::*)(const std::string&)) &I3Configuration::ClassName)
    .add_property("InstanceName", 
		  (std::string (I3Configuration::*)() const) &I3Configuration::InstanceName)
    .def_pickle(boost_serializable_pickle_suite<I3Configuration>())
    ;


  class_<I3ConfigurationDescriptionProxy>("DescriptionProxy",
    init<I3ConfigurationConstPtr>())
    .def("__getitem__", &I3ConfigurationDescriptionProxy::Get)
    ;

}
