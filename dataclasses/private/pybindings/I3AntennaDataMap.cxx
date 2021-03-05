#include <dataclasses/I3AntennaDataMap.h>
#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

void register_I3AntennaDataMap() {

  bp::class_<I3AntennaDataMap, bp::bases<I3FrameObject>, I3AntennaDataMapPtr>("I3AntennaDataMap")
  .def(bp::dataclass_suite<I3AntennaDataMap>())
  ;

  register_pointer_conversions<I3AntennaDataMap>();
}
