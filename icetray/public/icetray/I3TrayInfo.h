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
#ifndef ICETRAY_I3TRAYINFO_H_INCLUDED
#define ICETRAY_I3TRAYINFO_H_INCLUDED

#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include <icetray/I3Configuration.h>
#include <icetray/I3FrameObject.h>

struct I3TrayInfo : public I3FrameObject
{
  std::map<std::string, std::string> host_info;

  std::string svn_url, svn_externals;
  unsigned svn_revision;

  std::vector<std::string> modules_in_order;
  std::vector<std::string> factories_in_order;

  std::map<std::string, I3ConfigurationPtr> module_configs;
  std::map<std::string, I3ConfigurationPtr> factory_configs;
	
  template <class Archive>
  void 
  serialize (Archive & ar, unsigned version);

  I3TrayInfo();

};

std::ostream& 
operator<<(std::ostream& os, const I3TrayInfo& config);

I3_DEFAULT_NAME(I3TrayInfo);
I3_POINTER_TYPEDEFS(I3TrayInfo);
I3_CLASS_VERSION(I3TrayInfo, 2);
#endif


