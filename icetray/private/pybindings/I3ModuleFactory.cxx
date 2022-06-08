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

#include <icetray/I3Factory.h>
#include <icetray/I3ServiceFactory.h>
#include <boost/foreach.hpp>

using namespace boost::python;

I3ConfigurationPtr
module_default_config(const std::string& name)
{
	I3Context context;

	try {
		I3ModulePtr mod =
		    I3::Singleton<I3ModuleFactory>::get_const_instance()
		    .Create(name)(context);
		return I3ConfigurationPtr(new
		    I3Configuration(mod->GetConfiguration()));
	} catch (const std::runtime_error &) {
		I3ServiceFactoryPtr srv = 
		    I3::Singleton<I3ServiceFactoryFactory>::get_const_instance()
		    .Create(name)(context);
		return I3ConfigurationPtr(new
		    I3Configuration(srv->GetConfiguration()));
	}
	return I3ConfigurationPtr();
}

template <typename factory_t>
boost::python::list
get_registrations(const std::string &project)
{
	typedef typename factory_t::product_map_t::value_type value_t;
	
	boost::python::list module_names;
	const factory_t &factory = I3::Singleton<factory_t>::get_const_instance();
	BOOST_FOREACH(const value_t &info, factory) {
		if (info.second.project == project)
			module_names.append(info.first);
	}
	
	return module_names;
}

template <typename factory_t>
bool
contains(const std::string &name)
{
	const factory_t &factory = I3::Singleton<factory_t>::get_const_instance();
	return factory.find(name) != factory.end();
}

boost::python::list
get_projects()
{
	typedef I3ModuleFactory factory_t;
	typedef factory_t::product_map_t::value_type value_t;
	
	boost::python::list project_list;
	std::set<std::string> projects;
	
	const factory_t &factory = I3::Singleton<factory_t>::get_const_instance();
	BOOST_FOREACH(const value_t &info, factory)
		projects.insert(info.second.project);
	
	BOOST_FOREACH(const std::string &project, projects)
		project_list.append(project);
	
	return project_list;
}

void register_I3ModuleFactory()
{
  def("module_default_config", &module_default_config);
  def("modules", &get_registrations<I3ModuleFactory>, args("project"));
  def("services", &get_registrations<I3ServiceFactoryFactory>, args("project"));
  def("projects", &get_projects);
  def("is_module", &contains<I3ModuleFactory>);
  def("is_service", &contains<I3ServiceFactoryFactory>);
}
