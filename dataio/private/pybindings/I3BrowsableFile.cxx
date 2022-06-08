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

#include <dataio/I3File.h>

using namespace boost::python;
namespace bp = boost::python;

namespace {
  void i3file_open_file(I3File& i3file,
			const std::string name)
  {
    i3file.open_file(name);
  }
}

void register_I3BrowsableFile()
{
  class_<I3File, boost::noncopyable>
    ("I3BrowsableFile", "A low-level interface to .i3 files. "
     "Prefer I3File if you are doing analysis ")
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
    .def("get_related_frames", &I3File::get_related_frames,
	 bp::arg("index"), 
	 "Get only preceding frames 'index' depends on (see get_frame())")
    .def("__getitem__", &I3File::get_frame,
	 bp::arg("index"),
	 "Get frame by number, equivalent to index()")
    ;

}
