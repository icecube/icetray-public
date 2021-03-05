#include <dataclasses/physics/I3IceActRecoPulseMap.h>
#include <icetray/python/dataclass_suite.hpp>

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
