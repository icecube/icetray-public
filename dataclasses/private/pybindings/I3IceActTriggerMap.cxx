#include <dataclasses/physics/I3IceActTriggerMap.h>
#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;


void register_I3IceActTriggerMap() {

  bp::class_<I3IceActTriggerMap, bp::bases<I3FrameObject>, I3IceActTriggerMapPtr>("I3IceActTriggerMap")
  .def(bp::dataclass_suite<I3IceActTriggerMap>());

  register_pointer_conversions<I3IceActTriggerMap>();
}
