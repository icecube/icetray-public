/**
 * copyright  (C) 2004
 * the icecube collaboration
 * @version $Id$
 * @file I3Direction.h
 * @date $Date$
 */

//***********************************************************
//-- Created: Dusan Turcan, UMD, Sep 2, 2004
//   Done similarly to I3Position
//***********************************************************

// $Id$

#ifndef I3DIRECTION_H_INCLUDED
#define I3DIRECTION_H_INCLUDED

#include <icetray/I3FrameObject.h>
#include <dataclasses/I3Constants.h>
#include "Utility.h"
#include <dataclasses/I3Vector.h>
#include <dataclasses/external/CompareFloatingPoint.h>

static const unsigned i3direction_version_ = 0;

//Forward declaration
class I3Position;

/**
 * @brief The basic direction class for IceCube. 
 *
 * All directions in IceCube should be written with this class.
 * Directions can be given in zenith/azimuth or direction cosines.
 * Optionally, they can also be set with theta/phi.
 * 
 * Theta/Phi angles refer to the direction of travel of the particle -- the
 * direction that the particle is going.  If you consider the direction vector
 * which starts at the origin, theta is the angle from the +z axis to that
 * vector; phi is the angle from the +x axis to the x-y projection of that
 * vector.
 *
 * The direction cosines (x,y,z) refer to this same direction vector -- the
 * vector of the direction of travel of the particle; they simply describe
 * the end point of that vector.  When you set a direction with the direction
 * cosines, they don't have to be normalized, but when you get them out (with
 * GetX(), GetY(), GetZ()) they will be normalized to 1.
 *
 * Zenith/Azimuth angles refer to the direction which is opposite to the
 * particle's direction; i.e. they refer to the direction the particle (track)
 * came from.  If you consider this "opposite" direction vector, zenith is the
 * angle from the +z axis to that vector; azimuth is the angle from the
 * +x axis to the x-y projection of that vector.
 *
 * See the graphic of the definitions of Zenith/Azimuth and theta/Phi 
 * direction angles:
 * https://wiki.icecube.wisc.edu/index.php/I3Direction
 *
 * @todo implement "print out" of all information in a uniform way...
 * @todo ensure that the temporary data isn't written to disk.
 */
class I3Direction : public I3FrameObject
{
 public:

  I3Direction():
  zenith_(NAN),
  azimuth_(NAN),
  xDir_(NAN),
  yDir_(NAN),
  zDir_(NAN),
  isCalculated_(false)
  {}

  /**
   * Additional constructor: 2 arguments mean construct dir. with zen,azi
   */
  I3Direction(double zen, double azi):
  zenith_(zen),
  azimuth_(azi),
  xDir_(NAN),
  yDir_(NAN),
  zDir_(NAN),
  isCalculated_(false)
  {}

  /**
   * Additional constructor: 3 arguments mean construct dir. with x,y,z
   */
  I3Direction(double x, double y, double z):
  zenith_(NAN),
  azimuth_(NAN),  
  xDir_(x),
  yDir_(y),
  zDir_(z),
  isCalculated_(false)    
  {
    CalcSphFromCar();
  }
  
  explicit I3Direction(const I3Position&);
  
  std::ostream& Print(std::ostream&) const override;

  //--------------

  /**
   * Store direction with theta, phi
   */
  void SetThetaPhi(double theta, double phi);

  //--------------

  /**
   * Provide Zenith of direction
   */
  inline double GetZenith() const {return zenith_;}

  /**
   * Provide Azimuth of direction
   */
  inline double GetAzimuth() const {return azimuth_;}

  /**
   * Provide X of direction in cartesian ref frame
   */
  inline double GetX() const {
    if (!isCalculated_) CalcCarFromSph();
    return xDir_;
  }

  /**
   * Provide Y of direction in cartesian ref frame
   */
  inline double GetY() const {
    if (!isCalculated_) CalcCarFromSph();
    return yDir_;
  }

  /**
   * Provide Z of direction in cartesian ref frame
   */
  inline double GetZ() const {
    if (!isCalculated_) CalcCarFromSph();
    return zDir_;
  }

  /**
   * Calculate Theta of direction
   */
  inline double CalcTheta() const {
    return I3Constants::pi - static_cast<double>(zenith_);
  }

  /**
   * Calculate Phi of direction
   */
  inline double CalcPhi() const {
    double phi = I3Constants::pi + static_cast<double>(azimuth_);
    if (phi >= 2.*I3Constants::pi) phi -= 2.*I3Constants::pi;
    return phi;
  }


  //--------------

  /**
   * Rotate direction around X axis by angle
   */
  void RotateX(double angle);

  /**
   * Rotate direction around Y axis by angle
   */
  void RotateY(double angle);

  /**
   * Rotate direction around Z axis by angle
   */
  void RotateZ(double angle);
  
  /**
   * Vector inversion (makes the vector point in the opposite direction)
   */
  I3Direction operator-() const{
    I3Direction d;
    d.zenith_=I3Constants::pi-zenith_;
    d.azimuth_=azimuth_+I3Constants::pi;
    if(d.azimuth_>=2*I3Constants::pi)
      d.azimuth_-=2*I3Constants::pi;
    d.isCalculated_=false;
    return d;
  }

  /**
   * Cross product of this x d
   */
  I3Direction Cross(const I3Direction& d) const{
    if (!isCalculated_) CalcCarFromSph();
    return I3Direction (yDir_*d.GetZ() - zDir_*d.GetY(),
                        zDir_*d.GetX() - xDir_*d.GetZ(),
                        xDir_*d.GetY() - yDir_*d.GetX());
  }
  
  /**
   * Vector (cross) product
   */
  I3Position Cross(const I3Position&) const;

  /**
   * Scalar (dot) product
   */
  double operator*(const I3Direction& other) const{
    if(isCalculated_ && other.isCalculated_)
      return xDir_*other.xDir_ + yDir_*other.yDir_ + zDir_*other.zDir_;
    //if one of the directions doesn't already have cartesian
    //coordinates calculated, calculating them would be less efficient
    double cad=cos(azimuth_ - other.azimuth_);
    double czd=cos(zenith_ - other.zenith_);
    double czs=cos(zenith_ + other.zenith_);
    return 0.5*(cad*(czd-czs)+czd+czs);
  }
  
  /**
   * Scalar (dot) product
   */
  double operator*(const I3Position& other) const;
  
  /**
   * Multiplication by a scalar
   */
  I3Position operator*(double) const;
  
  /**
   * Division by a scalar
   */
  I3Position operator/(double) const;

  bool operator==(const I3Direction& rhs) const {
    return (CompareFloatingPoint::Compare_NanEqual(zenith_, rhs.zenith_) &&
            CompareFloatingPoint::Compare_NanEqual(azimuth_, rhs.azimuth_)
    );
  }
  bool operator!=(const I3Direction& rhs) const {
    return !(*this == rhs);
  }

  //---
  
  /** returns the angle between in two I3Directions
   * @return the angle in rad (I3native units)
   */
  inline double Angle(const I3Direction& rhs) const
  {
    return acos(*this*rhs);
  }

 protected:
  /**
   * direction coordinates -- spherical
   */ 
  double zenith_;
  double azimuth_;

  /**
   * direction coordinates -- cartesian (direction cosines)
   */ 
  mutable double xDir_; //!
  mutable double yDir_; //!
  mutable double zDir_; //!

  /**
   * Did we calculate the directions before?
   */
  mutable bool isCalculated_; //!

 private:
  /**
   * Change zenith,azimuth coordinates into x,y,z directional coordinates.
   * The three numbers x,y,z are calculated to add up (in quadrature) to 1.
   * theta=pi-zenith and phi=azimuth-pi in these IceCube coordinates.
   */
  void CalcCarFromSph() const;

  /**
   * Change x,y,z directional coordinates to zenith,azimuth coordinates.
   * zenith=pi-theta and azimuth=phi+pi in these IceCube coordinates.
   * The three numbers DO NOT have to add up (in quadrature) to 1.
   * Even if they don't, the direction that they define is stored.
   * SO BE CAREFUL IF YOU SPECIFY THE DIRECTION IN THIS WAY!
   */
  void CalcSphFromCar();

  friend class icecube::serialization::access;

  template <class Archive>
  void serialize(Archive& ar, unsigned version);
};

I3Position operator*(double, const I3Direction&);

std::ostream& operator<<(std::ostream& oss, const I3Direction& d);

I3_POINTER_TYPEDEFS(I3Direction);
I3_CLASS_VERSION(I3Direction, i3direction_version_);

typedef I3Vector<I3Direction> I3DirectionVect;
I3_POINTER_TYPEDEFS(I3DirectionVect);

#endif //I3DIRECTION_H_INCLUDED
