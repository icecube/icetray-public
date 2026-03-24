//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   This file is part of IceTray.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//   
//   THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
//   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
//   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
//   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
//   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//   SUCH DAMAGE.
//   
//   SPDX-License-Identifier: BSD-2-Clause
//   
//

#include "pybindings.hpp"
#include <boost/preprocessor.hpp>

#include <dataclasses/I3Constants.h>

using namespace boost::python;

#define I3_CONSTANT_NAMES                  \
  (c)(z_vacuum)(KBoltzmann)                \
  (h)(hbar)(hbarc)                         \
  (pi)(e)(NA)                              \
  (n_ice_phase)(n_ice_group)(n_ice)        \
  (theta_cherenkov)(c_ice)                 \
  (SurfaceElev)(OriginElev)(zIceTop)       \
  (earthRadius)                            \
  (Coordinate_shift_x)(Coordinate_shift_y) \
  (Coordinate_shift_z)                     \
  (dt_window_l)(dt_window_h)               \
  (dedx_a)(dedx_b)                         \
  (G_Fermi)(epsilon_0)(mu_0)               \
  (alpha)(sinsq_theta_W)                   \
  (cos_theta_W)(sin_theta_C)               \
  (cos_theta_C)(ref_wavelength)

#define I3CONSTANT_DEF(r,data,t) \
  .def_readonly(BOOST_PP_STRINGIZE(t), BOOST_PP_CAT(I3Constants::, t))

// dummy class just used as a namespace.
namespace {
  struct icecube_dataclasses_I3Constants { };
}

void register_I3Constants()
{
  class_<icecube_dataclasses_I3Constants>("I3Constants")
    .def_readonly("c", I3Constants::c, "Speed of light in vacuum")
    .def_readonly("z_vacuum", I3Constants::z_vacuum, "Impedance of free space")
    .def_readonly("KBoltzmann", I3Constants::KBoltzmann, "Boltzmann's constant")
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
    .def_readonly("earthRadius", I3Constants::earthRadius, "earth radius in IceCube units")
    .def_readonly("Coordinate_shift_x", I3Constants::Coordinate_shift_x, "conversion between icecube and amanda")
    .def_readonly("Coordinate_shift_y", I3Constants::Coordinate_shift_y, "conversion between icecube and amanda")
    .def_readonly("Coordinate_shift_z", I3Constants::Coordinate_shift_z, "conversion between icecube and amanda")
    .def_readonly("dt_window_l", I3Constants::dt_window_l, "default value for time residuals")
    .def_readonly("dt_window_h", I3Constants::dt_window_h, "default value for time residuals")
    .def_readonly("dedx_a", I3Constants::dedx_a, "constant muon dedx in ice [GeV/meter water equivalent]")
    .def_readonly("dedx_b", I3Constants::dedx_b, "enegy-dependent muon dedx term in ice [GeV/meter water equivalent]")
    .def_readonly("h", I3Constants::h, "Planck Constant")
    .def_readonly("hbar", I3Constants::hbar, "reduced Planck Constant")
    .def_readonly("hbarc", I3Constants::hbarc, "Unit conversion")
    .def_readonly("G_Fermi", I3Constants::G_Fermi, "Fermi constant, / (hbar * c)^3 is assumed")
    .def_readonly("epsilon_0", I3Constants::epsilon_0, "Permittivity of free space")
    .def_readonly("mu_0", I3Constants::mu_0, "Permeability of free space")
    .def_readonly("alpha", I3Constants::alpha, "Fine-structure constant")
    .def_readonly("sinsq_theta_W", I3Constants::sinsq_theta_W, "Weak-mixing Angle sin^{2} theta (M_{Z})")
    .def_readonly("cos_theta_W", I3Constants::cos_theta_W, "Weak-mixing Angle cos theta_{W}")
    .def_readonly("sin_theta_C", I3Constants::sin_theta_C, "Cabibbo Angle sin_theta_C")
    .def_readonly("cos_theta_C", I3Constants::cos_theta_C, "Cabibbo Angle cos_theta_C")
    .def_readonly("ref_wavelength", I3Constants::ref_wavelength, "Reference wavelength of Cherenkov photon")
    .def( freeze() )
    ;
}
