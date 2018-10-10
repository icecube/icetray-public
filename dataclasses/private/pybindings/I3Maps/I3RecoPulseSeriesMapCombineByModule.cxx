
#include <dataclasses/physics/I3RecoPulseSeriesMapCombineByModule.h>
#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

I3RecoPulseSeriesMapPtr
underhanded_apply(const I3RecoPulseSeriesMapCombineByModule& shifter, const I3Frame &frame)
{
  return boost::const_pointer_cast<I3RecoPulseSeriesMap>(shifter.Apply(frame));
}

void register_I3RecoPulseSeriesMapCombineByModule()
{
  bp::class_<I3RecoPulseSeriesMapCombineByModule, bp::bases<I3FrameObject>,
      I3RecoPulseSeriesMapCombineByModulePtr>("I3RecoPulseSeriesMapCombineByModule",
      bp::init<const std::string &>(bp::args("pulses_key")))
    .def("apply", &underhanded_apply, "Apply the combination, returning I3RecoPulseSeriesMap.")
    .add_property("source", &I3RecoPulseSeriesMapCombineByModule::GetSource)
    .def(bp::dataclass_suite<I3RecoPulseSeriesMapCombineByModule>())
    ;
  ;

  register_pointer_conversions<I3RecoPulseSeriesMapCombineByModule>();
}
