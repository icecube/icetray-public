/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Pybindings for I3DEggCal and I3DEggCalMap Classes
 *
 * @file I3DEggCal.cxx
 * @date 2026-4-30
 * @author lbloom12
 *
 */


#include "pybindings.hpp"
#include <dataclasses/calibration/I3DEggCal.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;



// Create pybindings for I3DEggCal...
void register_I3DEggCal()
{
  class_<I3DEggCal, boost::shared_ptr<I3DEggCal> >("I3DEggCal")

  // ...to read/write all non-constant values
  #define I3DEGGCAL_VALS (hvGainRelation)(pmtTransitTime)              \
                         (pmtTransitTimeSpread)(adcBaselineRMS)        \
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
