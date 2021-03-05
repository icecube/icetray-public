#include <dataclasses/physics/I3IceActWaveformMap.h>
#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

void register_I3IceActWaveform() {

  bp::class_<I3IceActWaveform, bp::bases<I3FrameObject>, I3IceActWaveformPtr>("I3IceActWaveform")
  .def(bp::dataclass_suite<I3IceActWaveform>());

  register_pointer_conversions<I3IceActWaveform>();
}

void register_I3IceActWaveformMap() {

  bp::class_<I3IceActWaveformMap, bp::bases<I3FrameObject>, I3IceActWaveformMapPtr>("I3IceActWaveformMap")
  .def(bp::dataclass_suite<I3IceActWaveformMap>());

  register_pointer_conversions<I3IceActWaveformMap>();
}
