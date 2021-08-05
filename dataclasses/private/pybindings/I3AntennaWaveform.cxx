#include <dataclasses/I3AntennaWaveform.h>
#include <icetray/python/dataclass_suite.hpp>
#include <complex>

namespace bp = boost::python;

template <typename T>
void register_i3antennawaveform_of(const std::string& s) {

  typedef I3AntennaWaveform<T> antennawaveform_t;

  bp::class_<antennawaveform_t, bp::bases<I3FrameObject>, boost::shared_ptr<antennawaveform_t> >
  (s.c_str())
  .def(bp::init<>())
  .add_property("binning", &antennawaveform_t::GetBinning, &antennawaveform_t::SetBinning, "Get the trace step size")
  .add_property("offset", &antennawaveform_t::GetOffset, &antennawaveform_t::SetOffset, "Offset of trace (i.e. T0)")
  .def("__len__", &antennawaveform_t::GetSize)
  .def("__getitem__", &antennawaveform_t::GetBin)
  .def("__setitem__", &antennawaveform_t::SetBin)
  .def("GetSize", &antennawaveform_t::GetSize, "Length of trace (stop - start)")
  .def("CopyIntoTrace", &antennawaveform_t::CopyIntoTrace, "Copies given trace into self")
  .def("PushBack", &antennawaveform_t::PushBack, "Add to the end of the trace")
  .def("PopBack", &antennawaveform_t::PopBack, "Remove last item from trace")
  .def("Clear", &antennawaveform_t::Clear, "Clear all data from the trace")
  .def("GetSubset", &antennawaveform_t::GetSubset, "Get waveform which includes only a slice of the current trace")
  .def("Roll", &antennawaveform_t::Roll, "Rotate the elements in a waveform")
  .def(bp::self + bp::self)
  .def(bp::self - bp::self)
  .def(bp::self += bp::self)

//hush this false positive
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"

  .def(bp::self -= bp::self)

#pragma clang diagnostic pop
#endif

  .def(bp::self += T())
  .def(bp::self -= T())
  .def(bp::self * double())
  .def(bp::self / double())
  .def(bp::self *= double())
  .def(bp::self /= double())
  .def(bp::self_ns::str(bp::self))
  .def(bp::dataclass_suite<antennawaveform_t>())
  ;
  register_pointer_conversions<antennawaveform_t>();


  typedef I3Map<AntennaKey, antennawaveform_t> antennawaveformmap_t;

  bp::class_<antennawaveformmap_t, bp::bases<I3FrameObject>, boost::shared_ptr<antennawaveformmap_t> >
  ((s + std::string("Map")).c_str())
  .def(bp::dataclass_suite<antennawaveformmap_t>())
  .def(bp::self_ns::str(bp::self))
  ;

  register_pointer_conversions<antennawaveformmap_t>();
}


void register_I3AntennaWaveform() {
  register_i3antennawaveform_of<double>("AntennaTimeSeries");
  register_i3antennawaveform_of<std::complex<double>>("AntennaSpectrum");

  register_i3antennawaveform_of<I3Position>("EFieldTimeSeries");
  register_i3antennawaveform_of<I3ComplexVector>("EFieldSpectrum");
}
