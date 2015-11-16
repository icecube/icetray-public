//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id: I3FrameMixing.cxx 2262 2015-08-27 20:41:44Z olivas $
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

#include <icetray/I3FrameMixing.h>

using namespace boost::python;

void register_I3FrameMixing()
{
  class_<I3FrameMixer >
    ("I3FrameMixer","Logic for mixing I3Frames",init<>() )
    .def(init<bool>(args("track_order"), "Initialize with tracking of frame ordering."))
    .def("mix", &I3FrameMixer::Mix,
        "Mix keys from previous frames into current frame and update cache.")
    .def("get_mixed_frames", &I3FrameMixer::GetMixedFrames,args("stop"),
        "Get the most recent frames that mix into a frame of the given stream.")
    .add_property("track_order",
        (bool (I3FrameMixer::*)(void) const)&I3FrameMixer::TrackOrder,
        (void (I3FrameMixer::*)(bool))&I3FrameMixer::TrackOrder,
        "Turn frame order tracking on or off.")
    .def("reset",&I3FrameMixer::Reset,"Reset the cache.")
    ;
}

