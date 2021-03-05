#include <dataclasses/AntennaKey.h>

#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

std::string repr(const AntennaKey& key) {
  std::stringstream s;
  s << key;
  return s.str();
}

void register_AntennaKey() {
  bp::class_<AntennaKey, boost::shared_ptr<AntennaKey> >("AntennaKey")
  .def(bp::init<>())
  .def(bp::init<int, int>())
  .def("__repr__", repr)
  .def(bp::self == bp::self)
  .def(bp::self != bp::self)
  .def(bp::self < bp::self)
  .def("SetStationID", &AntennaKey::SetStationID, "Set station number")
  .def("SetAntennaID", &AntennaKey::SetAntennaID, "Set antenna number")
  .def("GetStationID", &AntennaKey::GetStationID, "Set station number")
  .def("GetAntennaID", &AntennaKey::GetAntennaID, "Set antenna number")
  .add_property("station", &AntennaKey::GetStationID, &AntennaKey::SetStationID, "Station ID")
  .add_property("antenna", &AntennaKey::GetAntennaID, &AntennaKey::SetAntennaID, "Antenna ID")
  .def(bp::dataclass_suite<AntennaKey>())
  ;
}
