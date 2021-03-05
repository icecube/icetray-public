#include <dataclasses/ScintKey.h>

#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

std::string repr(const ScintKey& key) {
  std::stringstream s;
  s << key;
  return s.str();
}

void register_ScintKey() {
  bp::class_<ScintKey, boost::shared_ptr<ScintKey> >("ScintKey")
  .def(bp::init<>())
  .def(bp::init<int, int>())
  .def("__repr__", repr)
  .def(bp::self == bp::self)
  .def(bp::self != bp::self)
  .def(bp::self < bp::self)
  .def("SetStationID", &ScintKey::SetStationID, "Set station number")
  .def("SetPanelID", &ScintKey::SetPanelID, "Set panel number")
  .def("GetStationID", &ScintKey::GetStationID, "Get station number")
  .def("GetPanelID", &ScintKey::GetPanelID, "Get panel number")
  .add_property("station", &ScintKey::GetStationID, &ScintKey::SetStationID, "Station ID")
  .add_property("panel", &ScintKey::GetPanelID, &ScintKey::SetPanelID, "Panel ID")
  .def(bp::dataclass_suite<ScintKey>())
  ;
}
