#include <dataclasses/I3ComplexVector.h>
#include <dataclasses/I3Position.h>
#include <dataclasses/I3Direction.h>
#include <serialization/complex.hpp>

I3ComplexVector::I3ComplexVector(const I3Direction& dir) :
  x_(dir.GetX()),
  y_(dir.GetY()),
  z_(dir.GetZ()) {}

I3ComplexVector::I3ComplexVector(const I3Position& pos) :
  x_(pos.GetX()),
  y_(pos.GetY()),
  z_(pos.GetZ()) {}

I3ComplexVector I3ComplexVector::RotateX(const double angle) {
  const double cos = std::cos(angle);
  const double sin = std::sin(angle);

  const std::complex<double> yprime = y_ * cos - z_ * sin;
  const std::complex<double> zprime = y_ * sin + z_ * cos;
  y_ = yprime;
  z_ = zprime;

  return *this;
}

I3ComplexVector I3ComplexVector::RotateY(const double angle) {
  const double cos = std::cos(angle);
  const double sin = std::sin(angle);

  const std::complex<double> xprime = x_ * cos + z_ * sin;
  const std::complex<double> zprime = -x_ * sin + z_ * cos;
  x_ = xprime;
  z_ = zprime;

  return *this;
}

I3ComplexVector I3ComplexVector::RotateZ(const double angle) {
  const double cos = std::cos(angle);
  const double sin = std::sin(angle);

  const std::complex<double> xprime = x_ * cos - y_ * sin;
  const std::complex<double> yprime = x_ * sin + y_ * cos;
  x_ = xprime;
  y_ = yprime;

  return *this;
}

I3ComplexVector  I3ComplexVector::GetUnitVector() const {
  std::complex<double> mag = Magnitude();
  if (std::complex<double>(0,0) != mag) {
    return *this / mag;
  }
  return *this;
}

void I3ComplexVector::Normalize() {
  std::complex<double> mag = Magnitude();
  if (std::complex<double>(0,0) != mag)
    *this *= 1. / mag;
}

double abs(const I3ComplexVector& vec) {
  return vec.Magnitude();
}

std::ostream& I3ComplexVector::Print(std::ostream& oss) const {
  oss << *this;
  return oss;
}

//Serialize method
//-----------------------------------------------
template <class Archive>
void
I3ComplexVector::serialize(Archive& ar, unsigned version) {
  if (version > i3complexvector_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3ComplexVector class.", version, i3complexvector_version_);

  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("X", x_);
  ar & make_nvp("Y", y_);
  ar & make_nvp("Z", z_);
}

I3_SERIALIZABLE(I3ComplexVector);