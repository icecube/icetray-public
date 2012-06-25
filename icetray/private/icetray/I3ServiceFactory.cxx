/*
 * class: I3ServiceFactory
 *
 * Version $Id: I3ServiceFactory.cxx 51710 2009-01-06 19:23:39Z troy $
 *
 * Date: 29 Sept 2003
 *
 * (c) IceCube Collaboration
 */

#include "icetray/I3ServiceFactory.h"

#include <cstring>

#include <icetray/I3TrayInfo.h>
#include "icetray/I3Module.h"

I3ServiceFactory::I3ServiceFactory(const I3Context& context)
  :context_(context)
{  
}

I3ServiceFactory::~I3ServiceFactory()
{
}

bool 
I3ServiceFactory::InitializeService(I3Context& services)
{
  // get parameters (if any) first
  // the parameters will be needed in installing the service object
  return InstallService(services);
}


void 
I3ServiceFactory::Configure() 
{
}

void
I3ServiceFactory::Reconfigure() 
{
}

void 
I3ServiceFactory::Finish()
{
  log_trace("%s", __PRETTY_FUNCTION__);
}

void 
I3ServiceFactory::Fatal(const std::string& message,
			int status) const 
{
}

