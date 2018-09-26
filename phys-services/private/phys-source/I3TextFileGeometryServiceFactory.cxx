#include "phys-services/source/I3TextFileGeometryServiceFactory.h"
#include "phys-services/source/I3TextFileGeometryService.h"

I3_SERVICE_FACTORY(I3TextFileGeometryServiceFactory);

I3TextFileGeometryServiceFactory::
I3TextFileGeometryServiceFactory(const I3Context& context) : 
  I3ServiceFactory(context)
{
  AddParameter("AmandaGeoFile","",amandaFile_);
  AddParameter("IceCubeGeoFile","",icecubeFile_);
}

I3TextFileGeometryServiceFactory::
~I3TextFileGeometryServiceFactory()
{

}

void I3TextFileGeometryServiceFactory::Configure()
{
  GetParameter("AmandaGeoFile",amandaFile_);
  GetParameter("IceCubeGeoFile",icecubeFile_);
}

bool I3TextFileGeometryServiceFactory::InstallService(I3Context& services)
{
  if(!geometry_)
    geometry_ = 
      boost::shared_ptr<I3TextFileGeometryService>
      (new I3TextFileGeometryService(amandaFile_,
				    icecubeFile_));

  return services.Put<I3GeometryService>(geometry_);
}

