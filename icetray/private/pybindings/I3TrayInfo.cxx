 //
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id: ithon.cxx 25598 2006-11-25 02:52:57Z troy $
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

#include <icetray/I3TrayInfo.h>

using namespace boost::python;

void register_I3TrayInfo()
{
  class_<I3TrayInfo, bases<I3FrameObject>, boost::shared_ptr<I3TrayInfo> >("I3TrayInfo")
//    .property("modules_in_order", &I3TrayInfo::modules_in_order)
//    .property("factories_in_order", &I3TrayInfo::factories_in_order)
//    .property("loaded_libs", &I3TrayInfo::loaded_libs)
    .def(self_ns::str(self))
    ;

  register_pointer_conversions<I3TrayInfo>();
}
