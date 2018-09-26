#ifndef I3GSLRANDOMSERVICEFACTORY_H
#define I3GSLRANDOMSERVICEFACTORY_H
/*
 * class: I3GSLRandomServiceFactory
 *
 * Version $Id$
 *
 * Date: 17 Feb 2004
 *
 * (c) IceCube Collaboration
 */

// Header files

#include <string>

// Forward declarations

class I3Context;

// Superclasses

#include "icetray/I3ServiceFactory.h"
#include "phys-services/I3RandomService.h"

/**
 * @brief This class installs a I3GSLRandomService.
 *
 * I3GSLRandomService supports two parameters: <VAR>Seed</VAR>,
 * <VAR>InstallServiceAs</VAR>.
 * @version $Id$
 * @author pretz
 */
class I3GSLRandomServiceFactory
: public I3ServiceFactory
{
 public:

  // Constructors and destructor

  I3GSLRandomServiceFactory(const I3Context& context);

  virtual ~I3GSLRandomServiceFactory();

  // public member functions

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

  I3GSLRandomServiceFactory
    (const I3GSLRandomServiceFactory& rhs); // stop default
  I3GSLRandomServiceFactory operator=
    (const I3GSLRandomServiceFactory& rhs); // stop default

  // instance member data
  unsigned long seed_;
  bool track_state_;
  I3RandomServicePtr random_;
  std::string installServiceAs_;

  SET_LOGGER("I3GSLRandomServiceFactory");
};

#endif
