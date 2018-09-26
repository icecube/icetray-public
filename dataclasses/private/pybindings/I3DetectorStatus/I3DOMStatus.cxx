//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id$
//
//   This file is part of IceTray.
//
//   IceTray is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
//   IceTray is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
      (nBinsATWD2)(nBinsATWD3)(nBinsFADC)
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
