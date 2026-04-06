/**
 * Copyright  (C) 2004 the icecube collaboration
 * SPDX-License-Identifier: BSD-2-Clause
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


static const unsigned i3omgeo_version_ = 2;


/**
 * List the names of enumeration members defined in this file
 * here. These can be used for e.g. pybindings, which require
 * the names of the enumeration members to be known. This list
 * should be updated whenever members or new enums are added to
 * the class.
 */
#define I3OMGEO_H_I3OMGeo_OMType        \
  (UnknownType)(AMANDA)(IceCube)(IceTop)(mDOM)(Scintillator)(IceAct)\
  (PDOM)(isoPDOM)(DEgg)(WOM)(FOM)(DMIce)(LOM)(LOM16)(LOM18)(RadioReceiver) \
  (POCAM)(PencilBeam)(RadioEmitter)(AcousticEmitter)(AbaloneHub)(FibreComm)

#define I3OMGEO_H_I3OMGeo_PMTType        \
  (Unknown)(DOM_Standard)(DOM_HQE)(DEgg)(mDOM)(LOM_Hamamatsu)(LOM_NNVT)

//Simple struct to contain all pertinent OM info.
//See I3Geometry.h for more info

class I3OMGeo
{
public:
    enum OMType {
      UnknownType = 0,
      AMANDA = 10,
      IceCube = 20,
      IceTop = 30,
      Scintillator = 40,
      IceAct = 50,
      // OMType > 100 are Gen2 R&D sensors
      PDOM = 110,
      isoPDOM = 111,
      DEgg = 120,
      mDOM = 130,
      WOM = 140,
      FOM = 150,
      DMIce = 160,
      LOM = 170,
      LOM16 = 171,
      LOM18 = 172,
      RadioReceiver = 180,
      // OMType > 200 for the various calibration sources
      POCAM = 200,
      PencilBeam = 210,
      RadioEmitter = 220,
      AcousticEmitter = 230,
      AbaloneHub = 240,
      FibreComm = 250,
    };

    enum class PMTType {
      Unknown = 0,        // default
      DOM_Standard = 1,   // Hamamatsu R7081-02
      DOM_HQE = 2,        // Hamamatsu R7081-02MOD
      DEgg = 10,          // Hamamatsu R5912-100
      mDOM = 20,          // Hamamatsu R15458-02
      LOM_Hamamatsu = 30, // Hamamatsu R16293-01-Y001
      LOM_NNVT = 31,      // NNVT N2041
    };
  
    I3OMGeo() : omtype(UnknownType), pmttype(PMTType::Unknown) {}

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
     * Nominal or HQE types
     */
    PMTType pmttype;

    /**
     * Effective collection area (use I3Units)
     */
    double area;

    /**
     * Gets the I3Direction from the I3Orientation
     */
    inline I3Direction GetDirection() const {return orientation.GetDir();}

    /**
     * Gets the beta for angular sensitivity
     */
    double GetPMTBeta() const;

    /**
     * Gets the directionally averaged area
     */
    double GetAverageArea() const;

    /**
     * Gets approximate surface area of the photocathode
     */
    double GetCurvedArea() const;
  
    bool operator==(const I3OMGeo& rhs) const
    {
      return (position == rhs.position &&
              orientation == rhs.orientation &&
              omtype == rhs.omtype &&
              pmttype == rhs.pmttype &&
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

std::string to_string(const I3OMGeo::PMTType&);
std::string to_string(const I3OMGeo::OMType&);

std::ostream& operator<<(std::ostream&, const I3OMGeo::OMType&);
std::ostream& operator<<(std::ostream&, const I3OMGeo::PMTType&);
std::ostream& operator<<(std::ostream&, const I3OMGeo&);

I3_POINTER_TYPEDEFS(I3OMGeo);
I3_CLASS_VERSION(I3OMGeo, i3omgeo_version_);

typedef I3Map<OMKey, I3OMGeo> I3OMGeoMap;
I3_POINTER_TYPEDEFS(I3OMGeoMap);

#endif //I3OMGEO_H_INCLUDED
