#ifndef I3TEXTFILEGEOMETRYSERVICEFACTORY_H
#define I3TEXTFILEGEOMETRYSERVICEFACTORY_H
/*
 * class: I3TextFileGeometryServiceFactory
 *
 * Version $Id$
 *
 * Date: 17 Feb 2004
 *
 * (c) IceCube Collaboration
 */

// Header files


// forward declarations

class I3Context;

#include "icetray/I3ServiceFactory.h"
class I3TextFileGeometryService;


class I3TextFileGeometryServiceFactory
: public I3ServiceFactory
{
 public:

  I3TextFileGeometryServiceFactory(const I3Context& context);

  virtual ~I3TextFileGeometryServiceFactory();

  virtual bool InstallService(I3Context& services);

  virtual void Configure();

 private:

  boost::shared_ptr<I3TextFileGeometryService> geometry_;

  std::string amandaFile_;
  std::string icecubeFile_;

  I3TextFileGeometryServiceFactory
    (const I3TextFileGeometryServiceFactory& rhs); // stop default
  I3TextFileGeometryServiceFactory operator=
    (const I3TextFileGeometryServiceFactory& rhs); // stop default


   SET_LOGGER("I3TextFileGeometryServiceFactory");
};

#endif 
