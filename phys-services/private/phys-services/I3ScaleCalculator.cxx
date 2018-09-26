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
                                      IceTopConfig topConf) :
  geo_(geo), iceConf_(iceConf), topConf_(topConf) {
 
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
}

I3ScaleCalculator::IceCubeConfig 
I3ScaleCalculator::GuessIceCubeConfig () const {
  // get geo information
  I3OMGeoMap omMap = geo_->omgeo;

  // count strings
  std::set<int > stringSet;
  BOOST_FOREACH(I3OMGeoMap::value_type om, omMap){
    // avoid AMANDA conflicts
    if(om.first.GetString() > 0)
      stringSet.insert (om.first.GetString ());
  }

  // guess different configs
  int stringNo = stringSet.size ();
  switch (stringNo) {
    case 9:
    case 16: // + IceTop ?         
 // case 28: // + AMANDA           
 // case 35: // + ICETOP and AMANDA
      return IC9;
    case 22:
    case 26: // + IceTop ?
 // case 41: // + AMANDA           
 // case 45: // + ICETOP and AMANDA
      return IC22;
    case 40:
//  case 59:  // IC40 + AMANDA => conflicts with IC59! 
      return IC40;
    case 59:
      return IC59;
    case 79:
      return IC79;
    case 80:
      return IC80;
    case 86:
      return IC86;
    default:
      return IC_UNKNOWN;
    }
}

I3ScaleCalculator::IceTopConfig 
I3ScaleCalculator::GuessIceTopConfig () const {
  // get number of stations from geo
  I3StationGeoMap stationMap = geo_->stationgeo;
  int stationNo = stationMap.size();

  // guess config
  switch (stationNo) {
    case 16:
      return IT16;
    case 26:
      return IT26;
    case 40:
      return IT40;
    case 59:
      return IT59;
    case 73:
      return IT73;
    case 80:
      return IT80;
    case 81:
      return IT81;
    default:
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
    log_info("Empty IceCube detector configuration");
    break;
  case IC9:
    outerStrings.push_back(21);
    outerStrings.push_back(50);
    outerStrings.push_back(59);
    outerStrings.push_back(39);
    outerStrings.push_back(38);
    break;
  case IC22:
    outerStrings.push_back(21);
    outerStrings.push_back(50);
    outerStrings.push_back(74);
    outerStrings.push_back(73);
    outerStrings.push_back(78);
    outerStrings.push_back(65);
    outerStrings.push_back(46);
    outerStrings.push_back(38);
    break;
  case IC40:
    outerStrings.push_back(21);
    outerStrings.push_back(50);
    outerStrings.push_back(74);
    outerStrings.push_back(73);
    outerStrings.push_back(78);
    outerStrings.push_back(75);
    outerStrings.push_back(60);
    outerStrings.push_back(52);
    outerStrings.push_back(53);    // different from ContainmentSize.cxx in flat-ntuple
    outerStrings.push_back(44);
    outerStrings.push_back(46);
    outerStrings.push_back(38);    // different from ContainmentSize.cxx in flat-ntuple
    break;
  case IC59:
    outerStrings.push_back( 6);  
    outerStrings.push_back(50);
    outerStrings.push_back(74);
    outerStrings.push_back(73);
    outerStrings.push_back(78);
    outerStrings.push_back(75);
    outerStrings.push_back(60);
    outerStrings.push_back(52);
    outerStrings.push_back(53);   // different from ContainmentSize.cxx in flat-ntuple
    outerStrings.push_back(44);   // different from ContainmentSize.cxx in flat-ntuple
    outerStrings.push_back(36);
    outerStrings.push_back(17);
    outerStrings.push_back(3);
    break;
  case IC79:
    outerStrings.push_back(2);  
    outerStrings.push_back(6);
    outerStrings.push_back(50);
    outerStrings.push_back(74);
    outerStrings.push_back(73);
    outerStrings.push_back(78);
    outerStrings.push_back(75);
    outerStrings.push_back(41);
    break;
  case IC80:
    outerStrings.push_back(1);
    outerStrings.push_back(6);
    outerStrings.push_back(50);
    outerStrings.push_back(80);
    outerStrings.push_back(75);
    outerStrings.push_back(31);
    break;
  case IC86:
    outerStrings.push_back(1);
    outerStrings.push_back(6);
    outerStrings.push_back(50);
    outerStrings.push_back(74);
    outerStrings.push_back(73);
    outerStrings.push_back(78);
    outerStrings.push_back(75);
    outerStrings.push_back(31);
    break;
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
    log_info("Empty IceTop detector configuration");
    break;
  case IT16:
   outerStations.push_back(21);
   outerStations.push_back(50);
   outerStations.push_back(74);
   outerStations.push_back(47);
   break;
  case IT26:
    outerStations.push_back(21);
    outerStations.push_back(50);
    outerStations.push_back(74);
    outerStations.push_back(73);
    outerStations.push_back(78);
    outerStations.push_back(77);
    outerStations.push_back(64);
    outerStations.push_back(55);
    outerStations.push_back(46);
    outerStations.push_back(38);
    break;
  case IT40:
    outerStations.push_back(21);
    outerStations.push_back(50);
    outerStations.push_back(74);
    outerStations.push_back(73);
    outerStations.push_back(78);
    outerStations.push_back(75);
    outerStations.push_back(60);
    outerStations.push_back(52);
    outerStations.push_back(53);   // different from ContainmentSize.cxx in flat-ntuple
    outerStations.push_back(44);
    outerStations.push_back(46);
    outerStations.push_back(38);   // different from ContainmentSize.cxx in flat-ntuple
    break;
  case IT59:
    outerStations.push_back(6);
    outerStations.push_back(50);
    outerStations.push_back(74);
    outerStations.push_back(73);
    outerStations.push_back(78);
    outerStations.push_back(75);
    outerStations.push_back(60);
    outerStations.push_back(52);
    outerStations.push_back(53);   // different from ContainmentSize.cxx in flat-ntuple
    outerStations.push_back(44);   // different from ContainmentSize.cxx in flat-ntuple
    outerStations.push_back(36);
    outerStations.push_back(2);
    break;
  case IT73:
    outerStations.push_back(2);
    outerStations.push_back(6);
    outerStations.push_back(50);
    outerStations.push_back(74);
    outerStations.push_back(73);
    outerStations.push_back(78);
    outerStations.push_back(75);
    outerStations.push_back(41);
    break;
  case IT80:
    outerStations.push_back(1);
    outerStations.push_back(6);
    outerStations.push_back(50);
    outerStations.push_back(80);
    outerStations.push_back(75);
    outerStations.push_back(31);
    break;
  case IT81:
    outerStations.push_back(1);
    outerStations.push_back(6);
    outerStations.push_back(50);
    outerStations.push_back(74);
    outerStations.push_back(73);
    outerStations.push_back(78);
    outerStations.push_back(75);
    outerStations.push_back(31);
    break;
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

  // calculate the positions
  BOOST_FOREACH (int stringNo, outerStrings) {
    OMKey key (stringNo, 30);  // pick the middle of the string
    double xPos = omMap[key].position.GetX ();
    double yPos = omMap[key].position.GetY ();
    x.push_back (xPos);
    y.push_back (yPos);
    log_debug("String %d: x=%f y=%f", stringNo, xPos, yPos);
    // Get coordinates of top and bottom
    OMKey keyTop (stringNo,  1);  // top of the string
    OMKey keyBot (stringNo, 60);  // bottom of the string
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

  // calculate the positions
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

