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
  z_(NAN),
  r_(NAN),
  theta_(NAN),
  phi_(NAN),
  rho_(NAN),
  isCalculated_(false)
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
  z_(z),
  r_(NAN),    
  theta_(NAN),
  phi_(NAN),
  rho_(NAN),    
  isCalculated_(false)
  {}

  /**
   * Copy constructor
   */
  I3Position(const I3Position& p):
  x_(p.x_),
  y_(p.y_),
  z_(p.z_),
  r_(NAN),    
  theta_(NAN),
  phi_(NAN),
  rho_(NAN),    
  isCalculated_(false)
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
  inline double GetR() const {
    if (!isCalculated_) CalcSphCylFromCar();
    return r_;
  }

  /**
   * Provide Theta of position in spherical ref frame
   * If non-cartesian have not been calculated, then calculate them first
   */
  inline double GetTheta() const {
    if (!isCalculated_) CalcSphCylFromCar();
    return theta_;
  }

  /**
   * Provide Phi of position in spherical or cylindrical ref frame
   * If non-cartesian have not been calculated, then calculate them first
   */
  inline double GetPhi() const {
    if (!isCalculated_) CalcSphCylFromCar();
    return phi_;
  }

  /**
   * Provide Rho of position in cylindrical ref frame
   * If non-cartesian have not been calculated, then calculate them first
   */
  inline double GetRho() const {
    if (!isCalculated_) CalcSphCylFromCar();
    return rho_;
  }

  //--------------

  /**
   * Set X position while keeping Y,Z constant.  Recalculate SPH and CYL.
   */
  inline void SetX(double x) {
    x_=x;
    isCalculated_=false; // when accessing CYL/SPH, they will be recalculated
  }

  /**
   * Set Y position while keeping X,Z constant.  Recalculate SPH and CYL.
   */
  inline void SetY(double y) {
    y_=y;
    isCalculated_=false; // when accessing CYL/SPH, they will be recalculated
  }

  /**
   * Set Z position while keeping X,Y constant.  Recalculate SPH and CYL.
   */
  inline void SetZ(double z) {
    z_=z;
    isCalculated_=false; // when accessing CYL/SPH, they will be recalculated
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
    if(isCalculated_)
      return r_;
    //otherwise use self dot-product
    return sqrt(*this * *this);
  }

  /**
   * Computes the square of the vector magnitude of the position
   */
  double Mag2() const{
    if(isCalculated_)
      return r_*r_;
    //otherwise use self dot-product
    return *this * *this;
  }

  /**
   * Vector inversion (makes the vector point in the opposite direction)
   */
  I3Position operator-() const{
    I3Position p;
    p.x_=-x_;
    p.y_=-y_;
    p.z_=-z_;
    p.isCalculated_=false;
    return p;
  }

  /**
   * Vector addition
   */
  I3Position& operator+=(const I3Position& rhs){
    x_+=rhs.x_;
    y_+=rhs.y_;
    z_+=rhs.z_;
    isCalculated_=false;
    return *this;
  }

  /**
   * Vector subtraction
   */
  I3Position& operator-=(const I3Position& rhs){
    x_-=rhs.x_;
    y_-=rhs.y_;
    z_-=rhs.z_;
    isCalculated_=false;
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

  /**
   * Scalar (dot) product
   */
  double operator*(const I3Direction&) const;

  /**
   * Multiplication by a scalar
   */
  I3Position& operator*=(double a){
    x_*=a;
    y_*=a;
    z_*=a;
    isCalculated_=false;
    return *this;
  }

  /**
   * Divison by a scalar
   */
  I3Position& operator/=(double a){
    x_/=a;
    y_/=a;
    z_/=a;
    isCalculated_=false;
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

  /**
   * spherical (sph)
   */
  mutable double r_;
  mutable double theta_;
  mutable double phi_;

  /**
   * cylindrical (cyl) - Z and Phi are same.
   */
  mutable double rho_;

  /**
   * Whether the coordinates in secondary coordinates systems
   * (sph and cyl) are already computed
   */
  mutable bool isCalculated_;

 private:

  friend class icecube::serialization::access;

  template <class Archive>
  void serialize(Archive& ar, unsigned version);

  void CalcSphCylFromCar() const;
  void CalcCarCylFromSph();
  void CalcCarSphFromCyl();

};

I3Position operator*(double, const I3Position&);

std::ostream& operator<<(std::ostream& oss, const I3Position& p);

double abs(const I3Position& p);

I3_POINTER_TYPEDEFS(I3Position);
I3_CLASS_VERSION(I3Position, i3position_version_);

#endif //I3POSITION_H_INCLUDED
