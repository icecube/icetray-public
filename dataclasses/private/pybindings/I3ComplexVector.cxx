#include <dataclasses/I3ComplexVector.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/I3Direction.h>
#include <dataclasses/I3Position.h>
#include <complex>

namespace bp = boost::python;

constexpr unsigned i3complexvector_len(const I3ComplexVector&) { return 3; }

std::complex<double> i3complexvector_getitem(const I3ComplexVector& self, unsigned index) {
  switch (index) {
  case 0:
    return self.GetX();
  case 1:
    return self.GetY();
  case 2:
    return self.GetZ();
  default:
    throw std::out_of_range("index out of range");
  }
}

void i3complexvector_setitem(I3ComplexVector& self, unsigned index, std::complex<double> value) {
  switch (index) {
  case 0:
    self.SetX(value);
    break;
  case 1:
    self.SetY(value);
    break;
  case 2:
    self.SetZ(value);
    break;
  default:
    throw std::out_of_range("index out of range");
  }
}

void register_I3ComplexVector() {
  bp::class_<I3ComplexVector, bp::bases<I3FrameObject>, boost::shared_ptr<I3ComplexVector> >
  ("I3ComplexVector")
  .def(bp::dataclass_suite<I3Direction>())
  .def(bp::dataclass_suite<I3Position>())
  .def(bp::init<>())
  .def(bp::init<std::complex<double>, std::complex<double>, std::complex<double>>())
  .add_property("x", &I3ComplexVector::GetX, &I3ComplexVector::SetX, "X-Value")
  .add_property("y", &I3ComplexVector::GetY, &I3ComplexVector::SetY, "Y-Value")
  .add_property("z", &I3ComplexVector::GetZ, &I3ComplexVector::SetZ, "Z-Value")
  .def("__abs__", &I3ComplexVector::Magnitude)
  .def("magnitude", &I3ComplexVector::Magnitude, "Magnitude of vector")
  .def("normalize", &I3ComplexVector::Normalize, "Normalize the vector")
  .def("unit_vector", &I3ComplexVector::GetUnitVector, "Get the unit vector")
  .def("Dot", &I3ComplexVector::Dot, "Do dot product")
  .def("rotate_x", &I3ComplexVector::RotateX, "Rotation about x-axis")
  .def("rotate_y", &I3ComplexVector::RotateY, "Rotation about y-axis")
  .def("rotate_z", &I3ComplexVector::RotateZ, "Rotation about z-axis")
  .def(bp::self + bp::self)
  .def(bp::self - bp::self)
  .def(bp::self * bp::self)
  .def(bp::self * std::complex<double>())
  .def(bp::self / std::complex<double>())
  .def(bp::self += bp::self)

// hush this false positive
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"

  .def(bp::self -= bp::self)

#pragma clang diagnostic pop
#endif

  .def(bp::self *= std::complex<double>())
  .def(bp::self /= std::complex<double>())
  .def(bp::self == bp::self)
  .def(bp::self != bp::self)
  .def("__len__", i3complexvector_len)
  .def("__getitem__", i3complexvector_getitem)
  .def("__setitem__", i3complexvector_setitem)
  .def(bp::self_ns::str(bp::self))
  ;
  register_pointer_conversions<I3ComplexVector>();
}
