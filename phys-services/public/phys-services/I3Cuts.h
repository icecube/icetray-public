#ifndef I3CUTS_H
#define I3CUTS_H

#include "dataclasses/I3Constants.h"
#include <vector>

template <typename Key, typename Value> struct I3Map;
class OMKey; 
class I3Particle;
class I3RecoPulse;
class I3Geometry;

#ifdef I3_USE_CINT
// cint needs this for dictionary generation
#include "dataclasses/I3Position.h"
#else
class I3Position;
#endif

#define CONTAINMENT_FUNCTIONS_AVAILABLE

// Not ready yet...
//#define IC40_CONTAINMENT_BUG_HAS_BEEN_FIXED 1

/**
 * @brief The I3Cuts namespace is a collection of functions that calculate 
 * various cut parameters for reconstruction.  These include things like 
 * Ndir, Ldir, and Smoothness.
 * 
 * @todo So far we have Ndir, Ldir, and Smoothness, but other cut parameters
 * will and should be added as we have move more into an analysis phase of the 
 * experiment.
 */
namespace I3Cuts
{
  /**
   * Main function that does the actual calculation of all of the cut 
   * parameters.  First, it loops over all hits 
   * (or pulses), makes "projections" of the hits onto the input track, and 
   * calculates the time residual for each hit (difference between the 
   * expected and actual time of arrival of the Cherenkov photon from the 
   * given track and the OM).  If the time residual is within some set time 
   * window, then the hit is tagged as "direct hit".  Then, based on the 
   * times and distances of the direct (or all) hits, it calculates the 3 
   * quantities, according to their individual algorithms.
   * 
   * 
   * INPUT:
   * @param track --  Track used in calculation.
   * @param geometry -- Geometry object from the event.
   * @param pulsemap -- Map of I3RecoPulseSeries used in calculation. 
   * @param t1 -- Lower edge of the time residual window for direct hits.
   *              Generally, this value is -15ns.
   * @param t2 -- Upper edge of the time residual window for direct hits.
   *              Generally, this value is 15ns..150ns.
   * 
   * 
   * OUTPUT:
   * @param Nchan -- The Nchan cut parameter: total number of hit channels.
   * @param Nhit -- The Nhit cut parameter: total number of hits.
   * @param Nstring -- The Nstring cut parameter: total number of strings.
   * @param Ndir -- The Ndir cut parameter: number of direct hits.
   * @param Ldir -- The Ldir cut parameter (based on direct hits): "length" 
   *                of the event.  This is the length between the two most 
   *                extreme projections of the direct hits onto the track.  
   * @param Sall -- The Smoothness parameter (based on all hits): how 
   *                uniformly are the hit projections onto the track 
   *                distributed along that track.
   * @param Sdir -- The Smoothness parameter (based on direct hits): how 
   *                uniformly are the hit projections onto the track 
   *                distributed along that track.
   * 
   * 
   * @todo Think about making the code more efficient...
   */

  void CutsCalc(const I3Particle& track, 
		const I3Geometry& geometry, 
		const I3Map< OMKey, std::vector< I3RecoPulse > >& pulsemap,
		const double t1, 
		const double t2, 
		int& Nchan, 
		int& Nhit, 
		int& Nstring,
		int& Ndir, 
		double& Ldir,
		double& Sdir,
		double& Sall);


  /**
   * Main function that does the actual calculation of all of the cut 
   * parameters.  It loops over all hitseries it finds in the
   * hitseriesmap, calculating the cut parameters.
   * 
   * INPUT:
   * @param vertex --  Vertex used in calculation.
   * @param geometry -- Geometry object from the event.
   * @param pulsemap -- Map of I3RecoPulseSeries used in calculation. 
   * @param t1 -- Lower edge of the time residual window for direct hits.
   *              Generally, this value is -15ns.
   * @param t2 -- Upper edge of the time residual window for direct hits.
   *              Generally, this value is 15ns..150ns.
   * 
   * 
   * OUTPUT:
   * @param Nchan -- The Nchan cut parameter: total number of hit channels.
   * @param Nhit -- The Nhit cut parameter: total number of hits.
   * @param N_1hit -- Number of channels with only 1 hit in them.
   * @param Nstring -- The Nstring cut parameter: total number of strings.
   * @param Ndir -- The Ndir cut parameter: number of direct hits.
   * @param Nearly -- The Nearly cut parameter: number of early hits.
   * @param Nlate -- The Nlate cut parameter: number of late hits.
   * 
   * @todo Think about making the code more efficient...
   */

  void CascadeCutsCalc(const I3Particle& vertex, 
		       const I3Geometry& geometry, 
		       const I3Map< OMKey, std::vector< I3RecoPulse > >& pulsemap,
		       const double t1, 
		       const double t2, 
		       int& Nchan, 
		       int& Nhit, 
		       int& N_1hit, 
		       int& Nstring,
		       int& Ndir, 
		       int& Nearly,
		       int& Nlate);


  /**
   * A function which calculates the center of gravity of the hits. 
   * It takes a I3RecoPulseSeries map and the geometry,
   * and returns an I3Position of the COG.
   */
  I3Position COG(const I3Geometry& geometry,
		 const I3Map< OMKey, std::vector< I3RecoPulse > >& pulsemap);

  /**
   * A convenience function that calls CutsCalc() and returns the total 
   * number of channels (hit OMs) in the event.  If you are interested in
   * more than one quantity from CutsCalc(), use the CutsCalc() function 
   * directly, in order to save multiple calls to the function.
   */
  int Nchan(const I3Particle& track, 
	   const I3Geometry& geom, 
	   const I3Map< OMKey, std::vector<I3RecoPulse> >& hitmap, 
	   double t1 = I3Constants::dt_window_l, 
	   double t2 = I3Constants::dt_window_h);

  /**
   * A convenience function that calls CutsCalc() and returns the total 
   * number of hits in the event.  If you are interested in more than 
   * one quantity from CutsCalc(), use the CutsCalc() function directly,
   * in order to save multiple calls to the function.
   */
  int Nhit(const I3Particle& track, 
	   const I3Geometry& geom, 
	   const I3Map< OMKey, std::vector<I3RecoPulse> >& hitmap, 
	   double t1 = I3Constants::dt_window_l, 
	   double t2 = I3Constants::dt_window_h);

  /**
   * A convenience function that calls CutsCalc() and returns the total 
   * number of hits in the event.  If you are interested in more than 
   * one quantity from CutsCalc(), use the CutsCalc() function directly,
   * in order to save multiple calls to the function.
   */
  int N_1hit(const I3Particle& vertex, 
	     const I3Geometry& geom, 
	     const I3Map< OMKey, std::vector<I3RecoPulse> >& hitmap, 
	     double t1 = I3Constants::dt_window_l, 
	     double t2 = I3Constants::dt_window_h);

  /**
   * A convenience function that calls CutsCalc() and returns the total 
   * number of strings in the event.  If you are interested in more than 
   * one quantity from CutsCalc(), use the CutsCalc() function directly,
   * in order to save multiple calls to the function.
   */
  int Nstring(const I3Particle& track, 
	   const I3Geometry& geom, 
	   const I3Map< OMKey, std::vector<I3RecoPulse> >& hitmap, 
	   double t1 = I3Constants::dt_window_l, 
	   double t2 = I3Constants::dt_window_h);

  /**
   * A convenience function that calls CutsCalc() and returns the number 
   * of direct hits from a given track.  If you are interested in more than 
   * one quantity from CutsCalc(), use the CutsCalc() function directly,
   * in order to save multiple calls to the function.
   */
  int Ndir(const I3Particle& track, 
	   const I3Geometry& geom, 
	   const I3Map< OMKey, std::vector<I3RecoPulse> >& hitmap, 
	   double t1 = I3Constants::dt_window_l, 
	   double t2 = I3Constants::dt_window_h);

  /**
   * A convenience function that calls CutsCalc() and returns the number 
   * of direct hits from a given track.  If you are interested in more than 
   * one quantity from CutsCalc(), use the CutsCalc() function directly,
   * in order to save multiple calls to the function.
   */
  int CascadeNdir(const I3Particle& vertex, 
		  const I3Geometry& geom, 
		  const I3Map< OMKey, std::vector<I3RecoPulse> >& hitmap, 
		  double t1 = I3Constants::dt_window_l, 
		  double t2 = I3Constants::dt_window_h);

  /**
   * A convenience function that calls CutsCalc() and returns the number 
   * of direct hits from a given track.  If you are interested in more than 
   * one quantity from CutsCalc(), use the CutsCalc() function directly,
   * in order to save multiple calls to the function.
   */
  int Nearly(const I3Particle& vertex, 
	     const I3Geometry& geom, 
	     const I3Map< OMKey, std::vector<I3RecoPulse> >& hitmap, 
	     double t1 = I3Constants::dt_window_l, 
	     double t2 = I3Constants::dt_window_h);

  /**
   * A convenience function that calls CutsCalc() and returns the number 
   * of direct hits from a given track.  If you are interested in more than 
   * one quantity from CutsCalc(), use the CutsCalc() function directly,
   * in order to save multiple calls to the function.
   */
  int Nlate(const I3Particle& vertex, 
	    const I3Geometry& geom, 
	    const I3Map< OMKey, std::vector<I3RecoPulse> >& hitmap, 
	    double t1 = I3Constants::dt_window_l, 
	    double t2 = I3Constants::dt_window_h);

  /**
   * A convenience function that calls CutsCalc() and returns the 
   * "direct-hits length" of the event.  If you are interested in more than  
   * one quantity from CutsCalc(), use the CutsCalc() function directly, 
   * in order to save multiple calls to the function.
   */
  double Ldir(const I3Particle& track, 
	      const I3Geometry& geom, 
	      const I3Map< OMKey, std::vector<I3RecoPulse> >& hitmap, 
	      double t1 = I3Constants::dt_window_l, 
	      double t2 = I3Constants::dt_window_h);

  /**
   * A convenience function that calls CutsCalc() and returns the 
   * "direct-hits smoothness" of the event.  If you are interested in more   
   * than one quantity from CutsCalc(), use the CutsCalc() function, 
   * directly in order to save multiple calls to the function.
   */
  double SmoothDir(const I3Particle& track, 
		   const I3Geometry& geom, 
		   const I3Map< OMKey, std::vector<I3RecoPulse> >& hitmap, 
		   double t1 = I3Constants::dt_window_l, 
		   double t2 = I3Constants::dt_window_h);

  /**
   * A convenience function that calls CutsCalc() and returns the 
   * "all-hits smoothness" of the event.  If you are interested in more than  
   * one quantity from CutsCalc(), use the CutsCalc() function directly, 
   * in order to save multiple calls to the function.
   */
  double SmoothAll(const I3Particle& track, 
		   const I3Geometry& geom, 
		   const I3Map< OMKey, std::vector<I3RecoPulse> >& hitmap, 
		   double t1 = I3Constants::dt_window_l, 
		   double t2 = I3Constants::dt_window_h);

  /**
   * Computes the size of the "cylinder of closest approach", 
   * which is the amount by which one must expand a "reference cylinder"
   * (such as the volume of the in-ice detector) in order to just barely
   * contain the track.
   * For grusome defail, see Kath's thesis appendix A.
   *
   * "H0" = height of reference cylinder
   * "R0" = radius of reference cylinder
   * "center" = z-coordinate of center of reference cylinder
   *           (the x- and y-coordinates are assumed to be at zero)
   *
   * This is useful, for instance, for making cuts on whether a track goes
   * through the physical volume of the in-ice detector, close to the center,
   * or outside of it.
   */
  double CylinderSize(const I3Particle& track, 
		      double H0, 
		      double R0, 
		      double center);

  /**
   * A helper function for doing 2-D geometry: the intersection of two lines.
   * One line is defined by coordinates (x1,y1) and (x2, y2),
   * The second line is defined by coordinates (x3, y3) and (x4, y4).
   */
  void IntersectionOfTwoLines(double x1, double y1, double x2, double y2,
			      double x3, double y3, double x4, double y4,
			      double *xi, double *yi);

  /**
   * A helper function for doing 3-D geometry: the intersection of a line
   * and a plane.  The line is defined by a track I3Particle, and the plane
   * is defined by three I3Positions.
   */
  I3Position IntersectionOfLineAndPlane(const I3Particle& t,
					I3Position A, I3Position B, I3Position C);


  /**
   * A helper function for doing 2-D geometry: the center of mass of a 2-D 
   * arbitrary shape (n-gon or polygon).
   * This function takes as inputs: vectors representing the x and y coordinates
   * of points which define the corners of the shape.
   * (Could, for instance, find the CM of the IceTop-16 array.)
   */
  void CMPolygon(std::vector<double> x, 
		 std::vector<double> y, 
		 double *xresult,
		 double *yresult);

  /** 
   * A helper function which re-orders the "boundary" points of a polygon,
   * so that they are in order (either cw or ccw around a central point
   */
  void PutPointsInOrder(std::vector<double> *xinput, 
			std::vector<double> *yinput, 
			double xcenter, double ycenter,
			bool justcheck);

  /**
   * A helper function which quickly says whether ang2 is 
   * counter-clockwise from ang1 (by less than 180 degrees) or not.
   */
  bool CCW(double ang1, double ang2, bool exact);

  /**
   * A helper function which, given a center (xcm,ycm), two triangle endpoints, 
   * and a "question point" (xprime, yprime), computes the factor by which a
   * line would have to expand outwards from the center in order to intersect
   * the question point.  (This is part of ContainmentAreaSize.)
   */
  double TriangleExpansionFactor(double xcm, double ycm, //vertex from which triangle will expand
				 double x1, double y1,  // triangle point 2
				 double x2, double y2,  // triangle point 3
				 double xprime, double yprime);  // "the point" 


  /**
   * Computes the size of the "containment volume of closest approach", 
   * Analogous to "CylinderSize", above, but for a general shape
   * (not necessarily a cylinder... could be the IC-9 prism for instance)
   *
   * Must be a prism shape... defined by (x,y) coordinates, and 
   * limits of z in the vertical dimension
   */
  double ContainmentVolumeSize(const I3Particle& track, 
			       std::vector<double> x, 
			       std::vector<double> y, 
			       double zhigh, 
			       double zlow);

  /**
   * Computes the size of the "containment area of closest approach", 
   * Similar to "ContainmentVolumeSize", above, but the 2-dimensional
   * (rather than 3-dimensional) problem for a general shaped
   * area rather than a volume.  
   * (Could, for instance, be the IceTop-16 array.)
   */
  double ContainmentAreaSize(const I3Particle& track, 
			     std::vector<double> x, 
			     std::vector<double> y, 
			     double z);


}

#endif


