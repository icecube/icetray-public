//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id$
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

// This file registers both the I3VEMCalibration and I3IceTopSLCCalibration objects.
// (The "IceToppy stuff")

#include <dataclasses/calibration/I3VEMCalibration.h>
#include <dataclasses/calibration/I3IceTopSLCCalibration.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

void register_I3VEMCalibration()
{
  class_<I3VEMCalibration>("I3VEMCalibration")
    .def(copy_suite<I3VEMCalibration>())
    #define I3VEMCALPROPS (pePerVEM)(muPeakWidth)(hglgCrossOver)(corrFactor) 
    BOOST_PP_SEQ_FOR_EACH(WRAP_RW_RECASE, I3VEMCalibration, I3VEMCALPROPS)
    #undef I3VEMCALPROPS
    .def(dataclass_suite<I3VEMCalibration>())
    ;

  class_<I3VEMCalibrationMap, 
         I3VEMCalibrationMapPtr>("I3VEMCalibrationMap")
    .def(dataclass_suite<I3VEMCalibrationMap>())
    ;

}

void register_I3IceTopSLCCalibration()
{
  class_<I3IceTopSLCCalibration>("I3IceTopSLCCalibration")
    .def(copy_suite<I3IceTopSLCCalibration>())
    #define I3ITSLCCALPROPS \
    (intercept_C0A0)(intercept_C0A1)(intercept_C0A2)(intercept_C1A0)(intercept_C1A1)(intercept_C1A2) \
    (slope_C0A0)(slope_C0A1)(slope_C0A2)(slope_C1A0)(slope_C1A1)(slope_C1A2) \
    (intercept_CunkA0)(intercept_CunkA1)(intercept_CunkA2) \
    (slope_CunkA0)(slope_CunkA1)(slope_CunkA2) \
    (A0_A1_crossover)(A1_A2_crossover)
    BOOST_PP_SEQ_FOR_EACH(WRAP_RW, I3IceTopSLCCalibration, I3ITSLCCALPROPS)
    #undef I3ITSLCCALPROPS
    .def("GetIntercept",&I3IceTopSLCCalibration::GetIntercept,bp::args("chip", "atwd"))
    .def("SetIntercept",&I3IceTopSLCCalibration::SetIntercept,bp::args("chip", "atwd", "val"))
    .def("GetSlope",&I3IceTopSLCCalibration::GetSlope,bp::args("chip", "atwd"))
    .def("SetSlope",&I3IceTopSLCCalibration::SetSlope,bp::args("chip", "atwd", "val"))
    .def(dataclass_suite<I3IceTopSLCCalibration>())
    ;

  class_<I3IceTopSLCCalibrationMap,
         I3IceTopSLCCalibrationMapPtr>("I3IceTopSLCCalibrationMap")
    .def(dataclass_suite<I3IceTopSLCCalibrationMap>())
    ;

}
