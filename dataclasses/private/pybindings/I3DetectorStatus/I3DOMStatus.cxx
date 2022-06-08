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

#include <dataclasses/status/I3DOMStatus.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

void register_I3DOMStatus()
{
  {
    scope outer = 
      class_<I3DOMStatus, boost::shared_ptr<I3DOMStatus> >("I3DOMStatus")
      #define DOMSTATUSPROPERTIES (trigMode)(lcMode)(txMode)(lcWindowPre)(lcWindowPost)(lcSpan) \
      (statusFADC)(pmtHV)(speThreshold)(fePedestal)(dacTriggerBias0)(dacTriggerBias1)(dacFADCRef) \
      (deltaCompress)(domGainType)(cableType)(SLCActive)(mpeThreshold)(nBinsATWD0)(nBinsATWD1) \
      (nBinsATWD2)(nBinsATWD3)(nBinsFADC)(deltaCompress)(fbState) \
      (fbBrightness)(fbLength)(fbDelay)(fbMask)(fbRate)
      BOOST_PP_SEQ_FOR_EACH(WRAP_RW_RECASE, I3DOMStatus, DOMSTATUSPROPERTIES)
      #undef DOMSTATUSPROPERTIES
      // If we used snake_case, these two would end up as status_atw_da
      .def_readwrite("status_atwd_a", &I3DOMStatus::statusATWDa)
      .def_readwrite("status_atwd_b", &I3DOMStatus::statusATWDb)
      .def(dataclass_suite<I3DOMStatus>())
      ;

    enum_<I3DOMStatus::TrigMode>("TrigMode")
      .value("UnknownTrigMode",I3DOMStatus::UnknownTrigMode)
      .value("TestPattern",I3DOMStatus::TestPattern)
      .value("CPU",I3DOMStatus::CPU)
      .value("SPE",I3DOMStatus::SPE)
      .value("Flasher",I3DOMStatus::Flasher)
      .value("MPE",I3DOMStatus::MPE)
      .export_values()
      ;
    def("identity", identity_<I3DOMStatus::TrigMode>);

    enum_<I3DOMStatus::LCMode>("LCMode")
      .value("UnknownLCMode",I3DOMStatus::UnknownLCMode)
      .value("LCOff",I3DOMStatus::LCOff)
      .value("UpOrDown",I3DOMStatus::UpOrDown)
      .value("Up",I3DOMStatus::Up)
      .value("Down",I3DOMStatus::Down)
      .value("UpAndDown",I3DOMStatus::UpAndDown)
      .value("SoftLC",I3DOMStatus::SoftLC)
      .export_values()
      ;
    def("identity", identity_<I3DOMStatus::LCMode>);

    enum_<I3DOMStatus::OnOff>("OnOff")
      .value("Unknown",I3DOMStatus::Unknown)
      .value("Off",I3DOMStatus::Off)
      .value("On",I3DOMStatus::On)
      .export_values()
      ;
    def("identity", identity_<I3DOMStatus::OnOff>);

    enum_<I3DOMStatus::DOMGain>("DOMGain")
      .value("UnknownGainType",I3DOMStatus::UnknownGainType)
      .value("High",I3DOMStatus::High)
      .value("Low",I3DOMStatus::Low)
      .export_values()
      ;
    def("identity", identity_<I3DOMStatus::DOMGain>);

    enum_<I3DOMStatus::CableType>("CableType")
      .value("UnknownCableType",I3DOMStatus::UnknownCableType)
      .value("Terminated",I3DOMStatus::Terminated)
      .value("Unterminated",I3DOMStatus::Unterminated)
      .export_values()
      ;

    def("identity", identity_<I3DOMStatus::CableType>);

  }

  class_<I3DOMStatusMap, 
         I3DOMStatusMapPtr>("I3DOMStatusMap")
    .def(dataclass_suite<I3DOMStatusMap>())
    ;

}
