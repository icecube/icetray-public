// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include "dataclasses/I3Map.h"
#include "icetray/python/dataclass_suite.hpp"

using namespace boost::python;

void register_I3MapUnsignedDouble()
{
  class_<I3MapUnsignedDouble, bases<I3FrameObject>, I3MapUnsignedDoublePtr>("I3MapUnsignedDouble")
    .def(dataclass_suite<I3MapUnsignedDouble >())
    ;
  register_pointer_conversions<I3MapUnsignedDouble>();
}

