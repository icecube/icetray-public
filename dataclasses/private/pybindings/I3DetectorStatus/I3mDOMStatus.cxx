/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Serialization and Printing for I3mDOMStatus and I3mDOMStatusMap Classes
 *
 * @file I3mDOMStatus.cxx
 * @date 2026-03-26
 * @author lbloom12
 *
 */


#include <dataclasses/status/I3mDOMStatus.h>
#include <icetray/python/dataclass_suite.hpp>


using namespace boost::python;

// Create pybindings to read/write all values in I3DEggStatus
void register_I3mDOMStatus()
{
  scope outer =
    class_<I3mDOMStatus, boost::shared_ptr<I3mDOMStatus> >("I3mDOMStatus")
    #define MDOMSTATUSPROPERTIES  (enabled)(trigMode)(adcThreshold)      \
                                  (readoutMode)(preSamples)(postSamples) \
                                  (pmtHV)(adcBaselineDAC)(discDAC)
    BOOST_PP_SEQ_FOR_EACH(WRAP_RW_RECASE, I3mDOMStatus, MDOMSTATUSPROPERTIES)
    #undef MDOMSTATUSPROPERTIES
    .def(dataclass_suite<I3mDOMStatus>())
    ;

    
  // define the mDOMPMTTrigMode enum
  enum_<I3mDOMStatus::mDOMPMTTrigMode>("mDOMPMTTrigMode")
    .value("UnknownTrigMode", I3mDOMStatus::UnknownTrigMode)
    .value("ADC", I3mDOMStatus::ADC)
    .value("Disc", I3mDOMStatus::Disc)
    .export_values()
    ;
  def("identity", identity_<I3mDOMStatus::mDOMPMTTrigMode>);

    
  // define the mDOMReadoutMode enum
  enum_<I3mDOMStatus::mDOMReadoutMode>("mDOMReadoutMode")
    .value("UnknownReadoutMode", I3mDOMStatus::UnknownReadoutMode)
    .value("FIXED_LENGTH", I3mDOMStatus::FIXED_LENGTH)
    .value("VARIABLE_LENGTH", I3mDOMStatus::VARIABLE_LENGTH)
    .export_values()
    ;
  def("identity", identity_<I3mDOMStatus::mDOMReadoutMode>);
}



// Create pybindings for I3mDOMStatusMap
void register_I3mDOMStatusMap()
{
  class_<I3mDOMStatusMap, bases<I3FrameObject> >("I3mDOMStatusMap")
  .def(dataclass_suite<I3mDOMStatusMap>())
  ;
}
