/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */
#include <icetray/serialization.h>
#include <icetray/I3Logging.h>

#include <icetray/I3Tray.h>
#include <icetray/I3TrayInfoService.h>
#include <icetray/I3TrayInfo.h>
#include <icetray/I3Factory.h>

#include <sys/types.h>
#include <limits.h>
#include <unistd.h>
#include <pwd.h>
#include <time.h>

#ifdef I3_USE_ROOT
#include <RVersion.h>
#endif

template<class T>
static
std::map<std::string,I3ConfigurationPtr>
get_configs(const T& from)
{
  std::map<std::string, I3ConfigurationPtr> result;
  for (typename T::const_iterator iter = from.begin(); iter != from.end();
   iter++) {
     I3ConfigurationPtr ptr(new I3Configuration(iter->second->GetConfiguration()));
     result[iter->first] = ptr;
  }
 
  return result;
}

I3TrayInfo
I3TrayInfoService::GetConfig()
{ 
  I3TrayInfo the_config;
  //
  // load host/build info
  //
  the_config.host_info["gcc_version"] = __VERSION__;
#ifdef I3_USE_ROOT
  the_config.host_info["root_version"] = ROOT_RELEASE;
#else
  the_config.host_info["root_version"] = "Compiled w/o ROOT support";
#endif

  the_config.svn_externals = SVN_EXTERNALS;
  the_config.svn_url = SVN_URL;
  the_config.svn_revision = SVN_REVISION;

  const static unsigned hostname_max = 255;
  char hostname_tmp[hostname_max];
  gethostname(hostname_tmp, hostname_max);
  the_config.host_info["hostname"] = hostname_tmp;

  uid_t uid = getuid();

  struct passwd *pass = getpwuid(uid);
  the_config.host_info["username"] = pass->pw_name;

  time_t the_time;
  time(&the_time);
  the_config.host_info["start_time"] = ctime(&the_time);

  the_config.host_info["platform"] = BOOST_PP_STRINGIZE(I3_PLATFORM);

#if 0
  //
  //  svn info from the module factory
  //
  for (I3ModuleFactory::const_iterator iter = I3ModuleFactory::Instance().begin();
       iter != I3ModuleFactory::Instance().end();
       iter++)
    {
      the_config.svn_info[iter->first] = make_pair(iter->second.url, iter->second.revision);
    }

  //
  //  svn info for the servicefactory factory
  //
  for (I3ServiceFactoryFactory::const_iterator iter = 
	 I3ServiceFactoryFactory::Instance().begin();
       iter != I3ServiceFactoryFactory::Instance().end();
       iter++)
    {
      if (the_config.svn_info.count(iter->first))
	log_error("Name collision recording svn info between module %s and service factory %s",
		  iter->first.c_str(), iter->first.c_str());
      the_config.svn_info[iter->first] = make_pair(iter->second.url, iter->second.revision);
    }
#endif

  // and then the modules/factories and their configurations.  So nice
  // to have classes with full value semantics.
  the_config.modules_in_order = tray_.modules_in_order;
  the_config.factories_in_order = tray_.factories_in_order;
	
  the_config.module_configs = get_configs(tray_.modules);
  the_config.factory_configs = get_configs(tray_.factories);

  return the_config;
} 

