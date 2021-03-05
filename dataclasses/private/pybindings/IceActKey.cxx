#include <dataclasses/IceActKey.h>

#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

std::string repr(const IceActKey& key) {
  std::stringstream s;
  s << key;
  return s.str();
}

void register_IceActKey() {
  bp::class_<IceActKey, boost::shared_ptr<IceActKey> >("IceActKey")
  .def(bp::init<>())
  .def(bp::init<int, int>())
  .def("__repr__", repr)
  .def(bp::self == bp::self)
  .def(bp::self != bp::self)
  .def(bp::self < bp::self)
  .def("SetStationID", &IceActKey::SetStationID, "Set station number")
  .def("SetTelescopeID", &IceActKey::SetTelescopeID, "Set telescope number")
  .def("GetStationID", &IceActKey::GetStationID, "Get station number")
  .def("GetTelescopeID", &IceActKey::GetTelescopeID, "Get telescope number")
  .add_property("station", &IceActKey::GetStationID, &IceActKey::SetStationID, "Station ID")
  .add_property("telescope", &IceActKey::GetTelescopeID, &IceActKey::SetTelescopeID, "Telescope ID")
  .def(bp::dataclass_suite<IceActKey>())
  ;
}
