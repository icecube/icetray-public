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

#include <icetray/I3PhysicsTimer.h>
#include <icetray/python/boost_serializable_pickle_suite.hpp>

using namespace boost::python;

void register_I3RUsage()
{
  class_<I3RUsage, bases<I3FrameObject>, boost::shared_ptr<I3RUsage> >("I3RUsage")
    .def_readwrite("SystemTime",&I3RUsage::systemtime)
    .def_readwrite("UserTime",&I3RUsage::usertime)
    .def_readwrite("WallClockTime", &I3RUsage::wallclocktime)
    .def_pickle(boost_serializable_pickle_suite<I3RUsage>())
    ;

  register_pointer_conversions<I3RUsage>();
}
