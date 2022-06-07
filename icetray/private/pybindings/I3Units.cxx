/**
 *  Copyright (C) 2009
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *  
 *  SPDX-License-Identifier: BSD-2-Clause
 *  
 */

#include <boost/python.hpp>
#include <boost/preprocessor.hpp>

#include <icetray/I3Units.h>

//
// these correspond to I3UNITS in icetray/I3Units.h.
//
#define I3_UNITS (meter)						\
    (meter2)								\
    (meter3)								\
    (millimeter)							\
    (millimeter2)							\
    (millimeter3)							\
    (centimeter)							\
    (centimeter2)							\
    (centimeter3)							\
    (kilometer)								\
    (kilometer2)							\
    (kilometer3)							\
    (parsec)								\
    (micrometer)							\
    (nanometer)								\
    (angstrom)								\
    (fermi)								\
    (barn)								\
    (millibarn)								\
    (microbarn)								\
    (nanobarn)								\
    (picobarn)								\
    (feet)								\
    (mm)								\
    (mm2)								\
    (mm3)								\
    (cm)								\
    (cm2)								\
    (cm3)								\
    (m)									\
    (m2)								\
    (m3)								\
    (km)								\
    (km2)								\
    (km3)								\
    (pc)								\
    (ft)								\
    (radian)								\
    (milliradian)							\
    (degree)								\
    (steradian)								\
    (rad)								\
    (mrad)								\
    (sr)								\
    (deg)								\
    (second)								\
    (millisecond)							\
    (microsecond)							\
    (nanosecond)							\
    (picosecond)							\
    (minute)								\
    (hour)								\
    (day)								\
    (gregorianyear)							\
    (julianyear)							\
    (hertz)								\
    (kilohertz)								\
    (megahertz)								\
    (ns)								\
    (s)									\
    (ms)								\
    (electronvolt)							\
    (kiloelectronvolt)							\
    (megaelectronvolt)							\
    (gigaelectronvolt)							\
    (teraelectronvolt)							\
    (petaelectronvolt)							\
    (MeV)								\
    (eV)								\
    (keV)								\
    (GeV)								\
    (TeV)								\
    (PeV)								\
    (eplus)								\
    (eSI)								\
    (coulomb)								\
    (C)									\
    (picocoulomb)							\
    (pC)								\
    (joule)								\
    (ampere)								\
    (A)									\
    (milliampere)							\
    (mA)								\
    (nanoampere)							\
    (nA)								\
    (microvolt)								\
    (millivolt) (mV)							\
    (volt) (V)								\
    (kilovolt) (kV)							\
    (statvolt)                              \
    (watt)                                  \
    (milliwatt) (mW)                        \
    (ohm)								\
    (kelvin)								\
    (kilogram)(kg)							\
    (gram)(g)								\
    (milligram)(mg)							\
    (newton)								\
    (pascal)								\
    (bar)								\
    (millipascal)							\
    (micropascal)							\
    (nanopascal)							\
    (tesla) (Tesla)                         \
    (millitesla) (mT)                       \
    (microtesla) (uT)                       \
    (nanotesla) (nT)                        \
    (perCent)								\
    (perThousand)							\
    (perMillion)							\



#define I3_UNITS_INTERFACE(r,data,T) \
  static double BOOST_PP_CAT(i3units_return_,T)() { return I3Units::T; }


BOOST_PP_SEQ_FOR_EACH(I3_UNITS_INTERFACE,~,I3_UNITS)

#define I3_UNITS_DEF(r,data,T)						\
  .add_static_property(BOOST_PP_STRINGIZE(T),				\
		       &BOOST_PP_CAT(i3units_return_, T))


using namespace boost::python;


struct __i3units__ { };

void register_I3Units()
{
  // inserts a toplevel "def" command for each unit in I3_UNITS, above.
  // glue to these functions is provided in I3Units.py

  class_<__i3units__, boost::noncopyable>
    ("I3Units", 
     "'Namespace' holding values for I3Units",
     no_init)
    BOOST_PP_SEQ_FOR_EACH(I3_UNITS_DEF,~,I3_UNITS);
    ;

}


