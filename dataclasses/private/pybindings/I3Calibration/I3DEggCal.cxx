/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Pybindings for LinearityParameters,
 * I3DEggCal, and I3DEggCalMap Classes
 *
 * @file I3DEggCal.cxx
 * @date 2026-3-27
 * @author lbloom12
 *
 */


#include "pybindings.hpp"
#include <dataclasses/calibration/I3DEggCal.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;


// Create pybindings to read/write all the linearity parameters
void register_LinearityParameters()
{
  class_<LinearityParameters>("LinearityParameters")
    .def_readwrite("p0", &LinearityParameters::p0)
    .def_readwrite("p1", &LinearityParameters::p1)
    .def_readwrite("p2", &LinearityParameters::p2)
    .def(dataclass_suite<LinearityParameters>())
    ;
}




// Create pybindings for I3DEggCal...
void register_I3DEggCal()
{
  class_<I3DEggCal, boost::shared_ptr<I3DEggCal> >("I3DEggCal")

  // ...to read/write all non-constant values
  #define I3DEGGCAL_VALS (linearityParams)(hvGainRelation)(pmtTransitTime) \
                         (pmtTransitTimeSpread)(adcBaselineRMS)            \
                         (dacBaselineRelation)(temperature)(tauParams)
  BOOST_PP_SEQ_FOR_EACH(WRAP_RW_RECASE, I3DEggCal, I3DEGGCAL_VALS)
  #undef I3DEGGCAL_VALS

  // ...to only read constant values
  #define CONST_VALS (adcToVolts)(sampleRate)(frontEndImpedance)(deggTimeOffset)
  BOOST_PP_SEQ_FOR_EACH(WRAP_RO, I3DEggCal, CONST_VALS)
  #undef CONST_VALS

  .def(dataclass_suite<I3DEggCal>())
  ;
}



// Create pybindings for I3DEggCalMap
void register_I3DEggCalMap()
{
  class_<I3DEggCalMap, bases<I3FrameObject> >("I3DEggCalMap")
    .def(dataclass_suite<I3DEggCalMap>())
    ;
}
