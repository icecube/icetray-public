/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *  
 *  SPDX-License-Identifier: BSD-2-Clause
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
  the_config.host_info["boost_version"] = BOOST_LIB_VERSION;

  the_config.svn_externals = "";
  the_config.svn_url = "";
  the_config.svn_revision = 0;

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

  the_config.host_info["platform"] = I3_PLATFORM;

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

