#include <vector>
#include "phys-services/geo-selector/I3StringAdderServiceFactory.h"
#include "phys-services/geo-selector/I3StringAdderService.h"
#include "dataclasses/geometry/I3OMGeo.h"
#include "icetray/I3Units.h"

I3_SERVICE_FACTORY(I3StringAdderServiceFactory);

const double DEFAULT_DEPTH(500. * I3Units::m);
const double DEFAULT_DOM_SPACING(17. * I3Units::m);
const unsigned int DEFAULT_DOMS_PER_STRING(60);
const int DEFAULT_ORIENTATION(-1);
const int DEFAULT_TYPE(20);
const double DEFAULT_AREA(0.0444 * I3Units::m2);
const double DEFAULT_REL_ANGLE(0.*I3Units::degree);

I3StringAdderServiceFactory::
I3StringAdderServiceFactory(const I3Context& context) : 
  I3ServiceFactory(context),
  depth_(DEFAULT_DEPTH),
  domSpacing_(DEFAULT_DOM_SPACING),
  domsPerString_(DEFAULT_DOMS_PER_STRING),
  domOrientation_(DEFAULT_ORIENTATION),
  domType_(DEFAULT_TYPE),
  area_(DEFAULT_AREA),
  aziangle_(DEFAULT_REL_ANGLE),
  geoSelectorName_("I3StringAdderServiceFactory"),
  geoServiceName_("I3GeometryService")
{
    AddParameter("GeoSelectorName",
		 "Name of the new geometry service. The muxer needs this name.",
		 geoSelectorName_);
    AddParameter("GeoServiceName",
		 "Name of the previous geometry service.",
		 geoServiceName_);
    AddParameter("XPositions", 
		 "List of x positions", 
		 xPositions_);
    AddParameter("YPositions", 
		 "List of y positions", 
		 yPositions_);
    AddParameter("Depth", 
		 "Z position of the top most layer of DOMs", 
		 depth_);
    AddParameter("DOMSpacing", 
		 "Distance between DOMs", 
		 domSpacing_);
    AddParameter("DOMsPerString", 
		 "Number of DOMs per String", 
		 domsPerString_);
    AddParameter("DOMOrientation", 
		 "Orientation of the DOM(Up = 1, Down = -1)", 
		 domOrientation_);
    AddParameter("DOMType", 
		 "Type (AMANDA = 10, IceCube = 20, IceTop = 30)", 
		 domType_);
    AddParameter("Area", 
		 "Effective Collection Area of the DOM", 
		 area_);
    AddParameter("AziAngle", 
		 "Relative rotation angle of DOM in azimuth", 
		 aziangle_);
}

I3StringAdderServiceFactory::
~I3StringAdderServiceFactory()
{

}

void I3StringAdderServiceFactory::Configure()
{
  GetParameter("GeoSelectorName",geoSelectorName_);
  GetParameter("GeoServiceName",geoServiceName_);
  GetParameter("XPositions", xPositions_);
  GetParameter("YPositions", yPositions_);
  GetParameter("Depth", depth_);
  GetParameter("DOMSpacing", domSpacing_);
  GetParameter("DOMsPerString", domsPerString_);
  GetParameter("DOMOrientation", domOrientation_);
  GetParameter("DOMType", domType_);
  GetParameter("Area", area_);
  GetParameter("AziAngle", aziangle_);	       

  if(xPositions_.size() != yPositions_.size())
    log_fatal("different list sizes for x(%zu) and y(%zu)",
	      xPositions_.size(),yPositions_.size());

  if(depth_ > 1450*I3Units::m ||
     depth_ < -1000 * I3Units::m)
    log_fatal("depth (%f) out of range [-1000m,+1450m]",depth_);	      

  /**
   * Fill the OMGeo list
   */
  I3OMGeo omgeo;
  omgeo.omtype = static_cast<I3OMGeo::OMType>(domType_);
  omgeo.orientation = ((domOrientation_==1)?I3Orientation(0.,0.,1., std::cos(aziangle_),std::sin(aziangle_),0.):I3Orientation(0.,0.,-1., std::cos(aziangle_),std::sin(aziangle_),0.));
  omgeo.area = area_;
  omgeo.position.SetZ(depth_);

  for(std::vector<double>::iterator x_iter = xPositions_.begin(), y_iter = yPositions_.begin();
      x_iter != xPositions_.end() && y_iter != yPositions_.end();
      x_iter++, y_iter++){
    I3OMGeo g(omgeo);
    g.position.SetX(*x_iter);
    g.position.SetY(*y_iter);
    omGeoList_.push_back(g);
  }
}

bool I3StringAdderServiceFactory::InstallService(I3Context& services)
{
  if(!geometry_)
    geometry_ = 
      boost::shared_ptr<I3StringAdderService>
      (new I3StringAdderService(context_.Get<I3GeometryServicePtr>(geoServiceName_)));

  boost::dynamic_pointer_cast<I3StringAdderService>(geometry_)->SetDOMSpacing(domSpacing_);
  boost::dynamic_pointer_cast<I3StringAdderService>(geometry_)->SetNDOMsPerString(domsPerString_);
  boost::dynamic_pointer_cast<I3StringAdderService>(geometry_)->SetOMGeoList(omGeoList_);

  return services.Put<I3GeometryService>(geoSelectorName_,geometry_);
}

