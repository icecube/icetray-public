#include <dataclasses/I3AntennaChannel.h>
#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

//The normal function passes a reference, need to return a copy instead
FFTData get_ConstFFTData(const I3AntennaChannel& self) {
  FFTData temp = self.GetConstFFTData();
  return temp;
}

void register_I3AntennaChannel() {

  bp::class_<I3AntennaChannel, bp::bases<I3FrameObject>, I3AntennaChannelPtr>("I3AntennaChannel")
  .def(bp::dataclass_suite<I3AntennaChannel>())
  .def(bp::dataclass_suite<FFTData>())
  .def("GetMaxAmplitude", &I3AntennaChannel::GetMaxAmplitude, "Maximum value of Hilbert envelope")
  .def("GetTimeAtMaximum", &I3AntennaChannel::GetTimeAtMaximum, "Get time corresponding to maximum value of Hilbert envelope")
  .def("GetFFTData", get_ConstFFTData, "Get the FFTDataContainer")
  .def(bp::dataclass_suite<I3AntennaChannel>())
  ;

  register_pointer_conversions<I3AntennaChannel>();


  bp::class_<I3AntennaChannelMap, bp::bases<I3FrameObject>, I3AntennaChannelMapPtr>("I3AntennaChannelMap")
  .def(bp::dataclass_suite<I3AntennaChannelMap>())
  ;
  register_pointer_conversions<I3AntennaChannelMap>();

}