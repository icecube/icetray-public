#include <dataclasses/physics/I3ScintRecoPulseSeriesMap.h>
#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

void register_I3ScintRecoPulseSeriesMap() {

  bp::class_<I3ScintRecoPulseSeriesMap, bp::bases<I3FrameObject>, I3ScintRecoPulseSeriesMapPtr>("I3ScintRecoPulseSeriesMap")
  .def(bp::dataclass_suite<I3ScintRecoPulseSeriesMap>())
  ;

  register_pointer_conversions<I3ScintRecoPulseSeriesMap>();
}
