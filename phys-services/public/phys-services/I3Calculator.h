/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3Calculator.h
 * @version $Revision$
 * @date $Date$
 * @author pretz, dule, ehrlich
 */
#ifndef I3CALCULATOR_H
#define I3CALCULATOR_H

#include <cmath>
#include <utility>
#include "dataclasses/physics/I3Particle.h"
#include "icetray/I3Units.h"
#include "dataclasses/I3Constants.h"
#include "dataclasses/geometry/I3OMGeo.h"
#include "dataclasses/I3Direction.h"


/**
 * @brief This is a namespace, which provides a collection of stand-alone 
 * functions that calculate various quantites relating a track, a cascade, 
 * and an OM position.  These quantities include a various distances between 
 * two points, angle difference between two directions, time of travel of 
 * light...
 *
 * Many of the methods are wrapper methods, which are designed for ease of 
 * use,  and internally call the main calculation methods.  These can be used 
 * for simplicity.  The expert user can just use the main calculation 
 * methods/functions.
 */
namespace I3Calculator
{
  /**
   * The main calculation function, which calculates in one step the relevant 
   * quantities that relate a track to an OM position.  If you are interested
   * in all of these quantities (see Output Parameters), use this function.
   * 
   * 
   * INPUT:
   * @param track -- const reference to a track.
   *
   * @param position -- Position of an OM.
   *
   * @param IndexRefG -- Index of refraction (group), default=1.35634 (from I3Constants)
   *                     This is used to define the speed of light through ice.
   *
   * @param IndexRefP -- Index of refraction (phase), default=1.3195 (from I3Constants)
   *                     This is used to define the angle (w.r.t. the track)
   *                     under which Cherenkov photons are emitted in ice.
   *
   * @param direction -- Orientation of the OM.
   *
   *
   * OUTPUT:
   * @param chpos -- Position on the track, which is the
   * origin of Cherenkov light that arrives at the input OM position
   * ('position').  If no Cherenkov light originating from the input track
   * can arrive at the input OM position, 'chpos' will be a null I3Position.
   *
   * @param chtime -- Time between the track's nominal time 
   * and the time of the arrival of Cherenkov light at the input OM position.  
   * This 'chtime' includes the time (which could be negative) for the 
   * particle to travel (at speed c) from the track's nominal time to the 
   * point of origin of Cherenkov light ('chpos') PLUS the time (which is 
   * always positive) it takes the Cherenkov light to travel (at speed c/n) 
   * from its point of origin to the input OM position.  If no Cherenkov 
   * light originating from the input track can arrive at the input OM 
   * position, 'chtime' will be NAN.
   *
   * @param chdist -- Distance between the point of origin 
   * of Cherenkov light and the input OM position.
   * 
   * @param changle -- Angle between the path of Cherenkov 
   * light originating from the input track ('track') and the z-axis of the 
   * input OM position ('position').
   * 
   *
   * @todo Currently, CherenkovCalc calculates "closest approach" AND 
   * "cherenkov distances".  This is good for simplicity and non-repetitiveness
   * of the code, but it is not the most efficient.  If processing time becomes
   * an issue, we can make these things more efficient at the expence of
   * complicating and repeating the code.
   *
   * @sa I3Constants for refractive index (phase, group)
   */
  void CherenkovCalc(const I3Particle& track,
		     const I3Position& position,
		     I3Position& chpos,
		     double& chtime,
		     double& chdist,
		     double& changle,
		     const double IndexRefG=I3Constants::n_ice_group,
		     const double IndexRefP=I3Constants::n_ice_phase,
		     const I3Direction& direction=I3Direction(0.,0.,-1.));

  /**
   * Check if the input position ('position') lies on the input track 
   * ('track') within the given Precision.  The default Precision is 10cm.
   * 
   * @todo IsOnTrack uses CherenkovCalc for calculating "distance of closest
   * approach".  This method is not the most efficient, but makes the code much
   * simpler.  If processing time becomes an issue, we can make these routines 
   * more efficient.
   */
  bool IsOnTrack(const I3Particle& track, 
		 const I3Position& position,
		 const double Precision=0.1*I3Units::meter);

  /**
   * ------NEW!----------
   * Function which does the legwork for computing things related to the
   * distance of closest approach of a track to a point.  
   * Used by CherenkovCalc, and also by the convenience functions
   * ClosestApproachPosition and ClosestApproachDistance.
   * Originally inside CherenkovCalc, but useful enough to bring outside.
   * Returns two sets of closest-approach results: one "purely geometrical"
   * and one which takes into account stopping/starting points of track.
   */
  void ClosestApproachCalc(const I3Particle& track,
			   const I3Position& position,
			   I3Position& appos_inf,
			   double& apdist_inf,
			   I3Position& appos_stopstart,
			   double& apdist_stopstart);
  /**
   * A convenience function that calls CherenkovCalc() and returns the 
   * position of closest approach from (an I3Position) between the input 
   * track and OM position.  If you are interested in more than one quantity
   * from CherenkovCalc(), use the CherenkovCalc() function directly, in
   * order to save multiple calls to the function.
   */
  I3Position ClosestApproachPosition(const I3Particle& particle,
				     const I3Position& position);

  /**
   * A convenience function that calls CherenkovCalc() and returns the 
   * distance of closest approach (a double) between the input track and 
   * OM position.  If you are interested in more than one quantity
   * from CherenkovCalc(), use the CherenkovCalc() function directly, in
   * order to save multiple calls to the function.
   */
  double ClosestApproachDistance(const I3Particle& particle,
				 const I3Position& position);

  /**
   * Small useful function which computes the distance along a track
   * from its vertex to the place where an OM position is perpendicular.
   * (Used to be inside CutsCalc, but heck, it's a useful function for
   * other stuff too!)
   */
  double DistanceAlongTrack(const I3Particle& track, const I3Position& ompos);

 /**
   * A convenience function that calls CherenkovCalc() and returns the 
   * position on the input track, which is the point of origin of Cherenkov 
   * light which arrives at the input OM position.  If no Cherenkov light 
   * from the track can reach the OM, the function returns a null I3Position.
   * If you are interested in more than one quantity from CherenkovCalc(), 
   * use the CherenkovCalc() function directly, in order to save multiple 
   * calls to the function.
   */
  I3Position CherenkovPosition(const I3Particle& particle,
			       const I3Position& position,
			       const double IndexRefG=I3Constants::n_ice_group,
			       const double IndexRefP=I3Constants::n_ice_phase);

 /**
   * A convenience function that calls CherenkovCalc() and returns the time
   * between the track's nominal time and the time of arrival of Cherenkov 
   * light at the input OM position.  If no Cherenkov light from the 
   * track can reach the OM, the function returns a NAN.  If you are 
   * interested in more than one quantity from CherenkovCalc(), use the 
   * CherenkovCalc() function directly, in order to save multiple calls to 
   * the function.
   */
  double CherenkovTime(const I3Particle& particle,
		       const I3Position& position,
		       const double IndexRefG=I3Constants::n_ice_group,
		       const double IndexRefP=I3Constants::n_ice_phase);

 /**
   * A convenience function that calls CherenkovCalc() and returns the 
   * distance from origin of Cherenkov light on the input track to the input 
   * OM position.  If you are interested in more than one quantity
   * from CherenkovCalc(), use the CherenkovCalc() function directly, in
   * order to save multiple calls to the function.
   */
  double CherenkovDistance(const I3Particle& particle,
			   const I3Position& position,
			   const double IndexRefG=I3Constants::n_ice_group,
			   const double IndexRefP=I3Constants::n_ice_phase);

 /**
   * A convenience function that calls CherenkovCalc() and returns the
   * angle between the path of Cherenkov light from the input track and the 
   * z-axis of the input OM position.  An optional input is the direction 
   * of the OM (default = Down).  If you are interested in more than one 
   * quantity from CherenkovCalc(), use the CherenkovCalc() function 
   * directly, in order to save multiple calls to the function.
   */
  double CherenkovApproachAngle(const I3Particle& particle,
			      const I3Position& position,
			      const I3Direction& direction=I3Direction(0.,0.,-1.),
			      const double IndexRefG=I3Constants::n_ice_group,
			      const double IndexRefP=I3Constants::n_ice_phase);

  /**
   * A convenience function that calls CherenkovTime() and calculates the time 
   * residual between a hit and a track.
   * A time residual is defined as the difference between the measured hit time
   * and the expected time of arrival of a photon from the track.
   */
  double TimeResidual(const I3Particle& particle, 
		      const I3Position& hitpos, 
		      const double hittime,
		      const double IndexRefG=I3Constants::n_ice_group,
		      const double IndexRefP=I3Constants::n_ice_phase);

  /**
   * Returns the spatial angle between two input particles.
   */
  double Angle(const I3Particle& p1,
	       const I3Particle& p2);

  /**
   * Returns the distance between two input particles.
   */
  double Distance(const I3Particle& p1, 
		  const I3Particle& p2);

  /**
   * Returns the 4-distance between two input particles
   *  (using both distance and time)
   */
  double FourDistance(const I3Particle& p1, 
		      const I3Particle& p2);

  /**
   * Transform the input position into the coordinate system of the input 
   * track.  Effectively, the track is now in the z-dir.
   */
  I3Position InTrackSystem(const I3Direction& direction, 
			   const I3Position& pos);

  /**
   * Transform the input position into the coordinate system of the input 
   * track.  Effectively, the track is now in the z-dir.
   */
  I3Direction InTrackSystem(const I3Direction& direction, 
			    const I3Direction& dir);

  /**
   * Convenience function overload that extracts the direction out of a track
   * and then calls the regular InTrackSystem().
   */
  I3Position InTrackSystem(const I3Particle& track, 
			   const I3Position& pos);

  /**
   * Convenience function overload that extracts the direction out of a track
   * and then calls the regular InTrackSystem().
   */
  I3Direction InTrackSystem(const I3Particle& track, 
			    const I3Direction& dir);

  /**
   * Transform the input position out of the coordinate system of the input 
   * track.  Effectively, the track is now in its original direction.
   */
  I3Position InNominalSystem(const I3Direction& direction, 
			     const I3Position& pos);

  /**
   * Transform the input position out of the coordinate system of the input 
   * track.  Effectively, the track is now in its original direction.
   */
  I3Direction InNominalSystem(const I3Direction& direction, 
			      const I3Direction& dir);

  /**
   * Convenience function overload that extracts the direction out of a track
   * and then calls the regular InNominalSystem().
   */
  I3Position InNominalSystem(const I3Particle& track, 
			     const I3Position& pos);

  /**
   * Convenience function overload that extracts the direction out of a track
   * and then calls the regular InNominalSystem().
   */
  I3Direction InNominalSystem(const I3Particle& track, 
			      const I3Direction& dir);

  /**
   * Utility function, produces two directions perpendicular to the
   * input direction. Should maybe be included in the I3Calculator:: 
   * namespace in phys-services.
   */
  std::pair<I3Direction,I3Direction>
  GetTransverseDirections(const I3Direction &dir);

  /**
   * Utility function, produces one directions opposite to the
   * input direction. Should maybe be included in the I3Calculator:: 
   * namespace in phys-services.
   */
  I3Direction GetReverseDirection(const I3Direction &dir);

  /**
   * Utility function, rotates a direction around an axis.
   * Should maybe be included in the I3Calculator:: namespace
   * in phys-services.
   */
  void Rotate(const I3Direction &axis, I3Direction &dir, double angle );


  /**
   * Transform the input position into the shower coordinate system (SCS).
   * Our SCS has its origin in the shower core, a z-direction parallel to the
   * direction of shower origin, and the x-axis anti-parallel to the time flow
   * of signals appearing in the event, meaning that early hits are located at
   * x > 0, late hits at x < 0. The y-axis is parallel to the ground plane and
   * perpendicular to the time flow of signals.
   */
  I3Position InShowerSystem(const I3Particle& shower, const I3Position& pos);

  /**
   * Same as InShowerSystem, but with core and propagation direction axis
   * explicitly given.
   */
  I3Position InShowerSystem(const I3Position& core, const I3Direction& axis,
                            const I3Position& pos);

};

#endif
