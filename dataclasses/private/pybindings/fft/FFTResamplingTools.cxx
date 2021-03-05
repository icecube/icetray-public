#include <dataclasses/fft/FFTResamplingTools.h>

#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

void register_FFTResamplingTools() {
  //Disambiguate overloaded function for pybindings
  void (*Resample)(FFTData&, const double) = &fft::ResampleFFTDataContainer;

  std::string nested_name = bp::extract<std::string>(bp::scope().attr("__name__") + ".fft");
  bp::object nested_module(bp::handle<>(bp::borrowed(PyImport_AddModule(nested_name.c_str()))));
  bp::scope().attr("fft") = nested_module;
  bp::scope parent = nested_module;
  
  bp::def("ResampleFFTData", Resample, "Given an FFTDataContainer and a desired binning, the time series will be resampled");
}