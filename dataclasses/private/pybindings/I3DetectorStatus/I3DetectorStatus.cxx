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

#include <dataclasses/status/I3DetectorStatus.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

void register_I3DetectorStatus()
{
  class_<I3DetectorStatus, bases<I3FrameObject>, boost::shared_ptr<I3DetectorStatus> >("I3DetectorStatus")
    #define DETECTORSTATUSPROPS (startTime)(endTime)(domStatus)(triggerStatus)(daqConfigurationName)
    BOOST_PP_SEQ_FOR_EACH(WRAP_RW_RECASE, I3DetectorStatus, DETECTORSTATUSPROPS )
    #undef DETECTORSTATUSPROPS
    .def(dataclass_suite<I3DetectorStatus>())
    ;

  register_pointer_conversions<I3DetectorStatus>();
}
