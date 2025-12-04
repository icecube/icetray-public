/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Serialization and Printing for I3mDOMStatus and I3mDOMStatusMap Classes
 *
 * @file I3mDOMStatus.cxx
 * @date 2025-10-14
 * @author lbloom12
 *
 */


#include <dataclasses/status/I3mDOMStatus.h>
#include <icetray/python/dataclass_suite.hpp>


using namespace boost::python;

void register_I3mDOMStatus()
{
  {
    scope outer =
      class_<I3mDOMStatus, boost::shared_ptr<I3mDOMStatus> >("I3mDOMStatus")
      #define MDOMSTATUSPROPERTIES  (enabled)(trigMode)(adcThreshold)	  \
                                    (preSamples)(postSamples)(pmtHV)  \
                                    (adcBaselineDAC)(discDAC)
      BOOST_PP_SEQ_FOR_EACH(WRAP_RW_RECASE, I3mDOMStatus, MDOMSTATUSPROPERTIES)
      #undef MDOMSTATUSPROPERTIES
      .def(dataclass_suite<I3mDOMStatus>())
      ;

    enum_<I3mDOMStatus::mDOMPMTTrigMode>("mDOMPMTTrigMode")
      .value("UnknownTrigMode", I3mDOMStatus::UnknownTrigMode)
      .value("ADC", I3mDOMStatus::ADC)
      .value("Disc", I3mDOMStatus::Disc)
      .export_values()
      ;
    def("identity", identity_<I3mDOMStatus::mDOMPMTTrigMode>);
  }
}



// Create pybindings for I3mDOMStatusMap
void register_I3mDOMStatusMap()
{
  class_<I3mDOMStatusMap, bases<I3FrameObject> >("I3mDOMStatusMap")
  .def(dataclass_suite<I3mDOMStatusMap>())
  ;
}
