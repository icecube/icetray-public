/**
 *
 * copyright  (C) 2004
 * the icecube collaboration
 * @version n$Id$
 * @file I3Geometry.h
 * @date $Date$
 */

#ifndef I3GEOMETRY_H_INCLUDED
#define I3GEOMETRY_H_INCLUDED

#include <vector>
#include "dataclasses/Utility.h"
#include "dataclasses/geometry/I3TankGeo.h"
#include "dataclasses/geometry/I3OMGeo.h"
#include "dataclasses/I3Time.h"
#include <icetray/I3DefaultName.h>
#include "icetray/I3FrameObject.h"
#include "dataclasses/I3Map.h"
/*
  The following is a flattened version of the Geometry Class for dcv2
  The 8 classes of old are now 3 structs.  

  (1) I3Geometry contains the 'master' data, namely the std::map of OMKey to OM 
  (for inice, icetop, and amanda), as well as the std::map of the stations.

  (2) I3OMGeo contains all the necessary information for a single OM, including
  an enum to specify what type of OM it is.

  (3) I3TankGeo which represents the 2 tanks within an icetop station.

  The major change is that there are no longer InIce and IceTop objects that
  can be called from the geometry class.  Instead, one should call the
  I3OMGeoMap object directly and use this.  Hence, a change would look like:
  old: I3InIceGeometry inice = Geometry.GetInIceGeometry();
  new: I3OMGeoMap geomap = Geometry.omgeo;

  The other changes were minor (simply eliminating the Get/Set functions in
  favor of direct setting) 

  -bchristy
*/
static const unsigned i3geometry_version_ = 1;

class I3Geometry : public I3FrameObject
{
public:
  I3Geometry(){};
  ~I3Geometry();

  //Map of all OMs based on their OMKey
  I3OMGeoMap omgeo;

  //Map of all the stations. 
  //Each int specifies a StationGeo object, which is a std::vector of 2 TankGeo's.
  I3StationGeoMap stationgeo;

  I3Time startTime;
  I3Time endTime;

  const I3Geometry& operator=(const I3Geometry& geometry);
  
  bool operator==(const I3Geometry& rhs)
  {
    return (omgeo == rhs.omgeo &&
            stationgeo == rhs.stationgeo &&
            startTime == rhs.startTime &&
            endTime == rhs.endTime);
  }
  bool operator!=(const I3Geometry& rhs)
  {
    return !operator==(rhs);
  }

  friend class icecube::serialization::access;

  template <class Archive> void serialize(Archive & ar, unsigned version);
};

I3_DEFAULT_NAME(I3Geometry);
I3_POINTER_TYPEDEFS(I3Geometry);
I3_CLASS_VERSION(I3Geometry, i3geometry_version_);

#endif // I3GEOMETRY_H_INCLUDED
