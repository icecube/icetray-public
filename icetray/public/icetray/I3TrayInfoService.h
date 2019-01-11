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
#ifndef ICETRAY_I3TRAYINFOSERVICEFACTORY_H_INCLUDED
#define ICETRAY_I3TRAYINFOSERVICEFACTORY_H_INCLUDED

#include <icetray/I3ServiceFactory.h>

class I3Tray;
class I3Context;

class I3TrayInfoServiceFactory : public I3ServiceFactory
{
  I3Tray *the_tray_;

public:
  I3TrayInfoServiceFactory(const I3Context& context);
	
  virtual ~I3TrayInfoServiceFactory();
  bool InstallService(I3Context& c);
  
};

class I3TrayInfoService
{
  const I3Tray& tray_;
	
public:
	
  I3TrayInfoService(const I3Tray& tray) : tray_(tray) { }

  I3TrayInfo GetConfig();

};

#endif
