/**
 * This file is the header file of the
 * Scale class
 *
 * copyright  (C) 2007
 * the icecube collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Sebastian Panknin <sebastian.panknin@desy.de>   Last changed by: $LastChangedBy$
 */

#ifndef I3_SCALE_CALCULATOR_H_INCLUDED
#define I3_SCALE_CALCULATOR_H_INCLUDED

#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/geometry/I3Geometry.h"

/**
 * @brief The Scale object can calculate containment and scaling,
 * knowing the geometry of the detector and using phys-service functions
 * 
 */

class I3ScaleCalculator {
  
  // Add the logger facility statements
  SET_LOGGER("I3ScaleCalculator");

 public:

  // Lets identify some common "detector shapes" and give them names.
  // The actual enum-numbers are arbitrary, they'll just help remind us which is which
  enum IceCubeConfig {IC_UNKNOWN=-2, IC_GUESS=-1, IC_EMPTY=0, IC_CUSTOM=1,
                      IC79=79, IC86=86,  // The two backward-compatible definitions
                      IC79_STRICT=78, IC79_SMOOTH=77,  // Two possible ways to deal with the "notch"
                      IC86_STRICT=87, IC86_SMOOTH=88,  // Two possible ways to deal with the "notch"
                      DEEPCORE_ALL=15, DEEPCORE_BELOWDUST=16
                      // Please add here in the future: UPGRADE, GEN2....
  };
  enum IceTopConfig {IT_UNKNOWN=-2, IT_GUESS=-1, IT_EMPTY=0, IT_CUSTOM=1,
                     IT73=73, IT81=81,  // The two backward-compatible definitions
                     IT73_STRICT=72, IT73_SMOOTH=71, // Two possible ways to deal with the "notch"
                     IT81_STRICT=82, IT81_SMOOTH=83, // Two possible ways to deal with the "notch"
                     IT_INFILL_STA2_STRICT=6, IT_INFILL_STA2_BIGOVAL=7, IT_INFILL_TRIANGLE=8 // Experimental new ones?
                     // Please add here in the future: SURFACEARRAY....
  };


  I3ScaleCalculator (I3GeometryConstPtr geo, 
                     IceCubeConfig iceConf = IC_GUESS,              // default = please guess from geo
                     IceTopConfig topConf = IT_GUESS,               // default = please guess from stationgeo
                     std::vector<int> strings = std::vector<int>(),  // default = empty
                     std::vector<int> stations = std::vector<int>(),  // default = empty
                     int topDOMid = 1, int bottomDOMid = 60          // you can customize these too (for the in-ice bit)
                     );
  
  std::vector<int > GetOuterStrings () const;
  std::vector<int > GetOuterStations () const;
  void CalcOuterStringPositions (std::vector<double > &x, 
                                 std::vector<double > &y,
                                 double &zMin,
                                 double &zMax) const;
  void CalcOuterStationPositions (std::vector<double > &x, 
                                 std::vector<double > &y,
                                 double &z) const;

  double ScaleInIce (I3Particle part) const;  
  double ScaleIceCubeDetectorPolygon(I3Particle part) const;  
  double ScaleIceTop (I3Particle part) const;
  
  bool VertexIsInside (const I3Particle &part) const;

 private:

  IceCubeConfig GuessIceCubeConfig () const;
  IceTopConfig GuessIceTopConfig () const;

  double ScaleInIceMuon (I3Particle part) const;
  double ScaleInIceCascade (I3Particle part, bool areaonly) const;
  
  bool IsInside (double xp, double yp,
                const std::vector<double > &x, 
                const std::vector<double > &y) const;

  I3GeometryConstPtr geo_;
  IceCubeConfig iceConf_;
  IceTopConfig topConf_;
  std::vector<int> listOfBoundaryDeepStrings_;
  std::vector<int> listOfBoundarySurfaceStations_;
  int topDOMid_;
  int bottomDOMid_;
};


#endif

