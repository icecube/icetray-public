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

#include <vector>

#include <dataclasses/physics/I3EventHeader.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/ostream_overloads.hpp>

using namespace boost::python;

void register_I3EventHeader()
{
  {
    scope event_header_scope = 
      class_<I3EventHeader, bases<I3FrameObject>, boost::shared_ptr<I3EventHeader> >("I3EventHeader")
      PROPERTY(I3EventHeader, run_id,           RunID)
      PROPERTY(I3EventHeader, sub_run_id,       SubRunID)
      PROPERTY(I3EventHeader, event_id,         EventID)
      PROPERTY(I3EventHeader, sub_event_id,     SubEventID)
      PROPERTY(I3EventHeader, sub_event_stream, SubEventStream)
      PROPERTY(I3EventHeader, state,            State)
      PROPERTY(I3EventHeader, start_time,       StartTime)
      PROPERTY(I3EventHeader, end_time,         EndTime)
      .add_property("data_stream", &I3EventHeader::GetDataStream)
      .def(dataclass_suite<I3EventHeader>())
      ;

    register_pointer_conversions<I3EventHeader>();

    enum_<I3EventHeader::State>("StateType")
      .value("UNKNOWN_STATE"       ,I3EventHeader::UNKNOWN_STATE)
      .value("OK"                  ,I3EventHeader::OK)
      .value("CONFIG_IN_TRANSITION",I3EventHeader::CONFIG_IN_TRANSITION)
      .export_values()
      ;
  }
}
