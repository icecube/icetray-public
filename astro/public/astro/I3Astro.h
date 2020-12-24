#ifndef I3ASTRO_H_INCLUDED
#define I3ASTRO_H_INCLUDED

/**
 * @brief High-level functions for astronomical conversions
 * for IceCube datatypes 
 *
 * @copyright (C) 2015 The IceCube Collaboration
 * 
 * $Id$
 *
 * @file I3Astro.h
 * @author Kevin Meagher
 * @date August 2015
 * $Rev$
 * $LastChangedBy$
 * $LastChangedDate$
 *
 */

#include "icetray/I3FrameObject.h"
#include "icetray/I3Units.h"
class I3Time;
class I3Direction;

///Nominal latitude of the icecube detector in radians
const double ICECUBE_LATITUDE  = -89.9944*I3Units::degree;
///Nominal longitude of the icecube detector in radians
const double ICECUBE_LONGITUDE = -62.6081*I3Units::degree;


/**
 * @brief Container class for storing equatorial (J2000) coordinates
 */
struct I3Equatorial : public I3FrameObject
{
 I3Equatorial() : ra(NAN),dec(NAN) {};
 I3Equatorial(double r,double d) : ra(r),dec(d) {}; 
  
  /// Right Ascension in radians
  double ra;
  /// Declination in radians in radians 
  double dec;
};

/**
 * @brief Container class for storing galactic coordinates (IAU 1958)
 */
struct I3Galactic : public I3FrameObject
{
 I3Galactic() : l(NAN),b(NAN) {};
 I3Galactic(double gl,double gb) : l(gl),b(gb) {};
  
  /// Galactic Longitude in radians
  double l;
  /// Galactic Latitude in radians
  double b;
};

/**
 * @brief Container class for storing supergalactic (de Vaucouleurs) coordinates
 */
struct I3SuperGalactic : public I3FrameObject
{
 I3SuperGalactic() : l(NAN),b(NAN) {};
 I3SuperGalactic(double SGL,double SGB) : l(SGL),b(SGB) {};

  /// Supergalactic Longitude in radians 
  double l;
  /// Supergalactic Latitude in radians
  double b;
};

/**
 * @brief Container class for storing ecliptic coordinates 
 */
struct I3Ecliptic: public I3FrameObject
{
 I3Ecliptic() : l(NAN),b(NAN) {};
 I3Ecliptic(double el,double eb) : l(el),b(eb) {};
  
  /// Galactic Longitude in radians
  double l;
  /// Galactic Latitude in radians
  double b;
};



/**
 * @brief Convert from I3Time to sidereal time (GMST)
 *
 * @param eventTime - the time of the observation in I3Time
 * @returns GMST in decimal hours
 *
 */
double I3GetGMST(const I3Time& eventTime);


/**
 * @brief Convert from I3Time to antisidereal time (GMAST)
 *
 * @param eventTime - the time of the observation in I3Time
 * @returns GMAST in decimal hours
 *
 */
double I3GetGMAST(const I3Time& eventTime);

/**
 * @brief Convert from I3Time to extended-sidereal time (GMEST)
 *
 * @param eventTime - the time of the observation in I3Time
 * @returns GMEST in decimal hours
 *
 */

double I3GetGMEST(const I3Time& eventTime);

/**
 * @brief Gets the direction of the Moon in local IceCube coordinates 
 * at time
 *
 * @param time - the time of the observation in I3Time
 * @returns I3Direction object
 */
I3Direction I3GetMoonDirection(const I3Time& time);


/**
 * @brief Gets the direction of the Sun in local IceCube coordinates 
 * at time
 *
 * @param time - I3Time of the astronomical observation
 * @returns I3Direction object
 */
I3Direction I3GetSunDirection(const I3Time& time);

/**
 * @brief Convert an astronomical position in J2000 equatorial 
 * coordinates to local IceCube coordnates at a given time
 * 
 * @param equatorial - I3Equatorial position of astronomical observation 
 *        in J2000 coordinate system
 * @param time - I3Time of the astronomical observation
 * @returns I3Direction containing local IceCube direction
 */
I3Direction I3GetDirectionFromEquatorial(const I3Equatorial& equatorial,const I3Time& time);

/**
 * @brief Convert an IceCube direction to 
 * J2000 equatorial coordinates system at a given time
 *
 * @param direction - IceCube local coordinate to convert
 * @param time - I3Time of the astronomical observation
 * @returns I3Equatorial astronomical position of the 
 *          event in J2000 coordinate system
 */
I3Equatorial I3GetEquatorialFromDirection(const I3Direction& direction,const I3Time& time);

/**
 * @brief Convert from Equatorial (J2000) to Ecliptic coordinate system
 * 
 * @param equatorial - I3Equatorial position of astronomical observation 
 *        in J2000 coordinate system
 * @param time - I3Time of the astronomical observation
 * @returns I3Ecliptic position in ecliptic coordinate system
 */
I3Ecliptic I3GetEclipticFromEquatorial(const I3Equatorial& equatorial, const I3Time& time);

/**
 * @brief Convert from Ecliptic to Equatorial (J2000) coordinate system
 * 
 * @param ecliptic - I3Ecliptic position in galactic coordinate system
 * @param time - I3Time of the astronomical observation
 * @returns I3Equatorial position of astronomical observation 
 *          in J2000 coordinate system
 */
I3Equatorial I3GetEquatorialFromEcliptic(const I3Ecliptic& ecliptic, const I3Time& time);



/**
 * @brief Convert from Equatorial (J2000) to Galactic (IAU 1958) coordinate system
 * 
 * @param equatorial - I3Equatorial position of astronomical observation 
 *        in J2000 coordinate system
 * @returns I3Galactic position in galactic coordinate system
 */
I3Galactic I3GetGalacticFromEquatorial(const I3Equatorial& equatorial);

/**
 * @brief Convert from Galactic (IAU 1958) to Equatorial (J2000) coordinate system
 * 
 * @param galactic - I3Galactic position in galactic coordinate system
 * @returns I3Equatorial position of astronomical observation 
 *          in J2000 coordinate system
 */
I3Equatorial I3GetEquatorialFromGalactic(const I3Galactic& galactic);

/**
 * @brief Convert from Galactic (IAU 1958) to SuperGalactic (de Vaucouleurs) coordinate system
 *
 * @param galactic - I3Galactic position in galactic coordinate system
 * @returns I3SuperGalactic position of astronomical observation
 *
 */
I3SuperGalactic I3GetSuperGalacticFromGalactic(const I3Galactic& galactic);

/**
 * @brief Convert from SuperGalactic (de Vaucouleurs) to Galactic (IAU 1958) coordinate system
 *
 * @param supergalactic - I3SuperGalactic position in SuperGalactic coordinate system
 * @returns I3Galactic position of astronomical observation in IAU 1958 coordinate system
 *
 */
I3Galactic I3GetGalacticFromSuperGalactic(const I3SuperGalactic& supergalactic);

/**
 * @brief Convert from Equatorial (J2000) to SuperGalactic (de Vaucouleurs) coordinate system
 *
 * @param equatorial - I3Equatorial position in Equatorial (J200) coordinate system
 * @returns I3SuperGalactic position of astronomical observation in de Vaucouleurs coordinate system
 *
 */
I3SuperGalactic I3GetSuperGalacticFromEquatorial(const I3Equatorial& equatorial);

/**
 * @brief Convert from SuperGalactic (de Vaucouleurs) to Equatorial (J2000) coordinate system
 *
 * @param supergalactic - I3SuperGalactic position in Supergalactic (de Vaucouleurs) coordinate system
 * @returns I3Equatorial position of astronomical observation in J2000 coordinate system
 *
 */
I3Equatorial I3GetEquatorialFromSuperGalactic(const I3SuperGalactic& supergalactic);




#endif //I3ASTRO_H_INCLUDED

