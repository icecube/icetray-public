//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id: ithon.cxx 25598 2006-11-25 02:52:57Z troy $
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

#include <icetray/I3Factory.h>
#include <boost/foreach.hpp>

using namespace boost::python;

I3ModulePtr create_module(const std::string& name, I3Context& context)
{
  return I3::Singleton<I3ModuleFactory>::get_const_instance()
    .Create(name)(context);
}

/* A potemkin version, just to put its default config in the context. */
void
instantiate_module(const std::string& name, I3Context& context)
{
	try {
		I3::Singleton<I3ModuleFactory>::get_const_instance()
		    .Create(name)(context);
	} catch (std::runtime_error) {
		I3::Singleton<I3ServiceFactoryFactory>::get_const_instance()
		    .Create(name)(context);
	}
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
  def("create_module", &create_module);
  def("_instantiate_module", &instantiate_module);
  def("modules", &get_registrations<I3ModuleFactory>, args("project"));
  def("services", &get_registrations<I3ServiceFactoryFactory>, args("project"));
  def("projects", &get_projects);
}
