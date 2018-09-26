/**
 * copyright  (C) 2004
 * the icecube collaboration
 * @version $Id$
 * @file I3OMGeo.h
 * @date $Date$
 */

#ifndef I3OMGEO_H_INCLUDED
#define I3OMGEO_H_INCLUDED

#include "dataclasses/I3Position.h"
#include "dataclasses/I3Direction.h"
#include "dataclasses/I3Orientation.h"
#include "dataclasses/I3Map.h"
#include <string>
#include <iostream>
#include <sstream>
#include "icetray/OMKey.h"
#include "dataclasses/Utility.h"


static const unsigned i3omgeo_version_ = 1;

//Simple struct to contain all pertinent OM info.  
//See I3Geometry.h for more info

class I3OMGeo 
{
public:
    enum OMType {UnknownType = 0, AMANDA = 10, IceCube = 20, IceTop = 30, Scintillator = 40, IceAct = 50,
                 // OMType > 100 are Gen2 R&D optical modules
                 PDOM = 110, DEgg = 120, mDOM = 130, WOM = 140, FOM = 150 };

    I3OMGeo():omtype(UnknownType){} 
    
    ~I3OMGeo();
    
    /**
     * the OM's (or PMT's) x,y,z position
     */
    I3Position position;
    
    /**
     * Orientation of the OM (or PMT)
     */ 
    I3Orientation orientation;

    /**
     * InIce? IceTop? AMANDA OM? (see enum above)
     */ 
    OMType omtype;

    /**
     * Effective collection area (use I3Units)
     */ 
    double area;

    /**
     * Gets the I3Direction from the I3Orientation
     */
    inline I3Direction GetDirection() const {return orientation.GetDir();}
    
    bool operator==(const I3OMGeo& rhs) const
    {
      return (position == rhs.position &&
              orientation == rhs.orientation &&
              omtype == rhs.omtype &&
              area == rhs.area);
    }
    bool operator!=(const I3OMGeo& rhs) const
    {
      return !operator==(rhs);
    }
  
    std::ostream& Print(std::ostream&) const;
    
private:
    friend class icecube::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, unsigned version);
};

std::ostream& operator<<(std::ostream&, const I3OMGeo&);

I3_POINTER_TYPEDEFS(I3OMGeo);
I3_CLASS_VERSION(I3OMGeo, i3omgeo_version_);

typedef I3Map<OMKey, I3OMGeo> I3OMGeoMap;
I3_POINTER_TYPEDEFS(I3OMGeoMap);

#endif //I3OMGEO_H_INCLUDED
