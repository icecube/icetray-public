#include "phys-services/geo-selector/I3StringAdderService.h"
#include "icetray/I3Units.h"
#include "dataclasses/geometry/I3OMGeo.h"
#include "icetray/I3TrayHeaders.h"
#include "phys-services/geo-selector/GeoSelUtils.h"
#include <boost/foreach.hpp>

I3GeometryConstPtr I3StringAdderService::GetGeometry(I3Time time)
{
  //Get the default geometry from wherever
  I3GeometryConstPtr old_geo = geo_service_->GetGeometry(time);

  //Just copy over the old geometry
  I3GeometryPtr new_geo = I3GeometryPtr(new I3Geometry(*old_geo));
  
  int strng(81);
  BOOST_FOREACH(I3OMGeo& omgeo,omGeoList_){

    unsigned int omNumber(0);
    double z(NAN);
    for(omNumber = 1, z = omgeo.position.GetZ(); 
	omNumber <= domsPerString_; 
	omNumber++, z -= domSpacing_){

      OMKey om(strng,omNumber);
      I3OMGeo g(omgeo);
      g.position.SetZ(z);

      new_geo->omgeo[om] = g;
    }
    strng++;
  }

  return new_geo;
}

