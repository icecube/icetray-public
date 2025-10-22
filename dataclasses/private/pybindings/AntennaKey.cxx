// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dataclasses/AntennaKey.h>

#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

std::string repr(const AntennaKey& key) {
  std::stringstream s;
  s << key;
  return s.str();
}

size_t antkeyhash(const AntennaKey& key) {
  size_t res = 17;
  res = res * 31 + std::hash<int>()(key.GetStationID());
  res = res * 31 + std::hash<int>()(key.GetAntennaID());
  return res;
}

void register_AntennaKey() {
  bp::class_<AntennaKey, boost::shared_ptr<AntennaKey> >("AntennaKey")
  .def(bp::init<>())
  .def(bp::init<int, int>())
  .def("__hash__", antkeyhash)
  .def("__repr__", repr)
  .def(bp::self == bp::self)
  .def(bp::self != bp::self)
  .def(bp::self < bp::self)
  .def("SetStationID", &AntennaKey::SetStationID, "Set station number")
  .def("SetAntennaID", &AntennaKey::SetAntennaID, "Set antenna number")
  .def("GetStationID", &AntennaKey::GetStationID, "Get station number")
  .def("GetAntennaID", &AntennaKey::GetAntennaID, "Get antenna number")
  .add_property("station", &AntennaKey::GetStationID, &AntennaKey::SetStationID, "Station ID")
  .add_property("antenna", &AntennaKey::GetAntennaID, &AntennaKey::SetAntennaID, "Antenna ID")
  .def(bp::dataclass_suite<AntennaKey>())
  ;
}

void register_I3MapAntennaKeyAntennaKey()
{
  bp::class_<I3MapAntennaKeyAntennaKey, bp::bases<I3FrameObject>, I3MapAntennaKeyAntennaKeyPtr>("I3MapAntennaKeyAntennaKey")
  .def(bp::dataclass_suite<I3MapAntennaKeyAntennaKey>())
  ;
  register_pointer_conversions<I3MapAntennaKeyAntennaKey>();
}

void register_I3MapAntennaKeyVectorDouble()
{
  bp::class_<I3MapAntennaKeyVectorDouble, bp::bases<I3FrameObject>, I3MapAntennaKeyVectorDoublePtr>("I3MapAntennaKeyVectorDouble")
  .def(bp::dataclass_suite<I3MapAntennaKeyVectorDouble>())
  ;
  register_pointer_conversions<I3MapAntennaKeyVectorDouble>();
}
