/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  Phil Roth <proth@icecube.umd.edu>
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
