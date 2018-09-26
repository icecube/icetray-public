//
//   Copyright (c) 2009  Claudio Kopper
//   
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
