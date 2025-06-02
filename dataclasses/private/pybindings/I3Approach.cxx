// SPDX-FileCopyrightText: 2025 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dataclasses/I3Approach.h>

#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

std::string repr(const I3Approach &approach) {
  std::stringstream s;
  s << approach;
  return s.str();
}

void SetPosition1(I3Approach &approach, I3Position pos) {
  approach.SetPosition(pos);
}
void SetPosition2(I3Approach &approach, double pos_x, double pos_y, double pos_z) {
  approach.SetPosition(pos_x, pos_y, pos_z);
}

void register_I3Approach() {
  bp::class_<I3Approach, I3ApproachPtr>("I3Approach")
    .def(bp::init<>())
    .def(bp::init<double, I3Position>())
    .def(bp::init<double, double, double, double>())
    .def("__repr__", repr)
    .def(bp::self == bp::self)
    .def(bp::self != bp::self)
    .def(bp::self < bp::self)
    .def(bp::self > bp::self)
    .def(bp::self >= bp::self)
    .def(bp::self <= bp::self)
    .def("SetDistance", &I3Approach::SetDistance, "Set approach distance")
    .def("SetPosition", &SetPosition1, "Set approach position")
    .def("SetPosition", &SetPosition2, "Set approach position")
    .def("GetDistance", &I3Approach::GetDistance, "Get approach distance")
    .def("GetPosition", &I3Approach::GetPosition, "Get approach position")
    .add_property("distance", &I3Approach::GetDistance, &I3Approach::SetDistance, "Approach distance")
    .add_property("position", &I3Approach::GetPosition, &SetPosition1, "Approach position")
    .def(bp::dataclass_suite<I3Approach>())
  ;
}
