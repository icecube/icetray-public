//
//   Copyright (c) 2009  Claudio Kopper
//   
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

#include <dataclasses/I3Orientation.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

void register_I3Orientation()
{
	void (I3Orientation::* fromOrientation)(const I3Orientation&) = &I3Orientation::SetOrientation;
	void (I3Orientation::* fromDirection)(const I3Direction&) = &I3Orientation::SetOrientation;
	void (I3Orientation::* fromDirections)(const I3Direction&, const I3Direction&) = &I3Orientation::SetOrientation;
	void (I3Orientation::* fromDoubles)(double, double, double, double, double, double) = &I3Orientation::SetOrientation;

	I3Position  (I3Orientation::* RotatePositionIn) (const I3Position&) const  = &I3Orientation::RotateIn;
	I3Direction (I3Orientation::* RotateDirectionIn)(const I3Direction&) const = &I3Orientation::RotateIn;
	I3Position  (I3Orientation::* RotatePositionOut) (const I3Position&) const  = &I3Orientation::RotateOut;
	I3Direction (I3Orientation::* RotateDirectionOut)(const I3Direction&) const = &I3Orientation::RotateOut;
	
	const char *rotate_in_doc = "Rotate from a coordinate system where x=(1,0,0),y=(0,1,0),z=(0,0,1) to one where x=up,y=right,z=dir";
	const char *rotate_out_doc = "Rotate from a coordinate system where x=up,y=right,z=dir to one where x=(1,0,0),y=(0,1,0),z=(0,0,1)";

	class_<I3Orientation, bases<I3FrameObject>, boost::shared_ptr<I3Orientation> >("I3Orientation")
	.def(init<double,double,double,double,double,double>())
	.def(init<const I3Direction &,const I3Direction &>())
	.def(init<const I3Direction &>())

	.def("set_orientation", fromOrientation)
	.def("set_orientation", fromDirection)
	.def("set_orientation", fromDirections)
	.def("set_orientation", fromDoubles)

	.def("rotate_in", RotatePositionIn, rotate_in_doc)
	.def("rotate_in", RotateDirectionIn, rotate_in_doc)
	.def("rotate_out", RotatePositionOut, rotate_out_doc)
	.def("rotate_out", RotateDirectionOut, rotate_out_doc)

    //.def("rot_vector_in_place", &I3Orientation::RotVectorInPlace)

    #define MEMBERS (ResetOrientation)
    BOOST_PP_SEQ_FOR_EACH(WRAP_DEF_RECASE, I3Orientation, MEMBERS)
    #undef  MEMBERS

    #define RO_PROPERTIES \
        (Zenith)(Azimuth)(X)(Y)(Z)                                  \
        (Dir)(DirZenith)(DirAzimuth)(DirX)(DirY)(DirZ)              \
        (Up)(UpZenith)(UpAzimuth)(UpX)(UpY)(UpZ)                    \
        (Right)(RightZenith)(RightAzimuth)(RightX)(RightY)(RightZ)
    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, I3Orientation, RO_PROPERTIES)
    #undef  RO_PROPERTIES
	.add_property("dir_theta", &I3Orientation::CalcDirTheta)
	.add_property("dir_phi", &I3Orientation::CalcDirPhi)
	.add_property("up_theta", &I3Orientation::CalcUpTheta)
	.add_property("up_phi", &I3Orientation::CalcUpPhi)
	.add_property("right_theta", &I3Orientation::CalcRightTheta)
	.add_property("right_phi", &I3Orientation::CalcRightPhi)
    
    .def(self == self)
    .def(dataclass_suite<I3Orientation>())
    ;
    
	register_pointer_conversions<I3Orientation>();
    
}
