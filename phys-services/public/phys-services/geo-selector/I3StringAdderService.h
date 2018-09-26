/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3StringAdderService.h
 * @version $Revision$
 * @date $Date$
 * @author olivas
 */

#ifndef I3STRINGADDER_H
#define I3STRINGADDER_H

#include <vector>
#include <string>
#include <fstream>
#include <boost/shared_ptr.hpp>
#include <interfaces/I3GeometryService.h>
#include "icetray/I3Context.h"

class I3OMGeo;


/**
 * @brief A I3GeometryOrigin which reads the default geometry and serves up a reduced geometry.
 */
class I3StringAdderService : public I3GeometryService
{

  SET_LOGGER("I3StringAdderService");

  double domSpacing_;

  unsigned int domsPerString_;

  std::vector<I3OMGeo> omGeoList_;

  I3GeometryServicePtr geo_service_;

 public:
  I3StringAdderService(I3GeometryServicePtr g){
    geo_service_ = g;
  }

  virtual ~I3StringAdderService(){}

  I3GeometryConstPtr GetGeometry(I3Time time);

  void SetDOMSpacing(double s){ domSpacing_ = s; }

  double GetDOMSpacing(){ return domSpacing_; }

  void SetNDOMsPerString(unsigned int n){ domsPerString_ = n; };

  unsigned int GetNDOMsPerString(){ return domsPerString_; }

  void SetOMGeoList(std::vector<I3OMGeo>& v){ omGeoList_ = v; }

  const std::vector<I3OMGeo>& GetOMGeoList() const { return omGeoList_; }
};

I3_POINTER_TYPEDEFS(I3StringAdderService);

#endif
