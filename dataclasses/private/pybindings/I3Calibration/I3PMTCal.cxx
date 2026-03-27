/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Pybindings for I3PMTCal and I3PMTCalMap Classes
 *
 * @file I3PMTCal.cxx
 * @date 2026-03-26
 * @author lbloom12
 *
 */


#include "pybindings.hpp"
#include <dataclasses/calibration/I3PMTCal.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;


// Create pybindings to read/write all values in I3PMTCal
void register_I3PMTCal()
{
  class_<I3PMTCal, boost::shared_ptr<I3PMTCal> >("I3PMTCal")
  #define I3PMTCAL_VALS (relativePmtEff)(noiseRate)(speChargeDist)
  BOOST_PP_SEQ_FOR_EACH(WRAP_RW_RECASE, I3PMTCal, I3PMTCAL_VALS)
  #undef I3PMTCAL_VALS
  .def(dataclass_suite<I3PMTCal>())
  ;
}


// Create pybindings for I3PMTCalMap
void register_I3PMTCalMap()
{
  class_<I3PMTCalMap, bases<I3FrameObject> >("I3PMTCalMap")
  .def(dataclass_suite<I3PMTCalMap>())
  ;
}
