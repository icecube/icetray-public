#include "phys-services/I3ScaleCalculator.h"
#include "phys-services/I3Cuts.h"

#include <boost/foreach.hpp>
#include <vector>
#include <map>
#include <set>
#include <limits>

#define Z_TOP 1950.

using namespace std;

I3ScaleCalculator::I3ScaleCalculator (I3GeometryConstPtr geo, 
                                      IceCubeConfig iceConf, 
                                      IceTopConfig topConf,
                                      std::vector<int> strings,
                                      std::vector<int> stations,
                                      int topDOMid, int bottomDOMid):
  geo_(geo), iceConf_(iceConf), topConf_(topConf),
  listOfBoundaryDeepStrings_(strings), listOfBoundarySurfaceStations_(stations),
  topDOMid_(topDOMid), bottomDOMid_(bottomDOMid){
 
  if (iceConf_ == IC_GUESS) {
    iceConf_ = GuessIceCubeConfig ();
  }
  if (topConf_ == IT_GUESS) {
    topConf_ = GuessIceTopConfig ();
  }
  
  if (iceConf_ == IC_UNKNOWN) {
    log_error("Unknown IceCube detector configuration");
  }
  if (topConf_ == IT_UNKNOWN) {
    log_error("Unknown IceTop detector configuration");
  }

  if (iceConf_ == IC_CUSTOM) {
    log_info("Using a custom list of strings");
    listOfBoundaryDeepStrings_ = strings;
  }
  if (topConf_ == IT_CUSTOM) {
    log_info("Using a custom list of stations");
    listOfBoundarySurfaceStations_ = stations;
  }
  if (iceConf_ == IC79) {
    log_error("This code has been updated... need to specify either _STRICT or _SMOOTH treatment of the northeast notch");
    log_error("The IC79 option (by itself) will be obsolete soon.");
    log_error("For now, we'll use IC79_SMOOTH.  See documentation for details");
    iceConf_ = IC86_SMOOTH;
  }
  if (topConf_ == IT73) {
    log_error("This code has been updated... need to specify either _STRICT or _SMOOTH treatment of the northeast notch");
    log_error("The IT73 option (by itself) will be obsolete soon.");
    log_error("For now, we'll use IT73_SMOOTH.  See documentation for details");
    topConf_ = IT81_SMOOTH;
  }
  if (iceConf_ == IC86) {
    log_error("This code has been updated... need to specify either _STRICT or _SMOOTH treatment of the northeast notch");
    log_error("The IC86 option (by itself) will be obsolete soon.");
    log_error("For now, we'll use IC86_SMOOTH.  See documentation for details");
    iceConf_ = IC86_SMOOTH;
  }
  if (topConf_ == IT81) {
    log_error("This code has been updated... need to specify either _STRICT or _SMOOTH treatment of the northeast notch");
    log_error("The IT81 option (by itself) will be obsolete soon.");
    log_error("For now, we'll use IT81_SMOOTH.  See documentation for details");
    topConf_ = IT81_SMOOTH;
  }
  // Some special cases: the DeepCore choices start at a different place than the top of the detector
  // (the bottom of DeepCore is still at bottomDOMid of 60, but the topDOMid is different)
  // Later, this code will loop through all the strings take the average z coordinate... but none of the "boundary"
  // strings in this code are DC strings, so it should be OK.
  if (iceConf_ == DEEPCORE_ALL) {
    topDOMid_ = 19;  // for a "normal" string, approximately at the height of the top of a deepcore string (~190 meters)
  }
  if (iceConf_ == DEEPCORE_BELOWDUST) {
    topDOMid_ = 40;  // for a "normal" string, approximately at the height of the top of the bit of deepcore below the dustlayer (~-160 meters)
  }

  log_debug("At the end of the constructor, we've got configs: %d / %d", iceConf_, topConf_);
  log_debug("And lists of strings:");
  BOOST_FOREACH(int s, listOfBoundaryDeepStrings_) { log_debug("%d", s); }
  log_debug("And lists of stations:");
  BOOST_FOREACH(int s, listOfBoundarySurfaceStations_) { log_debug("%d", s); }
  log_debug("And top/bottom DOM's are: %d / %d", topDOMid_, bottomDOMid_);

}

I3ScaleCalculator::IceCubeConfig 
I3ScaleCalculator::GuessIceCubeConfig () const {
  // get geo information
  I3OMGeoMap omMap = geo_->omgeo;

  // count strings
  std::set<int > stringSet;
  BOOST_FOREACH(I3OMGeoMap::value_type om, omMap){
    // avoid AMANDA/IceAct and other assignments to "station zero"
    if(om.first.GetString() > 0)
      stringSet.insert (om.first.GetString ());
  }

  // guess different configs
  int stringNo = stringSet.size ();
  if (stringNo < 79) {
    log_error("Found fewer than 79 strings in this Geometry.");
    log_fatal("Boundary guesses for IC-59 and previous configurations are retired from this code; you can still specfify them by hand, by using IC_CUSTOM and specifying the boundary strings yourself.");
  }
  else if (stringNo == 79){
    return IC79_SMOOTH;
  }
  else if (stringNo == 86){
    return IC86_SMOOTH;  // For now, make the old (smooth) one the default.
  }
  else {
    log_error("I encountered an unfamiliar number of strings in the I3Geometry: %d", stringNo);
    return IC_UNKNOWN;
  }
}

I3ScaleCalculator::IceTopConfig 
I3ScaleCalculator::GuessIceTopConfig () const {
  // get number of stations from StationGeo part of the I3Geometry
  I3StationGeoMap stationMap = geo_->stationgeo;
  
  // count stations
  int stationNo = stationMap.size();
  if (stationNo < 73) {
    log_error("Found fewer than 73 stations in this Geometry.");
    log_fatal("Boundary guesses for IT-59 and previous configurations are retired from this code; you can still specfify them by hand, by using IT_CUSTOM and specifying the boundary stations yourself.");
  }
  else if (stationNo == 73){
    return IT73_SMOOTH;
   }
  else if (stationNo == 81){
    return IT81_SMOOTH;  // For now, make the old (smooth) one the default.
  }
  else {
    log_error("I encountered an unfamiliar number of surface stations in the StationGeo: %d", stationNo);
    return IT_UNKNOWN;
  }
}

std::vector<int > I3ScaleCalculator::GetOuterStrings () const {
  std::vector<int > outerStrings;
  outerStrings.clear ();

  switch (iceConf_) {
  case IC_GUESS:
    log_error("Still guessing required, should not happen!");
    break;
  case IC_UNKNOWN:
    log_error("Unknown IceCube detector configuration");
    break;
  case IC_EMPTY:
    log_info("Leaving IceCube empty.");
    break;
  case IC_CUSTOM:
    log_info("Using a custom IceCube detector configuration");
    outerStrings = listOfBoundaryDeepStrings_;
    break;
  case IC79_SMOOTH:  // This one "smooths out the notch" by one station
    outerStrings.push_back(2);  
    outerStrings.push_back(6);
    outerStrings.push_back(50);
    outerStrings.push_back(74);
    outerStrings.push_back(73); // <--- here's where the smoothening is
    outerStrings.push_back(78);
    outerStrings.push_back(75);
    outerStrings.push_back(41);
    break;
  case IC79_STRICT:  // This one takes the "notch" more literally
    outerStrings.push_back(2);
    outerStrings.push_back(6);
    outerStrings.push_back(50);
    outerStrings.push_back(74);
    outerStrings.push_back(72); // <--- no smoothing on this one
    outerStrings.push_back(78);
    outerStrings.push_back(75);
    outerStrings.push_back(41);
    break;
  case IC86_SMOOTH:  // This one "smooths out the notch" by one station
    outerStrings.push_back(1);
    outerStrings.push_back(6);
    outerStrings.push_back(50);
    outerStrings.push_back(74);
    outerStrings.push_back(73); // <--- here's where the smoothening is
    outerStrings.push_back(78);
    outerStrings.push_back(75);
    outerStrings.push_back(31);
    break;
  case IC86_STRICT:  // This one takes the "notch" more literally
    outerStrings.push_back(1);
    outerStrings.push_back(6);
    outerStrings.push_back(50);
    outerStrings.push_back(74);
    outerStrings.push_back(72); // <--- no smoothing on this one
    outerStrings.push_back(78);
    outerStrings.push_back(75);
    outerStrings.push_back(31);
    break;
  case DEEPCORE_ALL:  // The hexagon boundary of "DeepCore Fiducial" from https://wiki.icecube.wisc.edu/index.php/DeepCore
    outerStrings.push_back(26);
    outerStrings.push_back(27);
    outerStrings.push_back(37);
    outerStrings.push_back(46);
    outerStrings.push_back(45);
    outerStrings.push_back(35);
    break;
  case DEEPCORE_BELOWDUST:  // strings are the same as for DEEPCORE_ALL... just the upper DOM number is different (later)
    outerStrings.push_back(26);
    outerStrings.push_back(27);
    outerStrings.push_back(37);
    outerStrings.push_back(46);
    outerStrings.push_back(45);
    outerStrings.push_back(35);
    break;
  default:
    log_fatal("Unknown configuration %d", iceConf_);
  }
  return outerStrings;
}


std::vector<int > I3ScaleCalculator::GetOuterStations () const {
  std::vector<int > outerStations;
  outerStations.clear ();

  switch (topConf_) {
  case IT_GUESS:
    log_error("Still guessing required, should not happen!");
    break;
  case IT_UNKNOWN:
    log_error("Unknown IceTop detector configuration");
    break;
  case IT_EMPTY:
    log_info("Leaving IceTop empty.");
    break;
  case IT_CUSTOM:
    log_info("Using a custom IceTop detector configuration");
    outerStations = listOfBoundarySurfaceStations_;
    break;
  case IT73_SMOOTH: // This one "smooths out the notch" by one station
    outerStations.push_back(2);
    outerStations.push_back(6);
    outerStations.push_back(50);
    outerStations.push_back(74);
    outerStations.push_back(73); // <--- here's where the smoothening is
    outerStations.push_back(78);
    outerStations.push_back(75);
    outerStations.push_back(41);
    break;
  case IT73_STRICT: // This one takes the "notch" more literally
    outerStations.push_back(2);
    outerStations.push_back(6);
    outerStations.push_back(50);
    outerStations.push_back(74);
    outerStations.push_back(72); // <--- no smoothing on this one
    outerStations.push_back(78);
    outerStations.push_back(75);
    outerStations.push_back(41);
    break;
  case IT81_SMOOTH:  // This one "smooths out the notch" by one station
    outerStations.push_back(1);
    outerStations.push_back(6);
    outerStations.push_back(50);
    outerStations.push_back(74);
    outerStations.push_back(73); // <--- here's where the smoothening is
    outerStations.push_back(78);
    outerStations.push_back(75);
    outerStations.push_back(31);
    break;
  case IT81_STRICT:  // This one takes the "notch" more literally
    outerStations.push_back(1);
    outerStations.push_back(6);
    outerStations.push_back(50);
    outerStations.push_back(74);
    outerStations.push_back(72); // <--- no smoothing on this one
    outerStations.push_back(78);
    outerStations.push_back(75);
    outerStations.push_back(31);
    break;
  case IT_INFILL_STA2_STRICT:   // "Tall and narrow" - just the 6 stations in the 2-station trigger
    outerStations.push_back(26);
    outerStations.push_back(80);
    outerStations.push_back(81);
    outerStations.push_back(46);
    outerStations.push_back(79);
    break;
  case IT_INFILL_STA2_BIGOVAL:   // A "big oval" surrounding 2-station trigger, the next layer of stations out
    outerStations.push_back(17);
    outerStations.push_back(18);
    outerStations.push_back(27);
    outerStations.push_back(47);
    outerStations.push_back(56);
    outerStations.push_back(55);
    outerStations.push_back(25);
    break;
  case IT_INFILL_TRIANGLE:   // Looks smore ort of triangular, also includes stations 27 and 37
    outerStations.push_back(26);
    outerStations.push_back(27);
    outerStations.push_back(37);
    outerStations.push_back(46);
    outerStations.push_back(79);
    break;
  default:
    log_fatal("Unknown configuration %d", topConf_);
  }
  return outerStations;
}


  
void I3ScaleCalculator::CalcOuterStringPositions (std::vector<double > &x, 
                                                  std::vector<double > &y,
                                                  double &zMin,
                                                  double &zMax) const {
 
  // get the string numbers
  std::vector<int > outerStrings = GetOuterStrings ();

  // get the geo
  I3OMGeoMap omMap = geo_->omgeo;

  x.clear ();
  y.clear ();
  zMin = 0;
  zMax = 0;

  int nmiddle = (bottomDOMid_ + topDOMid_)/2;  // When between 1 and 60, use 30 (rounded down)
  log_debug("Middle DOM = %d", nmiddle);
  
  // calculate the positions:
  // the overall z-coordinates (top and bottom) are made from the average z's of the boundary
  BOOST_FOREACH (int stringNo, outerStrings) {
    OMKey key (stringNo, nmiddle);  // pick roughly the middle of the string
    double xPos = omMap[key].position.GetX ();
    double yPos = omMap[key].position.GetY ();
    x.push_back (xPos);
    y.push_back (yPos);
    log_debug("String %d: x=%f y=%f", stringNo, xPos, yPos);
    // Get coordinates of top and bottom
    OMKey keyTop (stringNo, topDOMid_);  // top of the string
    OMKey keyBot (stringNo, bottomDOMid_);  // bottom of the string
    zMax += omMap[keyTop].position.GetZ();
    zMin += omMap[keyBot].position.GetZ();
  }

  zMin /= outerStrings.size ();
  zMax /= outerStrings.size ();

}

void I3ScaleCalculator::CalcOuterStationPositions (std::vector<double > &x, 
                                                   std::vector<double > &y,
                                                   double &z) const {

  // get the station numbers
  std::vector<int > outerStrings = GetOuterStations ();
  
  // get the geometry
  I3StationGeoMap stationMap = geo_->stationgeo;
  x.clear ();
  y.clear ();

  // calculate the positions of the two tanks within the station, and average them (X and Y)
  // the z-coordinate is fixed.
  BOOST_FOREACH (int stringNo, outerStrings) {
    x.push_back ((stationMap[stringNo][0].position.GetX () 
                  + stationMap[stringNo][1].position.GetX ()) / 2);
    y.push_back ((stationMap[stringNo][0].position.GetY ()
                  + stationMap[stringNo][1].position.GetY ()) / 2);
  }
  z = Z_TOP;
}


double I3ScaleCalculator::ScaleInIce (I3Particle part) const {
  if (iceConf_ > IC_EMPTY) {
    if (part.IsCascade ()) {
      return ScaleInIceCascade (part,false);
    }
    else {
      return ScaleInIceMuon (part);
    }
  }
  else {
    log_error ("Unknown or empty IceCube Configuration.");
    return std::numeric_limits<double >::signaling_NaN ();
  }
};  

double I3ScaleCalculator::ScaleIceCubeDetectorPolygon (I3Particle part) const {
  if (iceConf_ > IC_EMPTY) {
    if (part.IsCascade ()) {
      return ScaleInIceCascade (part,true);
    }
    else {
      log_error ("Particle must be of shape Cascade to calculate IceCube detector polygon scaling factor");
      return std::numeric_limits<double >::signaling_NaN ();
    }
  }
  else {
    log_error ("Unknown or empty IceCube Configuration.");
    return std::numeric_limits<double >::signaling_NaN ();
  }
};  

double I3ScaleCalculator::ScaleIceTop (I3Particle part) const {
  if (topConf_ > IT_EMPTY) {
    std::vector<double > x;
    std::vector<double > y;
    double z;
    CalcOuterStationPositions (x, y, z);
    return I3Cuts::ContainmentAreaSize (part, x, y, z);
  }
  else {
    log_error ("Unknown or empty IceTop Configuration.");
    return std::numeric_limits<double >::signaling_NaN ();
  }
}


double I3ScaleCalculator::ScaleInIceMuon (I3Particle part) const {
    std::vector<double > x;
    std::vector<double > y;
    double ztop;
    double zbot;
    CalcOuterStringPositions (x, y, zbot, ztop);
    return I3Cuts::ContainmentVolumeSize (part, x, y, ztop, zbot);
}

double I3ScaleCalculator::ScaleInIceCascade (I3Particle part,bool areaonly) const {

  // get detector info
  std::vector<double > x;
  std::vector<double > y;
  double zTop;
  double zBot;

  CalcOuterStringPositions (x, y, zBot, zTop);

  double zMiddle = (zTop + zBot) / 2;

  // replace cascade with a directly upgoing muon
  I3Particle referenceTrack (part);
  referenceTrack.SetShape (I3Particle::InfiniteTrack); // convert to muon
  referenceTrack.SetDir (0, 0); // change to zenith 0

  // calculate the AreaSize
  double areaScale 
    = I3Cuts::ContainmentAreaSize (referenceTrack, x, y, zMiddle);

  // calculate the z-Scale
  double zScale = abs (referenceTrack.GetZ () - zMiddle)
    / (zTop - zMiddle);
  
  if (areaonly) {
  // return only the scaled area
  return areaScale;
  }  
  // return the minimum of both
  return std::max (zScale,  areaScale); 
}

bool I3ScaleCalculator::VertexIsInside (const I3Particle &part) const {

  // get detector info
  std::vector<double > xp;
  std::vector<double > yp;
  double zTop;
  double zBot;
 
  double x = part.GetX ();
  double y = part.GetY ();
  double z = part.GetZ ();

  CalcOuterStringPositions (xp, yp, zBot, zTop);

  // close the polygon:
  xp.push_back (xp[0]);
  yp.push_back (yp[0]);

  return ((z < zTop)
          && (z > zBot)
          && IsInside (x, y, xp, yp));

}

bool I3ScaleCalculator::IsInside(double xp, double yp,
                                 const std::vector<double > &x, 
                                 const std::vector<double > &y) const {
  //  implementation found in root.
   double xint;
   int inter = 0;
   double xn,yn;

   int np = x.size () - 1;

   for (int i=0; i < np; ++i) {
     
      if (i <np-1) {
        xn = x[i+1]; 
        yn = y[i+1];
      }
      else {
        xn = x[0];   
        yn = y[0];
      }
      if (y[i] == yn) continue;
      if (yp <= y[i] && yp <= yn) continue;
      if (y[i] < yp && yn < yp) continue;

      xint = x[i] + (yp-y[i])*(xn-x[i])/(yn-y[i]);
      if (xp < xint) inter++;
   }

   if (inter % 2) {
     return true;
   }
   else {
     return false;
   }
}

