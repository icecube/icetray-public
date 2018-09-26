#ifndef I3SPRNGRANDOMSERVICEFACTORY_H
#define I3SPRNGRANDOMSERVICEFACTORY_H
/*
 * class: I3SPRNGRandomServiceFactory
 *
 * Date: 04/01/2005

 * @brief This class installs a I3SPRNGRandomService. 
 * Based on I3GSLRandomService by pretz
 *
 * @version $Id$
 * @author juancarlos
 *
 * (c) IceCube Collaboration
 */

#include <string>

class I3Context;

#include "icetray/I3ServiceFactory.h"
#include "phys-services/I3RandomService.h"

/**
 * @brief This class installs a I3SPRNGRandomService.
 *
 * I3SPRNGRandomService supports six parameter:
 * <VAR>Seed</VAR>, <VAR>NStreams</VAR>, <VAR>StreamNum</VAR>,
 * <VAR>inStateFile</VAR>, <VAR>outStateFile</VAR>,
 * <VAR>InstallServiceAs</VAR>.
 */
class I3SPRNGRandomServiceFactory
: public I3ServiceFactory
{
 public:

  I3SPRNGRandomServiceFactory(const I3Context& context);

  virtual ~I3SPRNGRandomServiceFactory();

  /**
   * Installed this objects service into the specified services object.
   *
   * @param services the I3Context into which the service should be installed.
   * @return true if the services is successfully installed.
   */
  virtual bool InstallService(I3Context& services);

  /**
   * Configure service prior to installing it. 
   */
  virtual void Configure();

 private:

  // private constructors, destructor and assignment

  I3SPRNGRandomServiceFactory
    (const I3SPRNGRandomServiceFactory& rhs); // stop default
  I3SPRNGRandomServiceFactory operator=
    (const I3SPRNGRandomServiceFactory& rhs); // stop default

  // instance member data
  int seed_, nstreams_, streamnum_;
  std::string instatefile_,outstatefile_;
  I3RandomServicePtr random_;
  std::string installServiceAs_;

  SET_LOGGER("I3SPRNGRandomServiceFactory");
  
};

#endif
