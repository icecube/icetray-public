#include <dataclasses/fft/FFTDataContainer.h>
#include <dataclasses/I3Position.h>
#include <dataclasses/I3ComplexVector.h>
#include <icetray/python/dataclass_suite.hpp>
#include <complex>

namespace bp = boost::python;

template <typename T, typename F>
void register_fftdatacontainer_of(const std::string& s) {
  typedef FFTDataContainer<T, F> container_t;

  bp::class_<container_t, bp::bases<I3FrameObject>, boost::shared_ptr<container_t> >
  ((std::string("FFTData") + s).c_str())
  .def(bp::dataclass_suite<container_t>())
  .def(bp::init<>())
  .def(bp::init<container_t>())
  .def(bp::init<I3AntennaWaveform<T>>())
  .def(bp::init<I3AntennaWaveform<F>>())
  .add_property("nyquistzone", &container_t::GetNyquistZone, &container_t::SetNyquistZone)
  .def("GetFrequencyOfBin", &container_t::GetFrequencyOfBin)
  .def("GetBinOfFrequency", &container_t::GetBinOfFrequency)
  .def("GetTimeSeries", &container_t::GetTimeSeries, "Get the time series (FFT done automatically)", bp::return_internal_reference<>())
  .def("GetFrequencySpectrum", &container_t::GetFrequencySpectrum, "Get the freq spectrum (FFT done automatically)", bp::return_internal_reference<>())
  .def("LoadTimeSeries", &container_t::LoadTimeSeries)
  .def("LoadFrequencySpectrum", &container_t::LoadFrequencySpectrum)
  .def(bp::self_ns::str(bp::self))
  ;
  register_pointer_conversions<container_t>();

  typedef I3Map<AntennaKey, container_t> containermap_t;
  bp::class_<containermap_t, bp::bases<I3FrameObject>, boost::shared_ptr<containermap_t> >
  ((std::string("FFTData") + s + (std::string("Map"))).c_str())
  .def(bp::dataclass_suite<containermap_t>())
  ;

  register_pointer_conversions<containermap_t>();
}


void register_FFTDataContainer() {
  register_fftdatacontainer_of<double, std::complex<double>>("");

  register_fftdatacontainer_of<I3Position, I3ComplexVector>("3D");
}