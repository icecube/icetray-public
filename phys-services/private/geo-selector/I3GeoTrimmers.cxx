/**
 *  copyright  (C) 2007
 *  the icecube collaboration
 *  $Id$
 *
 *  @file
 *  @version $Revision$
 *  @date $Date$
 *  @author David Boersma <boersma@icecube.wisc.edu>
 */

// for std::pair
#include <utility>
#include <vector>
#include <map>

#include "phys-services/geo-selector/I3GeoTrimmers.h"
#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/I3Vector.h"
#include "icetray/OMKey.h"
#include "dataclasses/physics/I3RecoPulse.h"
#include "dataclasses/physics/I3DOMLaunch.h"
#include "dataclasses/physics/I3Waveform.h"
#include "dataclasses/physics/I3AMANDAAnalogReadout.h"
#include "Miniball.h"

I3OMGeoMapPtr I3GeoTrimmers::GetMinBallGeometry( const I3OMGeoMap &input_geo,
                                                 const I3OMGeoMap &subgeo,
                                                 double margin ){


  // storing lowest and highest 
  std::map<int,int> topOMs;
  std::map<int,int> botOMs;
  std::map<int,int>::iterator itop;
  std::map<int,int>::iterator ibot;
  I3OMGeoMap::const_iterator igeo;
  int Nused = 0;
  for ( igeo = subgeo.begin(); igeo != subgeo.end(); igeo++ ){
    int istr = igeo->first.GetString();
    int iom = igeo->first.GetOM();
    if ( (itop = topOMs.find(istr)) == topOMs.end() ){
      topOMs[istr] = iom;
      botOMs[istr] = iom;
      ++Nused;
    } else {
      ibot = botOMs.find(istr);
      if ( itop->second == ibot->second ){
        ++Nused;
      }
      if ( itop->second < iom ){
        itop->second = iom;
      }
      if ( ibot->second > iom ){
        ibot->second = iom;
      }
    }
  }
  log_debug( "input geo with %zu entries, subgeo with %zu entries",
             input_geo.size(), subgeo.size() );
  log_debug( "%d DOMs are used to find the minimum ball", Nused );

  // Use Bernd Gaertner's minimum ball code.
  // Fill the ball with the positions of the highest and lowest hit
  // DOMs on each string.
  miniball::Miniball<3> ballmaker;
  miniball::Point<3> p;
  for ( itop = topOMs.begin(); itop != topOMs.end(); ++itop ){
    int str = itop->first;
    ibot = botOMs.find(itop->first);
    int top = itop->second;
    int bot = ibot->second;
    if ( bot == top ){
      const I3Position &pos = subgeo.find(OMKey(str,top))->second.position;
      p[0] = pos.GetX();
      p[1] = pos.GetY();
      p[2] = pos.GetZ();
      ballmaker.check_in(p);
    } else {
      const I3Position &tpos = subgeo.find(OMKey(str,top))->second.position;
      p[0] = tpos.GetX();
      p[1] = tpos.GetY();
      p[2] = tpos.GetZ();
      ballmaker.check_in(p);
      const I3Position &bpos = subgeo.find(OMKey(str,bot))->second.position;
      p[0] = bpos.GetX();
      p[1] = bpos.GetY();
      p[2] = bpos.GetZ();
      ballmaker.check_in(p);
    }
  }
  log_debug("going to search minimum ball...");
  ballmaker.build();
  log_debug("done!");
  miniball::Point<3> center = ballmaker.center();
  double radius2 = ballmaker.squared_radius();
  double radius = sqrt(radius2) + margin;
  log_debug( "Center=(%.1f,%.1f,%.1f), r=%.3f (margin=%.3f)",
             center[0], center[1], center[2], radius, margin );
  if ( margin > 0 ){
    radius2 = radius * radius;
  }

  log_debug("now generating ball geo");
  I3OMGeoMapPtr ballmap( new I3OMGeoMap );
  for ( igeo = input_geo.begin(); igeo != input_geo.end(); ++igeo ){
    const I3Position &pos = igeo->second.position;
    double dx = pos.GetX() - center[0];
    double dy = pos.GetY() - center[1];
    double dz = pos.GetZ() - center[2];
    if ( dx*dx + dy*dy + dz*dz <= radius2 ){
      ballmap->insert(std::make_pair(igeo->first,igeo->second));
    }
  }
  log_debug("ball geo has %zu entries", ballmap->size() );

  return ballmap;
}

/// Utility function 
void
I3GeoTrimmers::AddMeToTheMap(OMKey mykey, I3OMGeo me, I3OMGeoMapPtr themap){
  // is it already in the map?
  if (themap->find(mykey)!=themap->end()) log_debug("It's already in there.");
  else 
    (*themap)[mykey]=me;
}

/// XYZ


/// Create a reduced geometry containing only the hit OMs.
/// Currently use OMs which appear as keys in pulseseries_; maybe it should
/// be made possible to use DOMLaunchSeries or AMANDAAnalogReadout
/// maps instead...

template <class omdatamap>
I3OMGeoMapPtr I3GeoTrimmers::GeoFromEventData(const I3OMGeoMap &input_geo,
                                              const omdatamap &datamap ){
  // The output
  I3OMGeoMapPtr output_geoptr=I3OMGeoMapPtr(new I3OMGeoMap);

  // The loop
  typename omdatamap::const_iterator ipulse;
  for (ipulse=datamap.begin(); ipulse!=datamap.end(); ipulse++) {
    I3OMGeoMap::const_iterator i_omgeo=input_geo.find(ipulse->first);
    if (i_omgeo==input_geo.end())
      log_warn("OM %d/%d found in pulse series, but not in geometry!",
		ipulse->first.GetString(), ipulse->first.GetOM());
    else 
      //(*output_geoptr)[ipulse->first]=i_omgeo->second;
      AddMeToTheMap(ipulse->first,i_omgeo->second,output_geoptr);
  }
  return output_geoptr;
}

I3OMGeoMapPtr I3GeoTrimmers::GeoFromPulseSeries(const I3OMGeoMap &input_geo,
                                 const I3RecoPulseSeriesMap &psm) {
    return I3GeoTrimmers::GeoFromEventData(input_geo,psm);
}
I3OMGeoMapPtr I3GeoTrimmers::GeoFromDOMLaunchSeries(const I3OMGeoMap &input_geo,
                                     const I3DOMLaunchSeriesMap &dlsm) {
    return I3GeoTrimmers::GeoFromEventData(input_geo,dlsm);
}
I3OMGeoMapPtr I3GeoTrimmers::GeoFromWFSeries(const I3OMGeoMap &input_geo,
                              const I3WaveformSeriesMap &wfsm) {
    return I3GeoTrimmers::GeoFromEventData(input_geo,wfsm);
}
I3OMGeoMapPtr I3GeoTrimmers::GeoFromAMANDAAnalogReadout(const I3OMGeoMap &input_geo,
                              const I3AMANDAAnalogReadoutMap &aarm) {
    return I3GeoTrimmers::GeoFromEventData(input_geo,aarm);
}

/// Create a reduced geometry containing only the hit OMs.
/// Currently use OMs which appear as keys in pulseseries_; maybe it should
/// be made possible to use DOMLaunchSeries or AMANDAAnalogReadout
/// maps instead...
I3OMGeoMapPtr
I3GeoTrimmers::GeoFromPulseSeriesWithNeighbors(I3OMGeoMap input_geo,
                                               I3RecoPulseSeriesMap psm) {

  // The output
  I3OMGeoMapPtr output_geoptr=I3OMGeoMapPtr(new I3OMGeoMap);

  // The loop
  I3RecoPulseSeriesMap::const_iterator ipulse;
  for (ipulse=psm.begin(); ipulse!=psm.end(); ipulse++) {
    I3OMGeoMap::const_iterator i_omgeo=input_geo.find(ipulse->first);
    if (i_omgeo==input_geo.end())
      log_warn("OM %d/%d found in pulse series, but not in geometry!",
		ipulse->first.GetString(), ipulse->first.GetOM());
    else {
        I3GeoTrimmers::AddMeToTheMap(ipulse->first,i_omgeo->second,output_geoptr);

      // Find its neighbors...
      int istring = i_omgeo->first.GetString();
      int iom = i_omgeo->first.GetOM();
      // Neighbor up:
      I3OMGeoMap::const_iterator found_up=input_geo.find(OMKey(istring,iom-1));
      if ((found_up!=input_geo.end()) &&
	  (found_up->second.omtype == i_omgeo->second.omtype))
	I3GeoTrimmers::AddMeToTheMap(found_up->first,found_up->second,output_geoptr);
      // Neighbor down:
      I3OMGeoMap::const_iterator found_down=input_geo.find(OMKey(istring,iom+1));
      if ((found_down!=input_geo.end()) &&
	  (found_down->second.omtype == i_omgeo->second.omtype))
	I3GeoTrimmers::AddMeToTheMap(found_down->first,found_down->second,output_geoptr);
    }
  }
  return output_geoptr;
  }

/// Create a reduced geometry in which Bad OM's have been removed.
I3OMGeoMapPtr I3GeoTrimmers::GeoWithoutBadOMs(I3OMGeoMap input_geo, I3VectorOMKey badomlist) {

  // The output
  I3OMGeoMapPtr output_geoptr=I3OMGeoMapPtr(new I3OMGeoMap(input_geo));

  // Remove Bad OM's from the duplicate geometry which will be used.
  I3VectorOMKey::iterator ib;
  for(ib = badomlist.begin(); ib != badomlist.end(); ib++) {
    if (output_geoptr->find(*ib) != output_geoptr->end())
      output_geoptr->erase(*ib);
    else 
      log_warn("Your bad OM %d %d is not in the geometry", 
	       ib->GetString(), ib->GetOM());
  }
  return output_geoptr;
}

/// Create a reduced geometry in which Bad OM's have been removed.
I3OMGeoMapPtr I3GeoTrimmers::GeoWithoutBadOMs(I3OMGeoMap input_geo, std::vector<OMKey> badomlist) {
  
  // The output
  I3OMGeoMapPtr output_geoptr=I3OMGeoMapPtr(new I3OMGeoMap(input_geo));
  
  // Remove Bad OM's from the duplicate geometry which will be used.
  std::vector<OMKey>::iterator ib;
  for(ib = badomlist.begin(); ib != badomlist.end(); ib++) {
    if (output_geoptr->find(*ib) != output_geoptr->end())
      output_geoptr->erase(*ib);
    else 
      log_info("Your bad OM %d %d is not in the geometry", 
               ib->GetString(), ib->GetOM());
  }
  return output_geoptr;
}

/// Create a reduced geometry in which all IceTop DOM's have been removed.
I3OMGeoMapPtr I3GeoTrimmers::GeoWithoutIceTop(I3OMGeoMap input_geo) {

  I3VectorOMKey badicetop;  
  for (int istr=1; istr<=80; istr++)
    for (int iom=61; iom<=64; iom++)
      if (input_geo.find(OMKey(istr,iom)) != input_geo.end())
	badicetop.push_back(OMKey(istr,iom));
  

  return I3GeoTrimmers::GeoWithoutBadOMs(input_geo, badicetop);
}

/// Create a reduced geometry in which only IceTop DOM's are included.
I3OMGeoMapPtr I3GeoTrimmers::GeoIceTopOnly(I3OMGeoMap input_geo) {

  I3RecoPulseSeriesMap goodicetop;
  I3RecoPulse junkpulse;
  for (int istr=1; istr<=80; istr++)
    for (int iom=61; iom<=64; iom++)
      if (input_geo.find(OMKey(istr,iom)) != input_geo.end())
	goodicetop[OMKey(istr,iom)].push_back(junkpulse);

  return I3GeoTrimmers::GeoFromPulseSeries(input_geo, goodicetop);
}


/////////////////// DUPLICATE VERSIONS which are for I3Geometry
/////////////////// rather than I3OMGeoMap
I3GeometryPtr I3GeoTrimmers::GeoFromPulseSeries(I3Geometry input_geo, I3RecoPulseSeriesMap psm) {
  I3GeometryPtr output_geoptr=I3GeometryPtr(new I3Geometry(input_geo));
  output_geoptr->omgeo = *(I3GeoTrimmers::GeoFromPulseSeries(input_geo.omgeo, psm));
  return output_geoptr;
}
I3GeometryPtr I3GeoTrimmers::GeoFromPulseSeriesWithNeighbors(I3Geometry input_geo, I3RecoPulseSeriesMap psm) {
  I3GeometryPtr output_geoptr=I3GeometryPtr(new I3Geometry(input_geo));
  output_geoptr->omgeo = *(I3GeoTrimmers::GeoFromPulseSeriesWithNeighbors(input_geo.omgeo, psm));
  return output_geoptr;
}
I3GeometryPtr I3GeoTrimmers::GeoWithoutBadOMs(I3Geometry input_geo, I3VectorOMKey badomlist) {
  I3GeometryPtr output_geoptr=I3GeometryPtr(new I3Geometry(input_geo));
  output_geoptr->omgeo = *(I3GeoTrimmers::GeoWithoutBadOMs(input_geo.omgeo, badomlist));
  return output_geoptr;
}
I3GeometryPtr I3GeoTrimmers::GeoWithoutIceTop(I3Geometry input_geo) {
  I3GeometryPtr output_geoptr=I3GeometryPtr(new I3Geometry(input_geo));
  output_geoptr->omgeo = *(I3GeoTrimmers::GeoWithoutIceTop(input_geo.omgeo));
  return output_geoptr;
}
I3GeometryPtr I3GeoTrimmers::GeoIceTopOnly(I3Geometry input_geo) {
  I3GeometryPtr output_geoptr=I3GeometryPtr(new I3Geometry(input_geo));
  output_geoptr->omgeo = *(I3GeoTrimmers::GeoIceTopOnly(input_geo.omgeo));
  return output_geoptr;
}

template <class omdatamap>
I3OMGeoMapPtr
I3GeoTrimmers::GetMinBallGeometryFromData( const I3OMGeoMap &input_geo,
                                           const omdatamap &datamap,
                                           double margin ){

  // get geomap of hit DOMs
  I3OMGeoMap geohit;
  typename omdatamap::const_iterator ipulse;
  for ( ipulse = datamap.begin(); ipulse != datamap.end(); ipulse++ ){
    I3OMGeoMap::const_iterator igeo = input_geo.find(ipulse->first);
    if ( igeo == input_geo.end() ){
      log_warn( "OM %d/%d found in pulse series, but not in geometry!",
      ipulse->first.GetString(), ipulse->first.GetOM());
    } else {
      geohit.insert( std::make_pair(igeo->first,igeo->second));
    }
  }

  // get geomap of all DOMs in a minimum ball around the hits
  return I3GeoTrimmers::GetMinBallGeometry( input_geo, geohit, margin );
}

// instantiate for various event info types
template I3OMGeoMapPtr I3GeoTrimmers::GetMinBallGeometryFromData<I3RecoPulseSeriesMap>(
        const I3OMGeoMap &input_geo,
        const I3RecoPulseSeriesMap &pulsemap,
        double margin );
template I3OMGeoMapPtr I3GeoTrimmers::GetMinBallGeometryFromData<I3DOMLaunchSeriesMap>(
        const I3OMGeoMap &input_geo,
        const I3DOMLaunchSeriesMap &dlsm,
        double margin );
template I3OMGeoMapPtr I3GeoTrimmers::GetMinBallGeometryFromData<I3WaveformSeriesMap>(
        const I3OMGeoMap &input_geo,
        const I3WaveformSeriesMap &wfsm,
        double margin );
template I3OMGeoMapPtr I3GeoTrimmers::GetMinBallGeometryFromData<I3AMANDAAnalogReadoutMap>(
        const I3OMGeoMap &input_geo,
        const I3AMANDAAnalogReadoutMap &aarm,
        double margin );
