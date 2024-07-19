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

#include <dataclasses/calibration/I3Calibration.h>
#include <dataclasses/calibration/I3IceTopSLCCalibration.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

void register_I3Calibration()
{
  class_<I3Calibration, bases<I3FrameObject>, boost::shared_ptr<I3Calibration> >("I3Calibration")
    .def(copy_suite<I3Calibration>())
    #define I3CALPROPS (startTime)(endTime)(domCal)(vemCal)
    BOOST_PP_SEQ_FOR_EACH(WRAP_RW_RECASE, I3Calibration, I3CALPROPS)
    #undef I3CALPROPS
    .def(dataclass_suite<I3Calibration>())
    ;

  register_pointer_conversions<I3Calibration>();
}

#ifndef ENUM_DEF
#define ENUM_DEF(r,data,T) .value(BOOST_PP_STRINGIZE(T), data::T)
#endif

void register_I3IceTopSLCCalibrationCollection()
{
  class_<I3IceTopSLCCalibrationCollection, bases<I3FrameObject>, boost::shared_ptr<I3IceTopSLCCalibrationCollection> >("I3IceTopSLCCalibrationCollection")
    .def(copy_suite<I3IceTopSLCCalibrationCollection>())
    // Because of weird capitalizations, we'll define these by hand rather than using a loop:
    .def_readwrite("start_run", &I3IceTopSLCCalibrationCollection::startRun)
    .def_readwrite("end_run", &I3IceTopSLCCalibrationCollection::endRun)
    .def_readwrite("it_slc_cal", &I3IceTopSLCCalibrationCollection::ITslcCal)
    .def_readwrite("provenance", &I3IceTopSLCCalibrationCollection::itSlcCalProvenance)
    .def(dataclass_suite<I3IceTopSLCCalibrationCollection>())
    ;

  // Get access to the Provenance Enums via pybindings.
  enum_<I3IceTopSLCCalibrationCollection::ITSLCCalProvenance>("ITSLCCalProvenance")
    BOOST_PP_SEQ_FOR_EACH(ENUM_DEF,I3IceTopSLCCalibrationCollection,I3ICETOPSLCCALIBRATION_H_I3IceTopSLCCalibrationCollection_ITSLCCalProvenance)
    .export_values()
    ;

  register_pointer_conversions<I3IceTopSLCCalibrationCollection>();
}
