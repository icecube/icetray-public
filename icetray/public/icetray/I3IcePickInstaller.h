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

#include <icetray/I3ServiceFactory.h>
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
class I3IcePickInstaller : public I3ServiceFactory
{
 public:

  /**
   * @brief The constructer.  The context is then passed to the 
   * constructer of the I3IcePick object being installed
   *
   * @param servicename_ is the name for where the I3IcePick should
   * be installed in the context
   */
  I3IcePickInstaller(const I3Context& context) :
    I3ServiceFactory(context),
    servicename_()
    {
      log_trace("Creating I3IcePickService");
      AddParameter("ServiceName",
		   "Name that the service will have in the context."
		   "This is typically redundant... by default, it uses "
		   "The name of the installer",
		   servicename_);
      
      log_trace("Need to create a new logic object");
      pick_ = shared_ptr<IcePick>(new IcePick(context));
    };

  /**
   * @brief Where the I3IcePick is actually put into the frame
   */
  bool InstallService(I3Context& context)
    {
      if(pick_)
	{
	  context.Put<I3IcePick>(pick_,servicename_);
	  return true;
	}
      else
	{
	  log_fatal("The logic object has not been initialized.");
	  return false;
	}
    };

  /**
   * @brief The configure method where servicename_ is set from the 
   * steering file.
   */
  void Configure()
    {
      GetParameter("ServiceName",servicename_);
      if (servicename_.empty())
	servicename_ = GetName();
      log_debug("Using servicename=%s", servicename_.c_str());
      pick_->ConfigureInterface();
    };

 private:

  shared_ptr<IcePick> pick_;
  std::string servicename_;

  SET_LOGGER("I3IcePickInstaller");
};

#endif
