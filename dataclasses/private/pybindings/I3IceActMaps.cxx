// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include "dataclasses/physics/I3IceActMaps.h"
#include "icetray/python/dataclass_suite.hpp"

namespace bp = boost::python;


void register_I3IceActRecoPulseSeries() {
  bp::class_<I3IceActRecoPulseSeries, bp::bases<I3FrameObject>, I3IceActRecoPulseSeriesPtr>("I3IceActRecoPulseSeries")
  .def(bp::dataclass_suite<I3IceActRecoPulseSeries>());
  register_pointer_conversions<I3IceActRecoPulseSeries>();
}

void register_I3IceActRecoPulseSeriesMap() {
  bp::class_<I3IceActRecoPulseSeriesMap, bp::bases<I3FrameObject>, I3IceActRecoPulseSeriesMapPtr>("I3IceActRecoPulseSeriesMap")
  .def(bp::dataclass_suite<I3IceActRecoPulseSeriesMap>());
  register_pointer_conversions<I3IceActRecoPulseSeriesMap>();
}

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

void register_I3IceActTriggerMap() {
  bp::class_<I3IceActTriggerMap, bp::bases<I3FrameObject>, I3IceActTriggerMapPtr>("I3IceActTriggerMap")
  .def(bp::dataclass_suite<I3IceActTriggerMap>());
  register_pointer_conversions<I3IceActTriggerMap>();
}

void register_I3IceActBoolMap() {
  bp::class_<I3IceActBoolMap, bp::bases<I3FrameObject>, I3IceActBoolMapPtr>("I3IceActBoolMap")
  .def(bp::dataclass_suite<I3IceActBoolMap>());
  register_pointer_conversions<I3IceActBoolMap>();
}

void register_I3IceActIntMap() {
  bp::class_<I3IceActIntMap, bp::bases<I3FrameObject>, I3IceActIntMapPtr>("I3IceActIntMap")
  .def(bp::dataclass_suite<I3IceActIntMap>());
  register_pointer_conversions<I3IceActIntMap>();
}

void register_I3IceActUnsignedMap() {
  bp::class_<I3IceActUnsignedMap, bp::bases<I3FrameObject>, I3IceActUnsignedMapPtr>("I3IceActUnsignedMap")
  .def(bp::dataclass_suite<I3IceActUnsignedMap>());
  register_pointer_conversions<I3IceActUnsignedMap>();
}

void register_I3IceActDoubleMap() {
  bp::class_<I3IceActDoubleMap, bp::bases<I3FrameObject>, I3IceActDoubleMapPtr>("I3IceActDoubleMap")
  .def(bp::dataclass_suite<I3IceActDoubleMap>());
  register_pointer_conversions<I3IceActDoubleMap>();
}

void register_I3IceActStringMap() {
  bp::class_<I3IceActStringMap, bp::bases<I3FrameObject>, I3IceActStringMapPtr>("I3IceActStringMap")
  .def(bp::dataclass_suite<I3IceActStringMap>());
  register_pointer_conversions<I3IceActStringMap>();
}

void register_I3IceActTelescopePixelUnsignedMap() {
  bp::class_<I3IceActTelescopePixelUnsignedMap, bp::bases<I3FrameObject>, I3IceActTelescopePixelUnsignedMapPtr>("I3IceActTelescopePixelUnsignedMap")
  .def(bp::dataclass_suite<I3IceActTelescopePixelUnsignedMap>());
  register_pointer_conversions<I3IceActTelescopePixelUnsignedMap>();
}

void register_I3IceActTelescopePixelDoubleMap() {
  bp::class_<I3IceActTelescopePixelDoubleMap, bp::bases<I3FrameObject>, I3IceActTelescopePixelDoubleMapPtr>("I3IceActTelescopePixelDoubleMap")
  .def(bp::dataclass_suite<I3IceActTelescopePixelDoubleMap>());
  register_pointer_conversions<I3IceActTelescopePixelDoubleMap>();
}

void register_I3IceActTelescopePixelVectorIntMap() {
  bp::class_<I3IceActTelescopePixelVectorIntMap, bp::bases<I3FrameObject>, I3IceActTelescopePixelVectorIntMapPtr>("I3IceActTelescopePixelVectorIntMap")
  .def(bp::dataclass_suite<I3IceActTelescopePixelVectorIntMap>());
  register_pointer_conversions<I3IceActTelescopePixelVectorIntMap>();
}

void register_I3IceActTelescopeStringDoubleMap() {
  bp::class_<I3IceActTelescopeStringDoubleMap, bp::bases<I3FrameObject>, I3IceActTelescopeStringDoubleMapPtr>("I3IceActTelescopeStringDoubleMap")
  .def(bp::dataclass_suite<I3IceActTelescopeStringDoubleMap>());
  register_pointer_conversions<I3IceActTelescopeStringDoubleMap>();
}
