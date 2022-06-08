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

