#ifndef I3GCDFILESERVICEFACTORY_H
#define I3GCDFILESERVICEFACTORY_H
/*
 * class: I3TextFileGeometryServiceFactory
 *
 * Version $Id$
 *
 * Date: 17 Feb 2004
 *
 * (c) IceCube Collaboration
 */

class I3Context;

#include "icetray/I3ServiceFactory.h"
class I3GCDFileGeometryService;
class I3GCDFileCalibrationService;
class I3GCDFileDetectorStatusService;

class I3GCDFileServiceFactory
: public I3ServiceFactory
{
 public:

  I3GCDFileServiceFactory(const I3Context& context);

  virtual ~I3GCDFileServiceFactory();

  virtual bool InstallService(I3Context& services);

  virtual void Configure();

 private:

  boost::shared_ptr<I3GCDFileGeometryService> geometry_;
  boost::shared_ptr<I3GCDFileCalibrationService> calibration_;
  boost::shared_ptr<I3GCDFileDetectorStatusService> status_;

  std::string filename_;

  I3GCDFileServiceFactory
    (const I3GCDFileServiceFactory& rhs); // stop default
  I3GCDFileServiceFactory operator=
    (const I3GCDFileServiceFactory& rhs); // stop default


   SET_LOGGER("I3GCDFileServiceFactory");
};

#endif 
