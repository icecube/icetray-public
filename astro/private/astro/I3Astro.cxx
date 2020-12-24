/**
 * @brief 
 *
 * @copyright (C) 2015 The Icecube Collaboration
 * 
 * $Id$
 *
 * @file I3Astro.cxx
 * @author Kevin Meagher
 * @date August 2015
 * $Rev$
 * $LastChangedBy$
 * $LastChangedDate$
 * 
 */

#include <star/pal.h>
#include "dataclasses/I3Time.h"
#include "dataclasses/I3Direction.h"
#include "astro/I3Astro.h"

namespace {

  /**
   * do this ourselves cause I3Time does something weird on days with leapseconds
   */
  double get_mjd(const I3Time& time)
  {
    return time.GetModJulianDay()+
           time.GetModJulianSec()/86400.+
           time.GetModJulianNanoSec()/8.64e13;
  }
  /**
   * Floored modular division rounding toward negative infinity
   *
   * It is totally ridiculous that c++ doesn't have this function
   * but the relativitly useless fmod and remainder are part of
   * the specification
   */
  double floored_division_remainder(double x,double y)
  {
    return fmod(fmod(x,y)+y,y);
  }

  /** 
   * Convert apparent equatorial cooridnates to local coordinates
   * This function is hidden in an anonymous namespace because 
   * apparent coordinates are too similar to J2000 and it confuses 
   * people.
   */
  I3Direction i3direction_from_apparent(double ra_rad,
					double dec_rad,
					double mjd)
  {
    double PalAz=NAN;
    double PalZen=NAN;
    double PalHA=NAN;
    double PalDec=NAN;
    double PalRA=NAN;

    // SLA_AOP : Apparent to Observed
    // ACTION  : Apparent to observed place, for sources distant from the solar system.
    palAop(ra_rad,dec_rad,   //geocentric apparent [α,δ] (radians)
	   mjd,              // date/time (Modified Julian Date, JD−2400000.5)
	   0,                //UT1−UTC (UTC seconds)
	   ICECUBE_LONGITUDE,//observer’s mean longitude (radians, east +ve)
	   ICECUBE_LATITUDE, //observer’s mean geodetic latitude (radians)
	   0,                //observer’s height above sea level (metres)
	   0,0,              //polar motion [x,y] coordinates (radians)
	   0,                //local ambient temperature (K; std=273.15D0)
	   0,                //local atmospheric pressure (mb; std=1013.25D0)
	   0,                //local relative humidity (in the range 0D0 – 1D0)
	   0,                //effective wavelength (μm, e.g. 0.55D0)
	   0,                //tropospheric lapse rate (K per metre, e.g. 0.0065D0)
	   //RETURNED:
	   &PalAz,           //observed azimuth (radians: N=0, E=90∘)
	   &PalZen,          //observed zenith distance (radians)
	   &PalHA,           //observed Hour Angle (radians)
	   &PalDec,          //observed δ (radians)
	   &PalRA            //observed α (radians)
	   );
	   
    
    //convert to IceCube Coordinates
    double ic_az  = M_PI/2. - PalAz - ICECUBE_LONGITUDE;
    
    //normaize azimuth to [0, 360)
    ic_az = floored_division_remainder(ic_az,2*M_PI);
    
    i3_assert ( ic_az  <= 2*M_PI);
    i3_assert ( ic_az  >= 0 );
    i3_assert ( PalZen <= M_PI);
    i3_assert ( PalZen >= 0);
    
    return I3Direction(PalZen,ic_az);
  }

  I3Direction i3direction_of_planet(int planet_number, const I3Time& time)
  {
    double mjd = get_mjd(time);

    //PAL_DTT : TT minus UTC PAL_DTT
    //ACTION  : Compute ∆TT, the increment to be applied to Coordinated
    //          Universal Time UTC to give Terrestrial Time TT.
    double tt = mjd+palDtt(mjd)/86400.;

    double raRad=NAN;
    double decRad=NAN;
    double diameterRad=NAN;
    //PAL_RDPLAN : Apparent [ α, δ ] of Planet PAL_RDPLAN
    //ACTION     : Approximate topocentric apparent [ α, δ ]
    //           : and angular size of a planet.
    palRdplan( tt,               //MJD of observation (JD−2400000.5) TT can be used 
	       planet_number,    // 0 = Sun, 3 = Moon
	       ICECUBE_LONGITUDE,// observer’s longitude (east +ve)
	       ICECUBE_LATITUDE, // and latitude (radians)
	       //RETURNED
	       &raRad, &decRad,  //topocentric apparent [α,δ] (radians)
	       &diameterRad      //angular diameter (equatorial, radians)
	       );
    
    return i3direction_from_apparent(raRad,decRad,mjd);
  }
  
}//anonymous namespace

double I3GetGMST(const I3Time& time)
{
  // NOTE: MJD is expected with respect to UT
  const double mjd = get_mjd(time);
  double gmst = palGmst(mjd)*I3Units::radian/I3Units::degree;
  gmst = fmod(gmst, 360);
  gmst = (gmst < 0.) ? gmst + 360: gmst;

  return gmst*24/360; // in hours
}

double I3GetGMAST(const I3Time& time)
{
  // NOTE: MJD is expected with respect to UT
  const double mjd = get_mjd(time);
  const double gmst = I3GetGMST(time);
  double dt = mjd - gmst;
  double ast = mjd + dt;
  ast = fmod(ast, 24);
  ast = (ast < 0.) ? ast + 24: ast;

  return ast; // in hours
}

double I3GetGMEST(const I3Time& time)
{
  // NOTE: MJD is expected with respect to UT
  const double mjd = get_mjd(time);
  const double gmst = I3GetGMST(time);
  double dt = mjd - gmst;
  double ast = mjd - 2*dt;
  ast = fmod(ast, 24);
  ast = (ast < 0.) ? ast + 24: ast;

  return ast; // in hours
}



I3Direction I3GetMoonDirection(const I3Time& time)
{
  //Moon = 3
  return i3direction_of_planet(3,time);
}

I3Direction I3GetSunDirection(const I3Time& time)
{
  //Sun = 0
  return i3direction_of_planet(0,time);
}

I3Direction I3GetDirectionFromEquatorial(const I3Equatorial &eq, const I3Time &time)
{

  double mjd = get_mjd(time);

  //PAL_DTT : TT minus UTC PAL_DTT
  //ACTION  : Compute ∆TT, the increment to be applied to Coordinated
  //          Universal Time UTC to give Terrestrial Time TT.
  double tt = mjd+palDtt(mjd)/86400.;
  
  double raRad  = NAN;
  double decRad = NAN;
  //SLA_MAP : Mean to Apparent SLA_MAP
  //ACTION  : Transform star [ α, δ ] from mean place to geocentric apparent. The reference
  //        : frames and timescales used are post IAU 1976.
  palMap( eq.ra,    //mean RA 
  	  eq.dec,   //mean dec
  	  0.0,      //proper motions RA
  	  0.0,      //proper motions Dec
  	  0.0,      //parallax			       
  	  0.0,      // radial velocity
  	  2000.0,   //epoch and eqinox of star data (Julian)
  	  tt,       //TDB for apparent place (JD−2400000.5)
	  // RETURNS
  	  &raRad,  //apparent RA
  	  &decRad  //apparent Dec			       
  	  );

  return i3direction_from_apparent(raRad,decRad,mjd);
}

I3Equatorial I3GetEquatorialFromDirection(const I3Direction & dir, const I3Time &time)
{
  double mjd = get_mjd(time);

  // Convert local coordinates from IceCube convention to Astronomy convention
  double PalAz = M_PI/2. - dir.GetAzimuth() - ICECUBE_LONGITUDE;

  double raRad  = NAN;
  double decRad = NAN;

  // SLA_OAP  : Observed to Apparent
  // ACTION   : Observed to apparent place.
  char coordtype[] ={'A'};
  palOap (coordtype,              //type of coordinates – ‘R’, ‘H’ or ‘A’ 
	  PalAz,            //observed Az, HA or RA (radians; Az is N=0, E=90∘)
	  dir.GetZenith(),  //observed zenith distance or δ (radians)
	  mjd,              //date/time (Modified Julian Date, JD−2400000.5)
	  0,                //UT1−UTC (UTC seconds)
	  ICECUBE_LONGITUDE,//observer’s mean longitude (radians, east +ve)
	  ICECUBE_LATITUDE, //observer’s mean geodetic latitude (radians)
	  0,                //observer’s height above sea level (metres)
	  0,0,              //polar motion [x,y] coordinates (radians)
	  0,                //local ambient temperature (K; std=273.15D0)
	  0,                //local atmospheric pressure (mb; std=1013.25D0)
	  0,                //local relative humidity (in the range 0D0 – 1D0)
	  0,                //effective wavelength (μm, e.g. 0.55D0)
	  0,                //tropospheric lapse rate (K per metre, e.g. 0.0065D0)
	  //RETURNED
	  &raRad,&decRad    //Returned geocentric apparent [α,δ]
	  );

  //SLA_AMP  : Apparent to Mean SLA_AMP
  //ACTION   : Convert star [ α, δ ] from geocentric apparent to mean place (post IAU 1976).
  double mra  = NAN;
  double mdec = NAN;
  palAmp(raRad,decRad, // apparent [ α, δ ] (radians)
	 mjd,          // TDB for apparent place (JD−2400000.5)
	 2000.0,       // equinox: Julian epoch of mean place
	 //RETURNED 
	 &mra, &mdec   // mean [ α, δ ] (radians)
	 );
  
  I3Equatorial eq(mra,mdec);

  i3_assert(eq.ra  >=  0);
  i3_assert(eq.ra  <=  2*M_PI);
  i3_assert(eq.dec >= -M_PI/2);
  i3_assert(eq.dec <= +M_PI/2);
  
  return eq;
}

I3Ecliptic I3GetEclipticFromEquatorial(const I3Equatorial& eq, const I3Time& time)
{
  double ecl_longitude(NAN);
  double ecl_latitude(NAN);
  double mjd = get_mjd(time);

  //PAL_EQECL : J2000 α, δ to Ecliptic
  //ACTION    : Transform from J2000.0 equatorial coordinates 
  //          : to ecliptic coordinates 
  palEqecl( eq.ra, eq.dec, mjd, &ecl_longitude, &ecl_latitude);

  I3Ecliptic ecl(ecl_longitude, ecl_latitude);

  i3_assert(ecl.l >=  0);
  i3_assert(ecl.l <=  2*M_PI);
  i3_assert(ecl.b >= -M_PI/2);
  i3_assert(ecl.b <= +M_PI/2);
  
  return ecl;
}

I3Equatorial I3GetEquatorialFromEcliptic(const I3Ecliptic& ecl, const I3Time& time)
{
  double eq_ra(NAN);
  double eq_dec(NAN);
  double mjd = get_mjd(time);

  //PAL_ECLEQ : Ecliptic to J2000 α, δ 
  //ACTION    : Transform from ecliptic coordinates 
  //          : to J2000.0 FK5 equatorial coordinates.
  palEcleq(ecl.l,ecl.b,mjd,&eq_ra,&eq_dec);
  
  I3Equatorial eq(eq_ra, eq_dec);

  i3_assert(eq.ra  >=  0);
  i3_assert(eq.ra  <=  2*M_PI);
  i3_assert(eq.dec >= -M_PI/2);
  i3_assert(eq.dec <= +M_PI/2);

  
  return eq;

}

I3Galactic I3GetGalacticFromEquatorial(const I3Equatorial& eq)
{
  double gal_longitude(NAN);
  double gal_latitude(NAN);
  
  //PAL_EQGAL : J2000 α, δ to Galactic
  //ACTION    : Transformation from J2000.0 FK5 equatorial coordinates
  //            to IAU 1958 galactic coordinates
  palEqgal( eq.ra, eq.dec, &gal_longitude, &gal_latitude);

  I3Galactic gal(gal_longitude,gal_latitude);

  i3_assert(gal.l >=  0);
  i3_assert(gal.l <=  2*M_PI);
  i3_assert(gal.b >= -M_PI/2);
  i3_assert(gal.b <= +M_PI/2);
  
  
  return gal;
}

I3Equatorial  I3GetEquatorialFromGalactic(const I3Galactic& gal)
{
  double eq_ra(NAN);
  double eq_dec(NAN);

  //PAL_GALEQ : Galactic to J2000 α, δ 
  //ACTION    : Transformation from IAU 1958 galactic coordinates
  //          : to J2000.0 FK5 equatorial coordinates.
  palGaleq(gal.l,gal.b,&eq_ra,&eq_dec);
  
  I3Equatorial eq(eq_ra, eq_dec);

  i3_assert(eq.ra  >=  0);
  i3_assert(eq.ra  <=  2*M_PI);
  i3_assert(eq.dec >= -M_PI/2);
  i3_assert(eq.dec <= +M_PI/2);
  
  return eq;
}

I3SuperGalactic I3GetSuperGalacticFromGalactic(const I3Galactic& gal)
{
  double sg_longitude(NAN);
  double sg_latitude(NAN);

  palGalsup( gal.l, gal.b, &sg_longitude, &sg_latitude );

  I3SuperGalactic SG(sg_longitude, sg_latitude);

  i3_assert(SG.l >= 0);
  i3_assert(SG.l <= 2*M_PI);
  i3_assert(SG.b >= -M_PI/2);
  i3_assert(SG.b <= M_PI/2);

  return SG;
}

I3Galactic I3GetGalacticFromSuperGalactic(const I3SuperGalactic& SG)
{
  double gal_longitude(NAN);
  double gal_latitude(NAN);

  palSupgal( SG.l, SG.b, &gal_longitude, &gal_latitude );

  I3Galactic gal(gal_longitude, gal_latitude);

  i3_assert(gal.l >= 0);
  i3_assert(gal.l <= 2*M_PI);
  i3_assert(gal.b >= -M_PI/2);
  i3_assert(gal.b <= M_PI/2);

  return gal;
}

I3SuperGalactic I3GetSuperGalacticFromEquatorial(const I3Equatorial& eq)
{
  I3Galactic gal = I3GetGalacticFromEquatorial(eq);
  I3SuperGalactic SG = I3GetSuperGalacticFromGalactic(gal);
  return SG;
}

I3Equatorial I3GetEquatorialFromSuperGalactic(const I3SuperGalactic& SG)
{
  I3Galactic gal = I3GetGalacticFromSuperGalactic(SG);
  I3Equatorial eq = I3GetEquatorialFromGalactic(gal);
  return eq;
}
