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

#include <dataclasses/I3Map.h>
#include <dataclasses/status/I3TriggerStatus.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

void register_I3TriggerStatus()
{
  {
    scope outer = 
      class_<I3TriggerStatus, boost::shared_ptr<I3TriggerStatus> >("I3TriggerStatus")
      .def_readwrite("trigger_name", &I3TriggerStatus::name_)
      .def_readwrite("trigger_settings",&I3TriggerStatus::settings_)
      .def_readwrite("readout_settings",&I3TriggerStatus::readoutconfigs_)
      .def(dataclass_suite<I3TriggerStatus>())
      ;

    class_<I3TriggerReadoutConfig, boost::shared_ptr<I3TriggerReadoutConfig> >("I3TriggerReadoutConfig")
      .def_readwrite("readout_time_minus", &I3TriggerReadoutConfig::readoutTimeMinus)
      .def_readwrite("readout_time_plus", &I3TriggerReadoutConfig::readoutTimePlus)
      .def_readwrite("readout_time_offset", &I3TriggerReadoutConfig::readoutTimeOffset)
      .def(dataclass_suite<I3TriggerReadoutConfig>())
      ;

    enum_<I3TriggerStatus::Subdetector>("Subdetector")
      .value("NOT_SPECIFIED",I3TriggerStatus::NOT_SPECIFIED)
      .value("ALL",I3TriggerStatus::ALL)
      .value("ICETOP",I3TriggerStatus::ICETOP)
      .value("INICE",I3TriggerStatus::INICE)
      .export_values()
      ;
    def("identity", identity_<I3TriggerStatus::Subdetector>);

  }

  class_<I3TriggerStatusMap, 
         I3TriggerStatusMapPtr>("I3TriggerStatusMap")
    .def(dataclass_suite<I3TriggerStatusMap>())
    ;

}
