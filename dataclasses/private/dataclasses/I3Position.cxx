
// $Id$


#include <iostream>
#include <icetray/serialization.h>
#include "dataclasses/I3Position.h"
#include "dataclasses/I3Direction.h"
#include "dataclasses/I3Constants.h"
#include "icetray/I3Units.h"

using namespace I3Constants;
using namespace I3Units;

//Serialize method
//-----------------------------------------------
template <class Archive>
void
I3Position::serialize(Archive& ar, unsigned version)
{
  if (version>i3position_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3Position class.",version,i3position_version_);

  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("X", x_);
  ar & make_nvp("Y", y_);
  ar & make_nvp("Z", z_);
}

I3_SERIALIZABLE(I3Position);

//-----------------------------------------------------------
I3Position::I3Position(const I3Direction& d):
x_(d.GetX()),
y_(d.GetY()),
z_(d.GetZ())
{}

//-----------------------------------------------------------
I3Position::I3Position(double r1, double r2, double r3, RefFrame frame){
  // Store position according to reference frame f

  switch (frame) {
    case car: // Input given in Cartesian coordinates
      x_=r1;
      y_=r2;
      z_=r3;
      break;

    case sph: // Input given in Spherical coordinates
      CalcCarFromSph(r1, r2, r3);
      break;

    case cyl: // Input given in Cylindrical coordinates
      z_=r3;
      CalcCarFromCyl(r1, r2);
      break;

    default: // Unsupported reference frame
      log_fatal("Unsupported reference frame passed to I3Position::SetPosition: %i",frame);
      break;
  }
}

//-------------------------------------------------------

double I3Position::GetR() const{
  return std::sqrt(x_*x_+y_*y_+z_*z_);
}

  /**
   * Provide Theta of position in spherical ref frame
   * If non-cartesian have not been calculated, then calculate them first
   */
double I3Position::GetTheta() const{
  const double r_=GetR();
  double theta_=0;
  if ((r_!=0.) && std::abs(z_/r_)<=1.) {
  theta_=std::acos(z_/r_);
  } else {
  if (z_<0.) theta_=pi;
  }
  if (theta_<0.) theta_+=2.*pi;

  return theta_;
  }

  /**
   * Provide Phi of position in spherical or cylindrical ref frame
   * If non-cartesian have not been calculated, then calculate them first
   */
double I3Position::GetPhi() const{
  double phi_=0;
  if ((x_!=0.f) || (y_!=0.f)) phi_=std::atan2(y_,x_);
  if (phi_<0.) phi_+=2.*pi;
  return phi_;
}

/**
 * Provide Rho of position in cylindrical ref frame
 * If non-cartesian have not been calculated, then calculate them first
 */
double I3Position::GetRho() const{
  return GetR()*std::sin(GetTheta());
}

//-----------------------------------------------------------
void I3Position::RotateX(double angle)
{
// Rotate around x-axis by angle
  const double s=std::sin(angle);
  const double c=std::cos(angle);
  const double y=y_;
  const double z=z_;
  y_=c*y-s*z;
  z_=s*y+c*z;
}

//-----------------------------------------------------------
void I3Position::RotateY(double angle)
{
// Rotate around y-axis by angle
  const double s=std::sin(angle);
  const double c=std::cos(angle);
  const double x=x_;
  const double z=z_;
  z_=c*z-s*x;
  x_=s*z+c*x;
}

//-----------------------------------------------------------
void I3Position::RotateZ(double angle)
{
// Rotate around z-axis by angle
  const double s=std::sin(angle);
  const double c=std::cos(angle);
  const double x=x_;
  const double y=y_;
  x_=c*x-s*y;
  y_=s*x+c*y;
}

//-----------------------------------------------------------
I3Position I3Position::GetUnitVector() const
{
  double mag = Magnitude();
  if (0 != mag) {
    return *this / mag;
  }
  return *this;
}

//-----------------------------------------------------------
void I3Position::Normalize()
{
  double mag = Magnitude();
  if (0 != mag)
    *this *= 1. / mag;
}

//-----------------------------------------------------------
double I3Position::GetAngularSeparation(const I3Position & pos) const
{
  double dot = pos.GetUnitVector() * GetUnitVector();
  return acos(dot);
}

//-----------------------------------------------------------
double I3Position::operator*(const I3Direction& rhs) const{
  return x_*rhs.GetX() + y_*rhs.GetY() + z_*rhs.GetZ();
}

//-----------------------------------------------------------
I3Position I3Position::Cross(const I3Direction& d) const{
  return I3Position (y_*d.GetZ() - z_*d.GetY(),
                     z_*d.GetX() - x_*d.GetZ(),
                     x_*d.GetY() - y_*d.GetX());
}

//-----------------------------------------------------------
void I3Position::CalcCarFromSph(double r_, double theta_, double phi_)
{
  // Calculate Cartesian  coordinates from Spherical
  x_=r_*std::sin(theta_)*std::cos(phi_);
  y_=r_*std::sin(theta_)*std::sin(phi_);
  z_=r_*std::cos(theta_);
}

//-----------------------------------------------------------
void I3Position::CalcCarFromCyl(double rho_, double phi_)
{
  // Calculate Cartesian coordinates from Cylindrical
  x_=rho_*std::cos(phi_);
  y_=rho_*std::sin(phi_);
}

//-----------------------------------------------------------
I3Position operator*(double a, const I3Position& p){
  return(p*a);
}

//-----------------------------------------------------------
std::ostream& operator<<(std::ostream& oss, const I3Position& p){
  return(p.Print(oss));
}
//-----------------------------------------------------------
std::ostream& I3Position::Print(std::ostream& oss) const{
  oss << "I3Position(" << GetX() << "," << GetY() << "," << GetZ() << ")";
  return oss;
}

//-----------------------------------------------------------
double abs(const I3Position& p){
	return(p.Magnitude());
}
