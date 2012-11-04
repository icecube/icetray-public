//
//   Copyright (c) 2004, 2005, 2006, 2007, 2010   Troy D. Straszheim  
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

#include <icetray/I3Tray.h>
#include <icetray/I3TrayInfo.h>
#include <icetray/I3TrayInfoService.h>
#include <icetray/python/boost_serializable_pickle_suite.hpp>

using namespace boost::python;

dict host_info(const I3TrayInfo& ti)
{
  dict d;
  for (std::map<std::string, std::string>::const_iterator iter = ti.host_info.begin();
       iter != ti.host_info.end();
       iter++)
    d[iter->first] = iter->second;
  return d;
}

static std::string I3TrayInfoRepr(I3TrayInfo &tray) {
  std::stringstream str;
  str << "tray = I3Tray.I3Tray()\n";
  BOOST_FOREACH(std::string service, tray.factories_in_order) {
    if (service == "__config") // Skip internal tray info service
      continue;
    const I3Configuration &config = *tray.factory_configs[service];
    str << "tray.AddService(";
    str << "'" << config.ClassName() << "', ";
    str << "'" << config.InstanceName() << "'";
    BOOST_FOREACH(std::string param, config.keys()) {
      std::string repr = boost::python::extract<std::string>(config.Get(param).attr("__repr__")());
      str << ", " << param << "=" << repr;
    }
    str << ")\n";
  }
  BOOST_FOREACH(std::string mod, tray.modules_in_order) {
    const I3Configuration &config = *tray.module_configs[mod];
    str << "tray.AddModule(";
    if (config.ClassName().find(".") != config.ClassName().npos ||
     config.ClassName().find(" ") != config.ClassName().npos)
      str << config.ClassName() << ", ";
    else
      str << "'" << config.ClassName() << "', ";
    str << "'" << config.InstanceName() << "'";
    BOOST_FOREACH(std::string param, config.keys()) {
      boost::python::object obj = config.Get(param);
      std::string repr = boost::python::extract<std::string>(config.Get(param).attr("__repr__")());
      str << ", " << param << "=" << repr;
    }
    str << ")\n";
  }

  return str.str();
}

void register_I3TrayInfo()
{
  class_<I3TrayInfo, bases<I3FrameObject>, boost::shared_ptr<I3TrayInfo> >("I3TrayInfo")
    .add_property("modules_in_order", &I3TrayInfo::modules_in_order)
    .add_property("factories_in_order", &I3TrayInfo::factories_in_order)
    .add_property("module_configs", &I3TrayInfo::module_configs)
    .add_property("factory_configs", &I3TrayInfo::factory_configs)
    .add_property("svn_url", &I3TrayInfo::svn_url)
    .add_property("svn_externals", &I3TrayInfo::svn_externals)
    .add_property("svn_revision", &I3TrayInfo::svn_revision)
    .add_property("host_info", &host_info)
//    .property("factories_in_order", &I3TrayInfo::factories_in_order)
//    .property("loaded_libs", &I3TrayInfo::loaded_libs)
    .def(self_ns::str(self))
    .def("__repr__", &I3TrayInfoRepr)
    .def_pickle(boost_serializable_pickle_suite<I3TrayInfo>())
    ;

  register_pointer_conversions<I3TrayInfo>();

  class_<I3TrayInfoService, boost::shared_ptr<I3TrayInfoService> >("I3TrayInfoService", init<const I3Tray *>())
    .add_property("config", &I3TrayInfoService::GetConfig)
    ;
}
