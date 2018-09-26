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

  // lets give our detector some common names
  enum IceCubeConfig {IC_UNKNOWN=-2, IC_GUESS=-1, IC_EMPTY=0, 
                      IC9=9, IC22=22, IC40=40, IC59=59, IC79=79, IC80=80, IC86=86};
  enum IceTopConfig {IT_UNKNOWN=-2, IT_GUESS=-1, IT_EMPTY=0,
                     IT16=16, IT26=26, IT40=40, IT59=59, IT73=73, IT80=80, IT81=81};


  I3ScaleCalculator (I3GeometryConstPtr geo, 
                     IceCubeConfig iceConf = IC_GUESS, 
                     IceTopConfig topConf = IT_GUESS);
  
  IceCubeConfig GuessIceCubeConfig () const;
  IceTopConfig GuessIceTopConfig () const;

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

  double ScaleInIceMuon (I3Particle part) const;
  double ScaleInIceCascade (I3Particle part, bool areaonly) const;
  
  bool IsInside (double xp, double yp,
                const std::vector<double > &x, 
                const std::vector<double > &y) const;

  I3GeometryConstPtr geo_;
  IceCubeConfig iceConf_;
  IceTopConfig topConf_;
};

#endif

