#include "phys-services/source/I3GCDFileServiceFactory.h"
#include "phys-services/source/I3GCDFileService.h"

I3_SERVICE_FACTORY(I3GCDFileServiceFactory);

I3GCDFileServiceFactory::
I3GCDFileServiceFactory(const I3Context& context) : 
  I3ServiceFactory(context)
{
  AddParameter("GCDFileName","", filename_);
}

I3GCDFileServiceFactory::
~I3GCDFileServiceFactory()
{

}

void I3GCDFileServiceFactory::Configure()
{
  GetParameter("GCDFileName", filename_);
}

bool I3GCDFileServiceFactory::InstallService(I3Context& services)
{
  if(!geometry_)
    geometry_ = 
      boost::shared_ptr<I3GCDFileGeometryService>
      (new I3GCDFileGeometryService(filename_) );

  if(!calibration_)
    calibration_ = 
      boost::shared_ptr<I3GCDFileCalibrationService>
      (new I3GCDFileCalibrationService(filename_) );

  if(!status_)
    status_ = 
      boost::shared_ptr<I3GCDFileDetectorStatusService>
      (new I3GCDFileDetectorStatusService(filename_) );

  return (services.Put<I3GeometryService>(geometry_) &&
	  services.Put<I3CalibrationService>(calibration_) &&
	  services.Put<I3DetectorStatusService>(status_) );
}

