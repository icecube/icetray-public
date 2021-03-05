
#ifndef I3COMPLEXVECTOR_H_INCLUDED
#define I3COMPLEXVECTOR_H_INCLUDED

#include <cmath>
#include <complex>
#include "icetray/I3FrameObject.h"
#include <dataclasses/Utility.h>
#include <sstream>
#include "dataclasses/external/CompareFloatingPoint.h"

/**
 * @brief Complex implementation of I3Position
 */
static const unsigned i3complexvector_version_ = 0;

//Forward declaration
class I3Direction;
class I3Position;

class I3ComplexVector : public I3FrameObject {
 public:

  /**
   * Default constructor
   */
  I3ComplexVector():
    x_(NAN),
    y_(NAN),
    z_(NAN)
  {}

  /**
   * Constructor from cartesian coordinates
   */
  I3ComplexVector(std::complex<double> x, std::complex<double> y, std::complex<double> z):
    x_(x),
    y_(y),
    z_(z)
  {}

  /**
   * Copy constructor
   */
  I3ComplexVector(const I3ComplexVector& p):
    x_(p.x_),
    y_(p.y_),
    z_(p.z_)
  {}

  explicit I3ComplexVector(const I3Direction& dir);

  explicit I3ComplexVector(const I3Position& pos);

  //--------------

  /**
   * Provide X of position in cartesian ref frame
   */
  inline std::complex<double> GetX() const {return x_;}

  /**
   * Provide Y of position in cartesian ref frame
   */
  inline std::complex<double> GetY() const {return y_;}

  /**
   * Provide Z of position in cartesian ref frame
   */
  inline std::complex<double> GetZ() const {return z_;}

  /**
   * Set X position while keeping Y,Z constant.
   */
  inline void SetX(std::complex<double> x) {
    x_ = x;
  }

  /**
   * Set Y position while keeping X,Z constant.
   */
  inline void SetY(std::complex<double> y) {
    y_ = y;
  }

  /**
   * Set Z position while keeping X,Y constant.
   */
  inline void SetZ(std::complex<double> z) {
    z_ = z;
  }

  //--------------

  /**
   * Rotate position around X axis by angle
   */
  I3ComplexVector RotateX(double angle);

  /**
   * Rotate position around Y axis by angle
   */
  I3ComplexVector RotateY(double angle);

  /**
   * Rotate position around Z axis by angle
   */
  I3ComplexVector RotateZ(double angle);

  /**
   * Computes the distance from this position to the origin of the
   * coordinate system (it's magnitude as a vector)
   */
  double Magnitude() const {
    return std::sqrt(std::pow(std::abs(x_), 2) +
                     std::pow(std::abs(y_), 2) +
                     std::pow(std::abs(z_), 2));
  }

  I3ComplexVector GetUnitVector() const;

  void Normalize();

  /**
   * Vector inversion (makes the vector point in the opposite direction)
   */
  I3ComplexVector operator-() const {
    I3ComplexVector p;
    p.x_ = -x_;
    p.y_ = -y_;
    p.z_ = -z_;
    return p;
  }
  ///////////////////////////////////////////////
  /**
   * Vector addition
   */
  I3ComplexVector& operator+=(const I3ComplexVector& rhs) {
    x_ += rhs.x_;
    y_ += rhs.y_;
    z_ += rhs.z_;
    return *this;
  }

  /**
   * Vector subtraction
   */
  I3ComplexVector& operator-=(const I3ComplexVector& rhs) {
    x_ -= rhs.x_;
    y_ -= rhs.y_;
    z_ -= rhs.z_;
    return *this;
  }

  /**
   * Vector addition
   */
  I3ComplexVector operator+(const I3ComplexVector& rhs) const {
    return I3ComplexVector(*this) += rhs;
  }

  /**
   * Vector subtraction
   */
  I3ComplexVector operator-(const I3ComplexVector& rhs) const {
    return I3ComplexVector(*this) -= rhs;
  }

  /**
   * Scalar (dot) product
   */
  std::complex<double> operator*(const I3ComplexVector& rhs) const {
    return x_ * std::conj(rhs.x_) + y_ * std::conj(rhs.y_) + z_ * std::conj(rhs.z_);
  }

  std::complex<double> Dot(const I3ComplexVector& rhs) const {
    return *this * rhs;
  }
  ///////////////////////////////////////////////

  /**
   * Multiplication by a single value
   */
  I3ComplexVector& operator*=(std::complex<double> a) {
    x_ *= a;
    y_ *= a;
    z_ *= a;
    return *this;
  }

  /**
   * Divison by a single value
   */
  I3ComplexVector& operator/=(std::complex<double> a) {
    x_ /= a;
    y_ /= a;
    z_ /= a;
    return *this;
  }

  /**
   * Multiplication by a single value
   */
  I3ComplexVector operator*(std::complex<double> a) const {
    return I3ComplexVector(*this) *= a;
  }

  /**
   * Division by a single value
   */
  I3ComplexVector operator/(std::complex<double> a) const {
    return I3ComplexVector(*this) /= a;
  }
  ///////////////////////////////////////////////

  /**
   * Multiplication by a single value
   */
  I3ComplexVector& operator*=(double a) {
    x_ *= a;
    y_ *= a;
    z_ *= a;
    return *this;
  }

  /**
   * Divison by a single value
   */
  I3ComplexVector& operator/=(double a) {
    x_ /= a;
    y_ /= a;
    z_ /= a;
    return *this;
  }

  /**
   * Multiplication by a single value
   */
  I3ComplexVector operator*(double a) const {
    return I3ComplexVector(*this) *= a;
  }

  /**
   * Division by a single value
   */
  I3ComplexVector operator/(double a) const {
    return I3ComplexVector(*this) /= a;
  }
  ///////////////////////////////////////////////


  bool operator==(const I3ComplexVector& rhs) const {
    return (x_ == rhs.x_ &&
            y_ == rhs.y_ &&
            z_ == rhs.z_
           );
  }

  bool operator!=(const I3ComplexVector& rhs) const {
    return !(*this == rhs);
  }

  std::ostream& Print(std::ostream&) const override;

  friend std::ostream &operator<<(std::ostream &os, const I3ComplexVector &rhs) {
    os << "I3ComplexVector(" << rhs.x_ << ", " << rhs.y_ << ", " << rhs.z_ << ")";
    return os;
  }

 protected:
  /**
   * cartesian (car)
   */
  std::complex<double> x_;
  std::complex<double> y_;
  std::complex<double> z_;


 private:

  friend class icecube::serialization::access;

  template<class Archive>
  void serialize(Archive &ar, unsigned int version);

};

std::ostream& operator<<(std::ostream& oss, const I3ComplexVector& p);

double abs(const I3ComplexVector& p);

I3_POINTER_TYPEDEFS(I3ComplexVector);
I3_CLASS_VERSION(I3ComplexVector, i3complexvector_version_);

#endif //I3POSITION_H_INCLUDED
