
#include <dataclasses/I3RecoPulseSeriesMapApplySPECorrection.h>
#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

I3RecoPulseSeriesMapPtr
underhanded_apply(const I3RecoPulseSeriesMapApplySPECorrection& shifter, const I3Frame &frame)
{
  return boost::const_pointer_cast<I3RecoPulseSeriesMap>(shifter.Apply(frame));
}

void register_I3RecoPulseSeriesMapApplySPECorrection()
{
  bp::class_<I3RecoPulseSeriesMapApplySPECorrection, bp::bases<I3FrameObject>,
      I3RecoPulseSeriesMapApplySPECorrectionPtr>("I3RecoPulseSeriesMapApplySPECorrection",
      bp::init<const std::string &, const std::string &>(bp::args("pulses_key", "calibration_key")))
    .def("apply", &underhanded_apply, "Apply the correction to an I3Frame, returning an I3RecoPulseSeries.")
    .add_property("pulses_source", &I3RecoPulseSeriesMapApplySPECorrection::GetPulsesSource)
    .add_property("calibration_source", &I3RecoPulseSeriesMapApplySPECorrection::GetCalibrationSource)
    .def(bp::dataclass_suite<I3RecoPulseSeriesMapApplySPECorrection>())
    ;
  ;

  register_pointer_conversions<I3RecoPulseSeriesMapApplySPECorrection>();
}
