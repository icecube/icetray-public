// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <icetray/serialization.h>
#include <dataclasses/geometry/I3Geometry.h>

#include <boost/foreach.hpp>

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/lexical_cast.hpp>

I3Geometry::~I3Geometry() {}

template <class Archive>
void
I3Geometry::serialize(Archive& ar, unsigned version) {
  if (version > i3geometry_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3Geometry class.", version, i3geometry_version_);

  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));

  if (version == 0) {
    // these are now I3Maps and I3Vectors, not std::maps and std::vectors
    typedef std::map<OMKey, I3OMGeo> I3OMGeoMap_old;
    typedef std::map<int, std::vector<I3TankGeo> > I3StationGeoMap_old;

    I3OMGeoMap_old omgeo__;
    I3StationGeoMap_old stationgeo__;

    ar & make_nvp("OMGeo", omgeo__);
    ar & make_nvp("StationGeo", stationgeo__);

    omgeo.clear();
    BOOST_FOREACH(const I3OMGeoMap_old::value_type & pair, omgeo__) {
      omgeo.insert(pair);
    }

    stationgeo.clear();
    BOOST_FOREACH(const I3StationGeoMap_old::value_type & pair, stationgeo__) {
      stationgeo.insert(std::make_pair(pair.first,
                                       I3Vector<I3TankGeo>(pair.second.begin(), pair.second.end())
                                      ));
    }
  }

  if (version >= 1) {
    ar & make_nvp("OMGeo", omgeo);
    ar & make_nvp("StationGeo", stationgeo);
  }

  if (version >= 2) {
    ar & make_nvp("ScintGeo", scintgeo);
    ar & make_nvp("AntennaGeo", antennageo);
    ar & make_nvp("IceActGeo", iceactgeo);
  }
  if (version >= 3) {
    ar & make_nvp("SnowHeightProvenance", snowHeightProvenance);
  }

  ar & make_nvp("StartTime", startTime);
  ar & make_nvp("EndTime", endTime);
}

const I3Geometry& I3Geometry::operator=(const I3Geometry& geometry) {
  omgeo = geometry.omgeo;
  stationgeo = geometry.stationgeo;
  scintgeo = geometry.scintgeo;
  antennageo = geometry.antennageo;
  iceactgeo = geometry.iceactgeo;
  startTime = geometry.startTime;
  endTime = geometry.endTime;
  snowHeightProvenance = geometry.snowHeightProvenance;

  return *this;
}

// Function to grab the correct I3TankGeo from the I3StationGeo, regardless of
// its ordering (that is, even if the two tanks are in the non-conventional order).
// (This function was copied and modified from topeventcleaning/I3TopHLCClusterCleaning.cxx)
I3TankGeo I3Geometry::GetTankGeo(const OMKey &key) const
{
  I3StationGeoMap::const_iterator station_geo = stationgeo.find(key.GetString());
  if(station_geo==stationgeo.end()) {
    log_fatal("Station %d doesn't exist in StationGeoMap!", key.GetString());
  }
  BOOST_FOREACH(const I3TankGeo &tank_geo, station_geo->second) {
    if (std::find(tank_geo.omKeyList_.begin(), tank_geo.omKeyList_.end(), key) != tank_geo.omKeyList_.end())
      return tank_geo;
  }
  log_fatal_stream("No TankGeo found for DOM " << key);
}
// A similar version, but for TankKeys
// Use the "Default OMKey" to determine it, which is DOM 61 for TankA and 63 for TankB
I3TankGeo I3Geometry::GetTankGeo(const TankKey &tankkey) const
{
  return GetTankGeo(tankkey.GetDefaultOMKey());
}

// For converting SnowHeightProvenance enum's to strings and back again:
// I stole/adapted this from I3Particle.  (Note sure if there's a better way.)
#define MAKE_ENUM_TO_STRING_CASE_LINE(r, data, t) case I3Geometry::t : return BOOST_PP_STRINGIZE(t);
std::string I3Geometry::GetSnowHeightProvenanceString() const
{
    switch (snowHeightProvenance) {
        BOOST_PP_SEQ_FOR_EACH(MAKE_ENUM_TO_STRING_CASE_LINE, ~, I3GEOMETRY_H_I3Geometry_SnowHeightProvenance)
    }
    return(boost::lexical_cast<std::string>( snowHeightProvenance ));
}

#define MAKE_STRING_TO_ENUM_IF_LINE2(r, data, t) else if ( str == BOOST_PP_STRINGIZE(t) ) { data = t; }
void I3Geometry::SetSnowHeightProvenanceString(const std::string &str)
{
  if (false) { }
  BOOST_PP_SEQ_FOR_EACH(MAKE_STRING_TO_ENUM_IF_LINE2, snowHeightProvenance, I3GEOMETRY_H_I3Geometry_SnowHeightProvenance)
  else {
    try {
      snowHeightProvenance = static_cast<SnowHeightProvenance>( boost::lexical_cast<int>(str) );
    } catch(boost::bad_lexical_cast &bad) {
      log_fatal("\"%s\" is not a valid SnowHeightProvenance.", str.c_str());
    }
  }
}

I3_SERIALIZABLE(I3Geometry);
