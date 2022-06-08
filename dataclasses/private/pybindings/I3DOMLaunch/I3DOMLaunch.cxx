//
//   Copyright (c) 2004, 2005, 2006, 2007, 2008   Troy D. Straszheim  
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

#include <vector>

#include <dataclasses/physics/I3DOMLaunch.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/ostream_overloads.hpp>

using namespace boost::python;

void register_I3DOMLaunch()
{

  std::vector<int>& (I3DOMLaunch::*get_raw_fadc)(void) = &I3DOMLaunch::GetRawFADC;
  object get_raw_fadc_func = make_function(get_raw_fadc, return_internal_reference<>());
  
  std::vector< std::vector<int> >& (I3DOMLaunch::*get_raw_atwds)(void) = &I3DOMLaunch::GetRawATWDs;
  object get_raw_atwds_func = make_function(get_raw_atwds, return_internal_reference<>());

  std::vector<int>& (I3DOMLaunch::*get_raw_charge_stamp)(void) = &I3DOMLaunch::GetRawChargeStamp;
  object get_raw_charge_stamp_func = make_function(get_raw_charge_stamp, return_internal_reference<>());

  {
    scope outer = 
    class_<I3DOMLaunch, boost::shared_ptr<I3DOMLaunch> >("I3DOMLaunch")
      .add_property("time", &I3DOMLaunch::GetStartTime, &I3DOMLaunch::SetStartTime)
      .add_property("lc_bit", &I3DOMLaunch::GetLCBit, &I3DOMLaunch::SetLCBit)
      .add_property("trigger_type", &I3DOMLaunch::GetTriggerType, &I3DOMLaunch::SetTriggerType)
      .add_property("trigger_mode", &I3DOMLaunch::GetTriggerMode, &I3DOMLaunch::SetTriggerMode)
      .add_property("which_atwd", &I3DOMLaunch::GetWhichATWD, &I3DOMLaunch::SetWhichATWD)
      .add_property("is_pedestal_sub", &I3DOMLaunch::GetIsPedestalSub, &I3DOMLaunch::SetIsPedestalSub)
      .add_property("charge_stamp_highest_sample", 
		    &I3DOMLaunch::GetChargeStampHighestSample, &I3DOMLaunch::SetChargeStampHighestSample)
      .add_property("raw_fadc", get_raw_fadc_func, &I3DOMLaunch::SetRawFADC)
      .add_property("raw_atwd", get_raw_atwds_func, get_raw_atwds_func)
      .add_property("raw_charge_stamp", get_raw_charge_stamp_func, get_raw_charge_stamp_func)
      .add_property("raw_atwd_charge_stamp",
        &I3DOMLaunch::GetRawATWDChargeStamp, &I3DOMLaunch::SetRawATWDChargeStamp)
      .add_property("which_atwd_charge_stamp", 
		    &I3DOMLaunch::GetWhichATWDChargeStamp, &I3DOMLaunch::SetWhichATWDChargeStamp)
      .def(self == self)
      .def(dataclass_suite<I3DOMLaunch>())
      ;

    enum_<I3DOMLaunch::TriggerType>("TriggerType")
      .value("TEST_PATTERN",I3DOMLaunch::TEST_PATTERN)
      .value("CPU_REQUESTED",I3DOMLaunch::CPU_REQUESTED)
      .value("SPE_DISCRIMINATOR_TRIGGER",I3DOMLaunch::SPE_DISCRIMINATOR_TRIGGER)
      .value("FLASHER_BOARD_TRIGGER",I3DOMLaunch::FLASHER_BOARD_TRIGGER)
      .value("MPE_DISCRIMINATOR_TRIGGER",I3DOMLaunch::MPE_DISCRIMINATOR_TRIGGER)
      .value("ONBOARD_LED",I3DOMLaunch::ONBOARD_LED)
      .export_values()
      ;
    def("identity", identity_<I3DOMLaunch::TriggerType>);
  
    enum_<I3DOMLaunch::TriggerMode>("TriggerMode")
      .value("UNDEFINED", I3DOMLaunch::UNDEFINED)
      .value("REQUIRE_BOTH", I3DOMLaunch::REQUIRE_BOTH)
      .value("FR_RUN", I3DOMLaunch::FR_RUN)
      .value("LC_LOWER", I3DOMLaunch::LC_LOWER)
      .value("LC_UPPER", I3DOMLaunch::LC_UPPER)
      .value("UNKNOWN_MODE", I3DOMLaunch::UNKNOWN_MODE)
      .value("SLC_READOUT", I3DOMLaunch::SLC_READOUT)
      .value("MIN_BIAS", I3DOMLaunch::MIN_BIAS)
      .value("LAST_TRIGGER_SITUATION", I3DOMLaunch::LAST_TRIGGER_SITUATION)
      .export_values()
      ;
  
    def("Identity", identity_<I3DOMLaunch::TriggerMode>);

    enum_<I3DOMLaunch::ATWDselect>("ATWDselect")
      .value("ATWDa", I3DOMLaunch::ATWDa)
      .value("ATWDb", I3DOMLaunch::ATWDb)
      .export_values()
      ;
    def("Identity", identity_<I3DOMLaunch::ATWDselect>);
  }

}
