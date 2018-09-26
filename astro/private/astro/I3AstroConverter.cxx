/**
 * @brief Tableio Converter for I3Particle to store 
 * astronomical data
 *
 * @copyright (C) 2015 The Icecube Collaboration
 * 
 * $Id$
 *
 * @file I3AstroConverter.cxx
 * @author Kevin Meagher
 * @date August 2015
 * $Rev$
 * $LastChangedBy$
 * $LastChangedDate$
 */

#include "icetray/I3Frame.h"
#include "icetray/I3Units.h"
#include "dataclasses/physics/I3Particle.h"
#include "astro/I3Astro.h"
#include "I3AstroConverters.h"

I3TableRowDescriptionPtr I3AstroConverter::CreateDescription(const I3Particle&)
{
  I3TableRowDescriptionPtr desc(new I3TableRowDescription);
  desc->AddField<double>("ra", "radian", "right ascension (FK5 J2000)");
  desc->AddField<double>("dec", "radian", "declination (FK5 J2000)");
  return desc;
}

size_t I3AstroConverter::FillRows(const I3Particle& particle, I3TableRowPtr rows)
{
  //tableio won't call FillRows() if there is no I3EventHeader
  const I3EventHeader& evtHeader = currentFrame_->Get<I3EventHeader>(eventHeaderName_);

  I3Equatorial eq = I3GetEquatorialFromDirection(particle.GetDir(),
						 evtHeader.GetStartTime());

  log_debug("Booking particle with: zenith=%8.4lf azimuth=%8.4f ra=%8.4lf dec=%+8.4lf",
	    particle.GetZenith()/I3Units::degree,particle.GetAzimuth()/I3Units::degree,
	    eq.ra/I3Units::degree,eq.dec/I3Units::degree
	    );
  
  rows->Set<double>("ra", eq.ra);
  rows->Set<double>("dec", eq.dec);

  return 1;
}

I3TableRowDescriptionPtr I3GalacticConverter::CreateDescription(const I3Particle&)
{
  I3TableRowDescriptionPtr desc(new I3TableRowDescription);
  desc->AddField<double>("gal_l", "radian", "galactic latitude (IAU1958)");
  desc->AddField<double>("gal_b", "radian", "galactic longitude (IAU1958)" );
  return desc;
}

size_t I3GalacticConverter::FillRows(const I3Particle& particle, I3TableRowPtr rows)
{
  //tableio won't call FillRows() if there is no I3EventHeader
  const I3EventHeader& evtHeader = currentFrame_->Get<I3EventHeader>(eventHeaderName_);

  I3Equatorial eq = I3GetEquatorialFromDirection(particle.GetDir(),
						 evtHeader.GetStartTime());
  I3Galactic gal = I3GetGalacticFromEquatorial(eq);

  log_debug("Booking particle with: zenith=%8.4lf azimuth=%8.4lf l=%+9.4lf b=%+8.4lf",
        particle.GetZenith()/I3Units::degree,particle.GetAzimuth()/I3Units::degree,
	    gal.l/I3Units::degree, gal.b/I3Units::degree
	    );
  
  rows->Set<double>("gal_l", gal.l);
  rows->Set<double>("gal_b", gal.b);

  return 1;
}

I3TableRowDescriptionPtr I3SuperGalacticConverter::CreateDescription(const I3Particle&)
{
  I3TableRowDescriptionPtr desc(new I3TableRowDescription);
  desc->AddField<double>("sg_l", "radian", "supergalactic latitude (de Vaucouleurs)");
  desc->AddField<double>("sg_b", "radian", "supergalactic longitude (de Vaucouleurs)");
  return desc;
}

size_t I3SuperGalacticConverter::FillRows(const I3Particle& particle, I3TableRowPtr rows)
{
  //tableio won't call FillRows() if there is no I3EventHeader
  const I3EventHeader& evtHeader = currentFrame_->Get<I3EventHeader>(eventHeaderName_);

  I3Equatorial eq = I3GetEquatorialFromDirection(particle.GetDir(),
						 evtHeader.GetStartTime());
  I3SuperGalactic sg = I3GetSuperGalacticFromEquatorial(eq);

  log_debug("Booking particle with: zenith=%8.4lf azimuth=%8.4lf sgl=%+9.4lf sgb=%+8.4lf",
	    particle.GetZenith()/I3Units::degree,particle.GetAzimuth()/I3Units::degree,
	    sg.l/I3Units::degree, sg.b/I3Units::degree
	    );
  
  rows->Set<double>("sg_l", sg.l);
  rows->Set<double>("sg_b", sg.b);

  return 1;
}

I3TableRowDescriptionPtr I3SunAndMoonConverter::CreateDescription(const I3EventHeader&)
{
  I3TableRowDescriptionPtr desc(new I3TableRowDescription);
  desc->AddField<double>("sun_zenith"  , "radian", "Zenith angle of the Sun");
  desc->AddField<double>("sun_azimuth" , "radian", "Azimuth angle of the Sun");
  desc->AddField<double>("moon_zenith" , "radian", "Zenith angle of the Sun");
  desc->AddField<double>("moon_azimuth", "radian", "Azimuth angle of the Sun");
  return desc;
}

size_t I3SunAndMoonConverter::FillRows(const I3EventHeader& header, I3TableRowPtr rows)
{
  I3Direction sun  = I3GetSunDirection (header.GetStartTime());
  I3Direction moon = I3GetMoonDirection(header.GetStartTime());
  
  log_debug("Booking direction of Sun: zenith=%8.4f azimuth=%8.4f and Moon: zenith=%8.4f azimuth=%8.4f",
	    sun .GetZenith()/I3Units::degree, sun .GetAzimuth()/I3Units::degree,
	    moon.GetZenith()/I3Units::degree, moon.GetAzimuth()/I3Units::degree
	    );
  
  rows->Set<double>( "sun_zenith" , sun .GetZenith ());
  rows->Set<double>( "sun_azimuth", sun .GetAzimuth());
  rows->Set<double>("moon_zenith" , moon.GetZenith ());
  rows->Set<double>("moon_azimuth", moon.GetAzimuth());

  return 1;
}
