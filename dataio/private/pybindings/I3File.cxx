//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id: I3File.cxx 165886 2018-10-01 14:37:58Z nwhitehorn $
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

#include <dataio/I3File.h>

using namespace boost::python;
namespace bp = boost::python;

void register_I3File()
{
  // this isn't used... I3LootFile is.
  // 
#if 0
  class_<I3File, boost::noncopyable>
    ("I3File", "A low-level interface to .i3 files. "
     "Prefer I3LootFile if you are doing analysis ")
    .def("open_file", 
	 i3file_open_file, 
	 bp::arg("filename"), 
	 "Open and scan a .i3 or .i3.gz file.")
    .def("size", &I3File::size,
	 "Return number of frames in file")
    .def("get_frame", &I3File::get_frame,
	 bp::arg("index"), 
	 "Get frame number 'index', including appropriate "
	 "Geometry/Calibration/DetectorStatus from preceding frames ")
    .def("get_raw_frame", &I3File::get_raw_frame,
	 bp::arg("index"), 
	 "Get only frame 'index' (see get_frame())")
    ;
#endif
}
