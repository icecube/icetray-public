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
