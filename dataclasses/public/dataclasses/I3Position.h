/**
 * copyright  (C) 2004
 * the icecube collaboration
 * @version $Id$
 * @file I3Position.h
 * @date $Date$
 */

//***********************************************************
//-- Created: Dusan Turcan UMD 26-05-2004
//   Taken from: Nick van Eijndhoven 06-feb-1999 UU-SAP Utrecht
//***********************************************************

// $Id$

#ifndef I3POSITION_H_INCLUDED
#define I3POSITION_H_INCLUDED

#include <cmath>

#include "icetray/I3FrameObject.h"
#include "Utility.h"
#include <sstream>
#include "dataclasses/external/CompareFloatingPoint.h"

/**
 * @brief The basic position class for IceCube.
 *
 * All positions in IceCube should be written with this class.
 * Positions can be given in cartesian, spherical, or cylindrical coordinates.
 *
 * @todo implement "print out" of all information in a uniform way...
 * @todo insure that the temporary data isn't written to disk.
 */
static const unsigned i3position_version_ = 0;

//Forward declaration
class I3Direction;

class I3Position : public I3FrameObject
{
 public:

  /**
   * Possible reference frames.
   */
  enum RefFrame { car = 0, sph = 1, cyl = 2 };

  //--------------

  /**
   * Default constructor
   */
  I3Position():
  x_(NAN),
  y_(NAN),
  z_(NAN)
  {}

  /**
   * Constructor for different coordinate systems
   *
   * The meaning of this constructor depends on the value of RefFrame.
   * If it is I3Position::car the three coordinates are treated as
   *   cartesian x, y, and z
   * If it is I3Position::sph the three coordinates are treated as
   *   spherical r, theta, and phi
   * If it is I3Position::cyl the three coordinates are treated as
   *   cylindrical rho, phi, and z
   */
	I3Position(double x, double y, double z, RefFrame f);

  /**
   * Constructor from cartesian coordinates
   */
  I3Position(double x, double y, double z):
  x_(x),
  y_(y),
  z_(z)

  {}

  /**
   * Copy constructor
   */
  I3Position(const I3Position& p):
  x_(p.x_),
  y_(p.y_),
  z_(p.z_)

  {}

  explicit I3Position(const I3Direction& d);

  //--------------
  
  std::ostream& Print(std::ostream&) const override;

  /**
   * Provide X of position in cartesian ref frame
   */
  inline double GetX() const {return x_;}

  /**
   * Provide Y of position in cartesian ref frame
   */
  inline double GetY() const {return y_;}

  /**
   * Provide Z of position in cartesian ref frame
   */
  inline double GetZ() const {return z_;}

  /**
   * Provide R of position in spherical ref frame
   * If non-cartesian have not been calculated, then calculate them first
   */
  double GetR() const;

  /**
   * Provide Theta of position in spherical ref frame
   * If non-cartesian have not been calculated, then calculate them first
   */
  double GetTheta() const;

  /**
   * Provide Phi of position in spherical or cylindrical ref frame
   * If non-cartesian have not been calculated, then calculate them first
   */
  double GetPhi() const;

  /**
   * Provide Rho of position in cylindrical ref frame
   * If non-cartesian have not been calculated, then calculate them first
   */
  double GetRho() const;

  //--------------

  /**
   * Set X position while keeping Y,Z constant.  Recalculate SPH and CYL.
   */
  inline void SetX(double x) {
    x_=x;
  }

  /**
   * Set Y position while keeping X,Z constant.  Recalculate SPH and CYL.
   */
  inline void SetY(double y) {
    y_=y;
  }

  /**
   * Set Z position while keeping X,Y constant.  Recalculate SPH and CYL.
   */
  inline void SetZ(double z) {
    z_=z;
  }

  //--------------

  /**
   * Rotate position around X axis by angle
   */
  void RotateX(double angle);

  /**
   * Rotate position around Y axis by angle
   */
  void RotateY(double angle);

  /**
   * Rotate position around Z axis by angle
   */
  void RotateZ(double angle);

  /**
   * Computes the distance from this position to the origin of the
   * coordinate system (it's magnitude as a vector)
   */
  double Magnitude() const{
    return sqrt(*this * *this);
  }

  /**
   * Computes the square of the vector magnitude of the position
   */
  double Mag2() const{
    return *this * *this;
  }

  I3Position GetUnitVector() const;

  void Normalize();

  double GetAngularSeparation(const I3Position &) const;

  /**
   * Vector inversion (makes the vector point in the opposite direction)
   */
  I3Position operator-() const{
    I3Position p;
    p.x_=-x_;
    p.y_=-y_;
    p.z_=-z_;
    return p;
  }

  /**
   * Vector addition
   */
  I3Position& operator+=(const I3Position& rhs){
    x_+=rhs.x_;
    y_+=rhs.y_;
    z_+=rhs.z_;
    return *this;
  }

  /**
   * Vector subtraction
   */
  I3Position& operator-=(const I3Position& rhs){
    x_-=rhs.x_;
    y_-=rhs.y_;
    z_-=rhs.z_;
    return *this;
  }

  /**
   * Vector addition
   */
  I3Position operator+(const I3Position& rhs) const{
    return I3Position(*this)+=rhs;
  }

  /**
   * Vector subtraction
   */
  I3Position operator-(const I3Position& rhs) const{
    return I3Position(*this)-=rhs;
  }

  /**
   * Scalar (dot) product
   */
  double operator*(const I3Position& rhs) const{
    return x_*rhs.x_ + y_*rhs.y_ + z_*rhs.z_;
  }

  double Dot(const I3Position& rhs) const {
    return *this * rhs;
  }

  /**
   * Scalar (dot) product
   */
  double operator*(const I3Direction&) const;

  double Dot(const I3Direction& rhs) const {
    return *this * rhs;
  }

  /**
   * Multiplication by a scalar
   */
  I3Position& operator*=(double a){
    x_*=a;
    y_*=a;
    z_*=a;
    return *this;
  }

  /**
   * Divison by a scalar
   */
  I3Position& operator/=(double a){
    x_/=a;
    y_/=a;
    z_/=a;
    return *this;
  }

  /**
   * Multiplication by a scalar
   */
  I3Position operator*(double a) const{
    return I3Position(*this)*=a;
  }

  /**
   * Division by a scalar
   */
  I3Position operator/(double a) const{
    return I3Position(*this)/=a;
  }

  /**
   * Vector (cross) product
   */
  I3Position Cross(const I3Position& d) const{
    return I3Position (y_*d.z_ - z_*d.y_,
                       z_*d.x_ - x_*d.z_,
                       x_*d.y_ - y_*d.x_);
  }

  /**
   * Vector (cross) product
   */
  I3Position Cross(const I3Direction&) const;

  bool operator==(const I3Position& rhs) const {
    return (CompareFloatingPoint::Compare_NanEqual(x_, rhs.x_) &&
            CompareFloatingPoint::Compare_NanEqual(y_, rhs.y_) &&
            CompareFloatingPoint::Compare_NanEqual(z_, rhs.z_)
    );
  }
  bool operator!=(const I3Position& rhs) const {
    return !(*this == rhs);
  }

 protected:
  /**
   * cartesian (car)
   */
  double x_;
  double y_;
  double z_;


 private:

  friend class icecube::serialization::access;

  template <class Archive>
  void serialize(Archive& ar, unsigned version);

  void CalcCarFromSph(double r_, double theta_, double phi_);
  void CalcCarFromCyl(double rho_, double phi_);

};

I3Position operator*(double, const I3Position&);

std::ostream& operator<<(std::ostream& oss, const I3Position& p);

double abs(const I3Position& p);

I3_POINTER_TYPEDEFS(I3Position);
I3_CLASS_VERSION(I3Position, i3position_version_);

#endif //I3POSITION_H_INCLUDED
