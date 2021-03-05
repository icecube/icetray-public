#include <dataclasses/fft/FFTHilbertEnvelope.h>

#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

//Wrappers for peak time because python can't pass numbers by reference
double GetHilbPeakTimeFFTData3d(const FFTData3D& fft) {
  double temp = 0;
  double time = fft::GetHilbertPeakTime(fft, temp);
  return time;
}
double GetHilbPeakTimeTrace3d(const EFieldTimeSeries& fft) {
  double temp = 0;
  double time = fft::GetHilbertPeakTime(fft, temp);
  return time;
}

//Wrappers for peak amplitude because python can't pass numbers by reference
double GetHilbPeakFFTData(const FFTData& fft) {
  double temp = 0;
  fft::GetHilbertPeakTime(fft, temp);
  return temp;
}
double GetHilbPeakFFTData3d(const FFTData3D& fft) {
  double temp = 0;
  fft::GetHilbertPeakTime(fft, temp);
  return temp;
}
double GetHilbPeakTrace(const AntennaTimeSeries& fft) {
  double temp = 0;
  fft::GetHilbertPeakTime(fft, temp);
  return temp;
}
double GetHilbPeakTrace3d(const EFieldTimeSeries& fft) {
  double temp = 0;
  fft::GetHilbertPeakTime(fft, temp);
  return temp;
}

class FFTNS {};
void register_FFTHilbertEnvelope() {

  std::string nested_name = bp::extract<std::string>(bp::scope().attr("__name__") + ".fft");
  bp::object nested_module(bp::handle<>(bp::borrowed(PyImport_AddModule(nested_name.c_str()))));
  bp::scope().attr("fft") = nested_module;
  bp::scope parent = nested_module;

  AntennaTimeSeries (*HilbEnvFFTData)(const FFTData&)          = &fft::GetHilbertEnvelope;
  EFieldTimeSeries  (*HilbEnvFFTData3d)(const FFTData3D&)      = &fft::GetHilbertEnvelope;
  AntennaTimeSeries (*HilbEnvTraceD)(const AntennaTimeSeries&) = &fft::GetHilbertEnvelope;
  EFieldTimeSeries  (*HilbEnvTrace3D)(const EFieldTimeSeries&) = &fft::GetHilbertEnvelope;

  bp::def("GetHilbertEnvelope", HilbEnvFFTData, "Get the Hilbert envelope for the given I3RadTrace or FFTData");
  bp::def("GetHilbertEnvelope", HilbEnvFFTData3d, "Get the Hilbert envelope for the given I3RadTrace or FFTData");
  bp::def("GetHilbertEnvelope", HilbEnvTraceD, "Get the Hilbert envelope for the given I3RadTrace or FFTData");
  bp::def("GetHilbertEnvelope", HilbEnvTrace3D, "Get the Hilbert envelope for the given I3RadTrace or FFTData");

  double (*GetHilbPeakTimeFFTData)(const FFTData&)          = &fft::GetHilbertPeakTime;
  double (*GetHilbPeakTimeTrace)(const AntennaTimeSeries&)  = &fft::GetHilbertPeakTime;

  bp::def("GetHilbertPeakTime", GetHilbPeakTimeFFTData,     "Get the time at which the Hilbert envelope reaches its peak");
  bp::def("GetHilbertPeakTime", &GetHilbPeakTimeFFTData3d,  "Get the time at which the Hilbert envelope reaches its peak");
  bp::def("GetHilbertPeakTime", GetHilbPeakTimeTrace,       "Get the time at which the Hilbert envelope reaches its peak");
  bp::def("GetHilbertPeakTime", &GetHilbPeakTimeTrace3d,    "Get the time at which the Hilbert envelope reaches its peak");

  bp::def("GetHilbertPeak", &GetHilbPeakFFTData,    "Get the amplitude of the Hilbert envelope's peak");
  bp::def("GetHilbertPeak", &GetHilbPeakFFTData3d,  "Get the amplitude of the Hilbert envelope's peak");
  bp::def("GetHilbertPeak", &GetHilbPeakTrace,      "Get the amplitude of the Hilbert envelope's peak");
  bp::def("GetHilbertPeak", &GetHilbPeakTrace3d,    "Get the amplitude of the Hilbert envelope's peak");
}