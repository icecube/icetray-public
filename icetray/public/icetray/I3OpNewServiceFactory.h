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
#ifndef ICETRAY_I3OPNEWSERVICEFACTORY_H_INCLUDED
#define ICETRAY_I3OPNEWSERVICEFACTORY_H_INCLUDED

#include <boost/shared_ptr.hpp>

#include <icetray/I3ServiceFactory.h>
#include <icetray/I3Context.h>

/**
 * Class template that creates/installs a service with plain old
 * operator new.
 */
template <class Service, class InstallAs = Service>
class I3OpNewServiceFactory : public I3ServiceFactory
{
 public:

  I3OpNewServiceFactory(const I3Context& context) 
    : I3ServiceFactory(context)
  { }

  virtual ~I3OpNewServiceFactory() { }

  /**
   * Installs this objects service into the specified services object.
   *
   * @param context the I3Context into which the service should be installed.
   * @return true if the services is successfully installed.
   */
  virtual 
  bool 
  InstallService(I3Context& context)
  {
    boost::shared_ptr<InstallAs> srv_p(new Service(context));
    return context.Put(srv_p);
  }

 private:

  I3OpNewServiceFactory(const I3OpNewServiceFactory& rhs); // stop default
  I3OpNewServiceFactory operator=(const I3OpNewServiceFactory& rhs); // stop default

};

#endif
