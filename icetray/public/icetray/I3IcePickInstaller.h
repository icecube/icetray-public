/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  Phil Roth <proth@icecube.umd.edu>
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
#ifndef ICETRAY_I3ICEPICKINSTALLER_H_INCLUDED
#define ICETRAY_I3ICEPICKINSTALLER_H_INCLUDED

#include <icetray/I3SingleServiceFactory.h>
#include <icetray/I3IcePick.h>

/**
 * @brief This service factory is meant to install an I3IcePick 
 * object into the context.  The specific I3IcePick being 
 * installed is used as a template throughtout this class.
 *
 * If you've written a new I3IcePick, be sure to add the appropriate 
 * I3_SERVICE_FACTORY() macro to a .cxx file.
 */

template <class IcePick>
class I3IcePickInstaller : public I3SingleServiceFactory<IcePick, I3IcePick>
{
 public:

  /**
   * @brief The constructer.  The context is then passed to the 
   * constructer of the I3IcePick object being installed
   *
   * @param context the I3Context where the I3IcePick should
   * be installed
   */
  I3IcePickInstaller(const I3Context& context) :
    I3SingleServiceFactory<IcePick, I3IcePick>(context)
    {
      I3ServiceFactory::AddParameter("ServiceName",
		   "Name that the service will have in the context."
		   "This is typically redundant... by default, it uses "
		   "The name of the installer",
		   "");
    };

  /**
   * @brief The configure method where servicename_ is set from the 
   * steering file.
   */
  void Configure()
    {
      std::string servicename;
      I3ServiceFactory::GetParameter("ServiceName",servicename);
      if (!servicename.empty())
	I3ServiceFactory::SetName(servicename);
      I3SingleServiceFactory<IcePick, I3IcePick>::myService_->ConfigureInterface();
    };
};

#endif
