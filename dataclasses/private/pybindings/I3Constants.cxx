//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   This file is part of IceTray.
//
//   IceTray is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
//   IceTray is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <boost/preprocessor.hpp>
#include <vector>

#include <dataclasses/I3Constants.h>

using namespace boost::python;

#define I3_CONSTANT_NAMES			\
  (c)(n_ice_phase)(n_ice_group)(n_ice)		\
  (theta_cherenkov)(c_ice)(pi)(e)(NA)		\
  (SurfaceElev)(OriginElev)(zIceTop)		\
  (Coordinate_shift_x)(Coordinate_shift_y)	\
  (Coordinate_shift_z)(dt_window_l)(dt_window_h)

#define I3CONSTANT_DEF(r,data,t) \
  .def_readonly(BOOST_PP_STRINGIZE(t), BOOST_PP_CAT(I3Constants::, t))

// dummy class just used as a namespace.
namespace {
  struct dummy { };
}

void register_I3Constants()
{
  class_<dummy>("I3Constants")
    .def_readonly("c", I3Constants::c, "Speed of light in vacuum")
    .def_readonly("n_ice_phase", I3Constants::n_ice_phase, "the average phase velocity for wavelength of 400nm") 
    .def_readonly("n_ice_group", I3Constants::n_ice_group, "avg group velocity for 400nm")
    .def_readonly("n_ice", I3Constants::n_ice, "index of refraction of ice")
    .def_readonly("theta_cherenkov", I3Constants::theta_cherenkov, "cherenkov angle in ice")
    .def_readonly("c_ice", I3Constants::c_ice, "c in ice")
    .def_readonly("pi", I3Constants::pi, "what is pi doing duplicated here")
    .def_readonly("e", I3Constants::e, "OY-ler's number.  awww yeah you know how we do.")
    .def_readonly("NA", I3Constants::NA, "avogadro's number")
    .def_readonly("SurfaceElev", I3Constants::SurfaceElev, "elevation of surface (amanda hole 4)")
    .def_readonly("OriginElev", I3Constants::OriginElev, "elev of icecube origin, by definition")
    .def_readonly("zIceTop", I3Constants::zIceTop, " Z-coordinate of IceTop (Origin Depth)"
		  "Obtained from DEFINED elevation of origin and MEASURED surface elevation")
    .def_readonly("Coordinate_shift_x", I3Constants::Coordinate_shift_x, "conversion between icecube and amanda")
    .def_readonly("Coordinate_shift_y", I3Constants::Coordinate_shift_y, "conversion between icecube and amanda")
    .def_readonly("Coordinate_shift_z", I3Constants::Coordinate_shift_z, "conversion between icecube and amanda")
    .def_readonly("dt_window_l", I3Constants::dt_window_l, "default value for time residuals")
    .def_readonly("dt_window_h", I3Constants::dt_window_h, "default value for time residuals")
    .def( freeze() )
    ;
}
