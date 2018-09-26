/**
 * copyright  (C) 2004
 * the icecube collaboration
 * @version $Id$
 * @file I3TankGeo.h
 * @date $Date$
 */

#ifndef I3TANKGEO_H_INCLUDED
#define I3TANKGEO_H_INCLUDED

#include <iostream>

#include "dataclasses/I3Position.h"
#include "dataclasses/Utility.h"
#include "icetray/OMKey.h"
#include "dataclasses/I3Vector.h"
#include "dataclasses/I3Map.h"
#include "dataclasses/external/CompareFloatingPoint.h"


static const unsigned i3tankgeo_version_ = 2;

/*Simple struct that contains all the pertinent Tank info.
  Note that there is no OM information for this struct. 
  This is what led me to comment out the cryptic switch statement in
  I3TextFileGeometryService and I3FileGeoOrigin (in phys-services) since
  it appeared to be assigning OMGeoPtr info to a TankGeo object.
  (in the old version, TankGeo inherited from I3SurfModuleGeo, hence it
  had OMGeo info associated with it)
  See I3Geometry.h for more info
*/


struct I3TankGeo 
{
  enum TankType {NotSet = 0, Tyvek_Lined = 1, Zirconium_Lined = 2};

  I3TankGeo():
    orientation(NAN),
    tankradius(NAN),
    tankheight(NAN),
    fillheight(NAN),
    snowheight(NAN),
    tanktype(NotSet)
  {};
  
  ~I3TankGeo();

  I3Position position; //tank x,y,z position
  double orientation; //relative angular rotation of tank
  double tankradius; //tank radius (I3Units!)
  double tankheight; //tank height (I3Units!)
  double fillheight; //water/ice level height (I3Units!)
  I3Vector<OMKey> omKeyList_;
  double snowheight; // snow "overburden" (use I3Units)
  TankType tanktype; // Type of tank.
  
  bool operator==(const I3TankGeo& rhs) const
  {
    return (position == rhs.position &&
            CompareFloatingPoint::Compare_NanEqual(orientation, rhs.orientation) &&
            CompareFloatingPoint::Compare_NanEqual(tankradius, rhs.tankradius) &&
            CompareFloatingPoint::Compare_NanEqual(tankheight, rhs.tankheight) &&
            CompareFloatingPoint::Compare_NanEqual(fillheight, rhs.fillheight) &&
            CompareFloatingPoint::Compare_NanEqual(snowheight, rhs.snowheight) &&
            tanktype == rhs.tanktype &&
            omKeyList_ == rhs.omKeyList_);
  }
  bool operator!=(const I3TankGeo& rhs)
  {
    return !operator==(rhs);
  }

  friend class icecube::serialization::access;

  template <class Archive> void serialize(Archive & ar, unsigned version);

  std::ostream& Print(std::ostream&) const;
};

std::ostream& operator<<(std::ostream&, const I3TankGeo&);

I3_CLASS_VERSION(I3TankGeo, i3tankgeo_version_);
I3_POINTER_TYPEDEFS(I3TankGeo);

typedef I3Vector<I3TankGeo> I3StationGeo;
I3_POINTER_TYPEDEFS(I3StationGeo);

typedef I3Map<int, I3StationGeo> I3StationGeoMap;
I3_POINTER_TYPEDEFS(I3StationGeoMap);

#endif //I3TANKGEO_H_INCLUDED
