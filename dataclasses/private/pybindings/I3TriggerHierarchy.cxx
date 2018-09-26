//
//   Copyright (c) 2015
//   the IceCube Collaboration
//
//   $Id: $
//
//   @file I3TriggerHierarchy.cxx
//   @version
//   @date $Date: 2015-12-15$
//   @author mjurkovic <martin.jurkovic@tum.de>
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


#include <dataclasses/physics/I3TriggerHierarchy.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/ostream_overloads.hpp>

using namespace boost::python;

using boost::python::object;

int
length(I3TriggerHierarchyPtr t) {
    if(t) {
        return  t->size();
    }
    return 0;
}

static I3TriggerHierarchyPtr
from_frame(I3Frame &frame, const std::string &name)
{
    I3TriggerHierarchyConstPtr ptr =
        frame.Get<I3TriggerHierarchyConstPtr>(name);
    return boost::const_pointer_cast<I3TriggerHierarchy>(ptr);
}


void
register_I3TriggerHierarchy()
{
    class_<I3TriggerHierarchy, bases<I3FrameObject>,
        I3TriggerHierarchyPtr>("I3TriggerHierarchy")
        .def("__len__", &length)
        .def("__iter__", bp::iterator<I3TriggerHierarchy>())
        .def("insert",&I3TriggerHierarchyUtils::Insert)
        .def("from_frame", &from_frame)
        .staticmethod("from_frame")
        .def(dataclass_suite<I3TriggerHierarchy>())
        ;

    register_pointer_conversions<I3TriggerHierarchy>();
}

