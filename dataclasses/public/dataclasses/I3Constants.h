/**
 * copyright  (C) 2004
 * the icecube collaboration
 * @version $Id$
 * @file I3Constants.h
 * @date $Date$
 */

#ifndef I3CONSTANTS_H_INCLUDE
#define I3CONSTANTS_H_INCLUDE

#include "icetray/I3Units.h"
#include <cmath>

/**
 * @brief A list of static variables commonly used by physics analysis
 *
 * Just a namespace filled with constants -- add a line
 * <tt> using namespace I3Constants </tt> to get access to them
 * directly, or just use @c I3Constants::pi, for example.
 * 
 * @todo probably need more constants here.   Add them here when you need them.
 */
namespace I3Constants
{
  /**
   * The speed of light in vacuum
   */
  static const double c = 2.99792458e8 * I3Units::m / (I3Units::second);

  /**
   * Impedance of free space
   */

  static const double z_vacuum = 376.730313668 * I3Units::ohm;

  /**
   * Boltzmann temperature constant (i.e. k_B)
   */

  static const double KBoltzmann = 1.38064852e-23 * I3Units::joule / I3Units::kelvin;

  /**
   * The "average" phase velocity (for wavelength of 400nm)
   * @sa https://arxiv.org/abs/hep-ex/0008001
   */
  static const double n_ice_phase = 1.3195;

  /**
   * The "average" group velocity (for wavelength of 400nm)
   * @sa https://arxiv.org/abs/hep-ex/0008001
   */
  static const double n_ice_group = 1.35634;

  /**
   * The index of refraction of ice
   */
  static const double n_ice = n_ice_group;

  /**
   * The Cherenkov angle in ice
   */
  static const double theta_cherenkov = std::acos(1/n_ice_phase) * I3Units::rad;

  /**
   * The speed of light in ice
   */
  static const double c_ice = c/n_ice_group;
  
  /**
   * pi.
   */
  static const double pi = M_PI;
  
  /**
   * Euler's number
   */
  static const double e = M_E;

  /**
   *  Avaogadro's Number
   */
 
  static const double NA = 6.0221415e23;

  /**
   * Elevation of ice surface (floor of string 21 deployment tower)
   * @sa http://wiki.icecube.wisc.edu/index.php/Coordinate_system
   */
  static const double SurfaceElev = 9291.31*I3Units::feet;

  /**
   * Elevation of IceCube origin (z=0) -- BY DEFINITION
   * From email from Kurt Woschnagg on 08/25/2004
   * @sa http://wiki.icecube.wisc.edu/index.php/Coordinate_system
   */
  static const double OriginElev = 2900*I3Units::feet;

  /**
   * Z-coordinate of IceTop (Origin Depth)
   * Obtained from DEFINED elevation of origin and MEASURED surface elevation
   *
   * @todo Make sure that the elevation of IceTop is the same as the elevation
   * of AMANDA hole 4.  If not, then correct for the difference.
   */
  static const double zIceTop = SurfaceElev-OriginElev;

  /**
   * Earth radius constant
   */
  static const double earthRadius = 6.371315e6*I3Units::m;

  /**
   * Conversion between IceCube and AMANDA coordinates
   * from docushare file by Kurt Woschnagg
   * (x,y,z)icecube = (x,y,z)amanda - (x,y,z)shift
   */
  static const double Coordinate_shift_x = -339.8*I3Units::m;
  static const double Coordinate_shift_y = -117.4*I3Units::m;
  static const double Coordinate_shift_z = -216.0*I3Units::m;


  /**
   * Default values for time residual calculations
   * It's good to have these in one central place.
   */
  static const double dt_window_l = -15*I3Units::ns;
  static const double dt_window_h = +25*I3Units::ns;
};

#endif //I3CONSTANTS_H_INCLUDED

