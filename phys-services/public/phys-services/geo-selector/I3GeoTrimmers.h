#ifndef I3GEOTRIMMERS_H_INCLUDED
#define I3GEOTRIMMERS_H_INCLUDED

/// This file contains tools for manipulating Geometries, to take out various
/// bad/good/desired/notdesired OM's.

// Available functions:
//---------------------------------------
// GeoFromPulseSeries:   Include OM's only from a PulseSeries
// GeoFromPulseSeriesWithNeighbors:   Inlucde OM's only from a PulseSeries,
//      plus their local-coincidence neighbors (whether hit or not hit)
// GeoWithoutBadOMs:     Exclude a list of bad OM's
// GeoWithoutIceTop:     Exclude all IceTop OM's
// GeoIceTopOnly:        Exclude all InIce OM's
//---------------------------------------

#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/I3Vector.h"
#include "icetray/OMKey.h"
#include "dataclasses/physics/I3RecoPulse.h"
#include "dataclasses/physics/I3DOMLaunch.h"
#include "dataclasses/physics/I3Waveform.h"
#include "dataclasses/physics/I3AMANDAAnalogReadout.h"

// this #define helps to decide in flat-ntuple (and other user code) whether
// the geotrimmers can only be used for recopulses or just for practically all
// domdata-types.
#define NEW_GEOTRIMMERS 1

namespace I3GeoTrimmers
{

  /// Utility function 
  void AddMeToTheMap(OMKey mykey, I3OMGeo me, I3OMGeoMapPtr themap);


  /// Create a reduced geometry containing only the hit OMs.
  /// Currently use OMs which appear as keys in pulseseries_; maybe it should
  /// be made possible to use DOMLaunchSeries or AMANDAAnalogReadout
  /// maps instead...

  template <class omdatamap>
  I3OMGeoMapPtr GeoFromEventData(const I3OMGeoMap &input_geo,
                               const omdatamap &datamap );

  I3OMGeoMapPtr GeoFromPulseSeries(const I3OMGeoMap &input_geo,
                                   const I3RecoPulseSeriesMap &psm);
  I3OMGeoMapPtr GeoFromDOMLaunchSeries(const I3OMGeoMap &input_geo,
                                       const I3DOMLaunchSeriesMap &dlsm);
  I3OMGeoMapPtr GeoFromWFSeries(const I3OMGeoMap &input_geo,
                                const I3WaveformSeriesMap &wfsm);
  I3OMGeoMapPtr GeoFromAMANDAAnalogReadout(const I3OMGeoMap &input_geo,
                                           const I3AMANDAAnalogReadoutMap &aarm);

  /// Create a reduced geometry containing only the hit OMs.
  /// Currently use OMs which appear as keys in pulseseries_; maybe it should
  /// be made possible to use DOMLaunchSeries or AMANDAAnalogReadout
  /// maps instead...
  I3OMGeoMapPtr GeoFromPulseSeriesWithNeighbors(I3OMGeoMap input_geo, I3RecoPulseSeriesMap psm);

  /// Create a reduced geometry in which Bad OM's have been removed.
  I3OMGeoMapPtr GeoWithoutBadOMs(I3OMGeoMap input_geo, I3VectorOMKey badomlist);
  I3OMGeoMapPtr GeoWithoutBadOMs(I3OMGeoMap input_geo, std::vector<OMKey> badomlist);

  /// Create a reduced geometry in which all IceTop DOM's have been removed.
  I3OMGeoMapPtr GeoWithoutIceTop(I3OMGeoMap input_geo);

  /// Create a reduced geometry in which only IceTop DOM's are included.
  I3OMGeoMapPtr GeoIceTopOnly(I3OMGeoMap input_geo);


  /////////////////// DUPLICATE VERSIONS which are for I3Geometry
  /////////////////// rather than I3OMGeoMap
  I3GeometryPtr GeoFromPulseSeries(I3Geometry input_geo, I3RecoPulseSeriesMap psm);
  I3GeometryPtr GeoFromPulseSeriesWithNeighbors(I3Geometry input_geo, I3RecoPulseSeriesMap psm);
  I3GeometryPtr GeoWithoutBadOMs(I3Geometry input_geo, I3VectorOMKey badomlist);
  I3GeometryPtr GeoWithoutIceTop(I3Geometry input_geo);
  I3GeometryPtr GeoIceTopOnly(I3Geometry input_geo);

  /**
   * Get geomap of all DOMs in a minimum ball around a subgeometry.
   * The implementation of this method uses GPL code written by Bernd Gaertner
   * (not a member of IceCube).
   */
  I3OMGeoMapPtr GetMinBallGeometry( const I3OMGeoMap &input_geo,
                                    const I3OMGeoMap &subgeo,
                                    double margin = 0. );

  /**
   * Get geomap of all DOMs in a minimum ball around a subgeometry.
   * The subgeometry is obtained from data (DOMs which actually have data)
   * and then the GetMinBallGeometry method is used to obtain the geomap
   * of the minimum surrounding ball.
   */
  template <class omdatamap>
  I3OMGeoMapPtr GetMinBallGeometryFromData( const I3OMGeoMap &input_geo,
                                            const omdatamap &datamap,
                                            double margin = 0. );

} //end of namespace

#endif  // I3GEOTRIMMERS_H_INCLUDED
