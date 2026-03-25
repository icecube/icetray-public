/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Pybindings for I3mDOMCal and I3mDOMCalMap Classes
 *
 * @file I3mDOMCal.cxx
 * @date 2026-1-21
 * @author lbloom12
 *
 */


#include "pybindings.hpp"
#include <dataclasses/calibration/I3mDOMCal.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;


// Create pybindings for I3mDOMCal...
void register_I3mDOMCal()
{
  // ...to read/write all non-constant values
  class_<I3mDOMCal, boost::shared_ptr<I3mDOMCal> >("I3mDOMCal")
  #define I3MDOMCAL_VALS  (linearityParams)(hvGainRelation)      \
                          (pmtTransitTime)(pmtTransitTimeSpread) \
                          (adcBaselineValue)(adcBaselineDAC)     \
                          (discThreshold)(discDAC)
  BOOST_PP_SEQ_FOR_EACH(WRAP_RW_RECASE, I3mDOMCal, I3MDOMCAL_VALS)
  #undef I3MDOMCAL_VALS

  // ...to only read constant values
  #define CONST_VALS (adcToVolts)(sampleRate)(frontEndImpedance) \
                     (mdomTimeOffset)(discSampleRate)
  BOOST_PP_SEQ_FOR_EACH(WRAP_RO, I3mDOMCal, CONST_VALS)
  #undef CONST_VALS

  // expose the function 'GetBaselineValue' which returns the
  // baseline value if given a DAC value that matches the calibration
  .def("GetValidBaselineValue", &I3mDOMCal::GetValidBaselineValue)

  // expose the function 'GetDiscThreshold' which returns the
  // discriminator threshold if given a DAC value that matches the calibration
  .def("GetValidDiscThreshold", &I3mDOMCal::GetValidDiscThreshold)

  .def(dataclass_suite<I3mDOMCal>())
  ;
}


// Create pybindings for I3mDOMCalMap
void register_I3mDOMCalMap()
{
  class_<I3mDOMCalMap, bases<I3FrameObject> >("I3mDOMCalMap")
  .def(dataclass_suite<I3mDOMCalMap>())
  ;
}
