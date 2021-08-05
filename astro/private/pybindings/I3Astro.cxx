/**
 * @brief Python pindings for astronimical conversions
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

#include "icetray/load_project.h"
#include "dataclasses/physics/I3Particle.h"
#include "tableio/converter/pybindings.h"
#include "astro/I3Astro.h"
#include "astro/I3AstroConverters.h"

using namespace boost::python;

I3_PYTHON_MODULE(astro)
{
  load_project("astro", false);
  import("icecube.icetray");

  scope().attr("ICECUBE_LONGITUDE")=ICECUBE_LONGITUDE;
  scope().attr("ICECUBE_LATITUDE" )=ICECUBE_LATITUDE;

  class_<I3Equatorial,bases<I3FrameObject> >("I3Equatorial",
    "Container class for storing equatorial (J2000) coordinates")
    .def(init<double, double>())
    .def_readwrite("ra",&I3Equatorial::ra,"Right Ascension in radians")
    .def_readwrite("dec",&I3Equatorial::dec,"Declination in radians in radians")
    ;
  
  class_<I3Galactic,bases<I3FrameObject> >("I3Galactic",
    "Container class for storing galactic coordinates (IAU 1958)")
    .def(init<double, double>())
    .def_readwrite("l",&I3Galactic::l,"Galactic Longitude in radians")
    .def_readwrite("b",&I3Galactic::b,"Galactic Latitude in radians")
    ;

  class_<I3SuperGalactic,bases<I3FrameObject> >("I3SuperGalactic",
    "Container class for storing supergalactic (de Vaucouleurs) coordinates")
    .def(init<double, double>())
    .def_readwrite("l",&I3SuperGalactic::l,"Supergalactic Longitude in radians")
    .def_readwrite("b",&I3SuperGalactic::b,"Supergalactic Latitude in radians")
    ;

  def("I3GetMoonDirection",
      &I3GetMoonDirection,
      (args("time")),
      "Returns the direction of the Moon in local IceCube coordinates at a given time\n"
      "\n"
      "Args:\n"
      " time (I3Time): time of the moon observation\n"
      "Returns:\n"
      " direction(I3Direction):Zenith and azimuth of the Moon obesrvation\n"
      );
  
  def("I3GetSunDirection",
      &I3GetSunDirection,
      args("time"),
      "Returns the direction of the Sun in local IceCube coordinates at a given time\n"
      "\n"
      "Args:\n"
      " time(I3Time): time of the astronomical observation\n"
      "Returns:\n"
      " direction (I3Direction): Zenith and azimuth of the Sun obesrvation\n"
      );

  def("I3GetDirectionFromEquatorial",
      &I3GetDirectionFromEquatorial,
      (args("equatorial"),arg("time")),
      "Convert an astronomical position in J2000 equatorial coordinates "
      "to local IceCube coordnates at a given time\n"
      "\n"
      "Args:\n"
      " equatorial (I3Equatorial): Right ascension and declination (J2000) of an astronomical observation\n"
      " time (I3Time): time of the astronomical observation\n"
      "Returns:\n"
      " direction(I3Direction): Zenith and azimuth of the astronomical obesrvation\n"
      );
  
  def("I3GetEquatorialFromDirection",
      &I3GetEquatorialFromDirection,
      (args("direction"),arg("time")),
      "Convert an IceCube direction to J2000 equatorial coordinates system at a given time\n"
      "\n"
      "Args:\n"
      " direction(I3Direction): Zenith and azimuth of the astronomical obesrvation\n"
      " time(I3Time): time of the astronomical observation\n"
      "Returns:\n"
      " equatorial(I3Equatorial): Right ascension and declination (J2000) of an astronomical observation\n"
      );
  
  def("I3GetGalacticFromEquatorial",
      &I3GetGalacticFromEquatorial,
      (args("equatorial")),
      "Convert from Equatorial (J2000) to Galactic (IAU 1958) coordinate system\n"
      "\n"
      "Args:\n"
      " equatorial(I3Equatorial): Right ascension and declination (J2000)\n"
      "Returns:\n"
      " galactic (I3Galactic): Galactic latitude and longitude\n"
      );
  
  def("I3GetEquatorialFromGalactic",
      &I3GetEquatorialFromGalactic,
      args("galactic"),
      "Convert from Galactic (IAU 1958) to Equatorial (J2000) coordinate system\n"
      "\n"
      "Args:\n"
      " galactic (I3Galactic): Galactic latitude and longitude\n"
      "Returns:\n"
      " equatorial (I3Equatorial): Right ascension and declination (J2000)\n"
      );

  def("I3GetSuperGalacticFromGalactic",
      &I3GetSuperGalacticFromGalactic,
      args("galactic"),
      "Convert from Galactic (IAU 1958) to Supergalactic (de Vaucouleurs) coordinate system\n"
      "\n"
      "Args:\n"
      " supergalactic (I3SuperGalactic): Supergalactic latitude and longitude\n"
      "Returns:\n"
      " galactic (I3Galactic): Galactic latitude and longitude\n"
      );

  def("I3GetGalacticFromSuperGalactic",
      &I3GetGalacticFromSuperGalactic,
      args("supergalactic"),
      "Convert from Supergalactic (de Vaucouleurs) to Galactic (IAU 1958) coordinate system\n"
      "\n"
      "Args:\n"
      " galactic (I3Galactic): Galactic latitude and longitude\n"
      "Returns:\n"
      " supergalactic (I3SuperGalactic): Supergalactic latitude and longitude\n"
      );

  def("I3GetSuperGalacticFromEquatorial",
      &I3GetSuperGalacticFromEquatorial,
      args("equatorial"),
      "Convert from Equatorial (J2000) to SuperGalactic (de Vaucouleurs) coordinate system\n"
      "\n"
      "Args:\n"
      " equatorial(I3Equatorial): Right ascension and declination (J2000)\n"
      "Returns:\n"
      " supergalactic (I3SuperGalactic): Supergalactic latitude and longitude\n"
      );

  def("I3GetEquatorialFromSuperGalactic",
      &I3GetEquatorialFromSuperGalactic,
      args("supergalactic"),
      "Convert from SuperGalactic (de Vaucouleurs) to Equatorial (J2000) coordinate system\n"
      "\n"
      "Args:\n"
      " supergalactic (I3SuperGalactic): Supergalactic latitude and longitude\n"
      "Returns:\n"
      " equatorial (I3Equatorial): Right ascension and declination (J2000)\n"
      );

  def("I3GetGMST",
      &I3GetGMST,
      (args("time")),
      "Returns the Greenich Mean Sideral Time\n"
      "\n"
      "Args:\n"
      " time (I3Time): time of observation\n"
      "Returns:\n"
      " gmst(double):sidereal time in decimal hours\n"
      );

   def("I3GetGMAST",
      &I3GetGMAST,
      (args("time")),
      "Returns the Greenich Mean Anti Sideral Time\n"
      "\n"
      "Args:\n"
      " time (I3Time): time of observation\n"
      "Returns:\n"
      " ast(double):anitsidereal time in decimal hours\n"
      );

    def("I3GetGMEST",
      &I3GetGMEST,
      (args("time")),
      "Returns the Greenich Mean Extended Sideral Time\n"
      "\n"
      "Args:\n"
      " time (I3Time): time of observation\n"
      "Returns:\n"
      " est(double):extened-sidereal time in decimal hours\n"
      );
 



  I3CONVERTER_NAMESPACE(astro);
  I3CONVERTER_EXPORT(I3AstroConverter, "Books equatorial coordinates of an I3Particle")
    .def(bp::init<const std::string&>())
    ;
  I3CONVERTER_EXPORT(I3GalacticConverter, "Books galactic coordinates of an I3Particle")
    .def(bp::init<const std::string&>())
    ;
  I3CONVERTER_EXPORT(I3SuperGalacticConverter, "Books supergalactic coordinates of an I3Particle")
    .def(bp::init<const std::string&>())
    ;
  I3CONVERTER_EXPORT(I3SunAndMoonConverter, "Books direction of Sun and Moon at the time of the Event")
    ;
}

