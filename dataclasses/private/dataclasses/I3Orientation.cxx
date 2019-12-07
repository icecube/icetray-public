// $Id$

#include <iostream>
#include <icetray/serialization.h>
#include "dataclasses/I3Orientation.h"
#include "dataclasses/I3Constants.h"

#include <math.h>
#include <float.h>
#include <algorithm> 

template <class Archive> 
void 
I3Orientation::serialize(Archive& ar, unsigned version)
{
	if (version>i3orientation_version_)
		log_fatal("Attempting to read version %u from file but running version %u of I3Orientation class.",version,i3orientation_version_);
	
	ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
	ar & make_nvp("Rotation", rot_);
}

I3_SERIALIZABLE(I3Orientation);
I3_SERIALIZABLE(I3OrientationVect);

std::ostream& operator<<(std::ostream& os, const I3Orientation& ori){
	return(ori.Print(os));
}

//-----------------------------------------------------------
I3Orientation::I3Orientation()
{
	// Creation of an I3Orientation object and initialization of parameters
	ResetOrientation();
}

//-----------------------------------------------------------
I3Orientation::I3Orientation(double x_dir, double y_dir, double z_dir, double x_up, double y_up, double z_up)
{
	// Creation of an I3Orientation object and initialization of parameters
	SetOrientation(x_dir, y_dir, z_dir, x_up, y_up, z_up);
}

//-----------------------------------------------------------
I3Orientation::I3Orientation(const I3Direction &dir, const I3Direction &up)
{
	SetOrientation(dir.GetX(), dir.GetY(), dir.GetZ(), up.GetX(), up.GetY(), up.GetZ());
}

//-----------------------------------------------------------
I3Orientation::I3Orientation(const I3Orientation& d)
{
	// Copy constructor
	rot_ = d.rot_;
	isDirCalculated_=false;
	isUpCalculated_=false;
	isRightCalculated_=false;
}

//-----------------------------------------------------------
I3Orientation::~I3Orientation()
{
	// Destructor to delete dynamically allocated memory
}

//-----------------------------------------------------------
void I3Orientation::SetOrientation(const I3Direction& d)
{
	const double dx = d.GetX();
	const double dy = d.GetY();
	const double dz = d.GetZ();
	
	// get some orthogonal direction
	double ox, oy, oz;
	const double x = dx < 0.0 ? -dx : dx;
	const double y = dy < 0.0 ? -dy : dy;
	const double z = dz < 0.0 ? -dz : dz;
	if (x < y) {
		if (x < z) {
			ox = 0.; oy = dz; oz=-dy;
		} else {
			ox = dy; oy = -dx; oz=0.;
		}
	} else {
		if (y < z) {
			ox = -dz; oy = 0.; oz=dx;
		} else {
			ox = dy; oy = -dx; oz=0.;
		}
	}
	
	SetOrientation(dx, dy, dz, ox, oy, oz);
}

//-----------------------------------------------------------
void I3Orientation::SetOrientation(const I3Orientation& d)
{
	rot_ = d.rot_;
	isDirCalculated_=false;
	isUpCalculated_=false;
	isRightCalculated_=false;
}

//-----------------------------------------------------------
void I3Orientation::SetOrientation(double x_dir, double y_dir, double z_dir, 
								   double x_up, double y_up, double z_up)
{
	const double del = 0.001;
	
	// the vectors must not be too short!
	const double dir_mag2 = x_dir*x_dir + y_dir*y_dir + z_dir*z_dir;
	const double up_mag2 = x_up*x_up + y_up*y_up + z_up*z_up;
	
	if (dir_mag2 < 1e-6) log_fatal("direction vector (\"dir\") too short! (length should be 1)");
	if (up_mag2 < 1e-6) log_fatal("direction vector (\"up\") too short! (length should be 1)");
	
	// normalize the vectors
	const double dir_mag = std::sqrt(dir_mag2);
	const double up_mag = std::sqrt(up_mag2);
	const double newX_x = x_up / up_mag;
	const double newX_y = y_up / up_mag;
	const double newX_z = z_up / up_mag;
	const double newZ_x = x_dir / dir_mag;
	const double newZ_y = y_dir / dir_mag;
	const double newZ_z = z_dir / dir_mag;
	
	// the vectors must be perpendicular
	if (newZ_x*newX_x + newZ_y*newX_y + newZ_z*newX_z > del) log_fatal("\"dir\" and \"up\" vectors are not perpendicular!");
	
	// newY = newZ x newX
	const double newY_x = newZ_y*newX_z - newZ_z*newX_y;
	const double newY_y = newZ_z*newX_x - newZ_x*newX_z;
	const double newY_z = newZ_x*newX_y - newZ_y*newX_x;
	
	double q[4] = {0.,0.,0.,0.};
	// matrix
	const double M[3][3] = {{newX_x, newY_x, newZ_x},
							{newX_y, newY_y, newZ_y},
							{newX_z, newY_z, newZ_z}};
	
	// calculate the quaternion from the matrix
	q[3] = std::sqrt( std::max( 0., 1. + M[0][0] + M[1][1] + M[2][2] ) ) / 2.; 
	q[0] = std::sqrt( std::max( 0., 1. + M[0][0] - M[1][1] - M[2][2] ) ) / 2.; 
	q[1] = std::sqrt( std::max( 0., 1. - M[0][0] + M[1][1] - M[2][2] ) ) / 2.; 
	q[2] = std::sqrt( std::max( 0., 1. - M[0][0] - M[1][1] + M[2][2] ) ) / 2.; 
	q[0] = copysign( q[0], M[2][1] - M[1][2] );
	q[1] = copysign( q[1], M[0][2] - M[2][0] );
	q[2] = copysign( q[2], M[1][0] - M[0][1] );

	rot_.set(q[0], q[1], q[2], q[3]); // x,y,z,w
	
	isDirCalculated_=false;
	isUpCalculated_=false;
	isRightCalculated_=false;
}

//-----------------------------------------------------------
void I3Orientation::SetOrientation(const I3Direction &dir, const I3Direction &up)
{
	SetOrientation(dir.GetX(), dir.GetY(), dir.GetZ(), up.GetX(), up.GetY(), up.GetZ());
}

void I3Orientation::SetOrientationFromQuaternion(const I3Quaternion &q)
{
	if (fabs(q.norm()-1.) > 1e-6) log_fatal("Quaternion norm==%f != 1!", q.norm());
	
	rot_=q;
	isDirCalculated_=false;
	isUpCalculated_=false;
	isRightCalculated_=false;
}

//-----------------------------------------------------------
void I3Orientation::ResetOrientation()
{
	// Set or Reset the direction to 0.
	zDir_=NAN;
	
	zUp_=NAN;
	
	zRight_=NAN;
	
	// set this as "calculated", so that all queries will return NAN
	// until a correct value is set
	isDirCalculated_=false;
	isUpCalculated_=false;
	isRightCalculated_=false;
}

//-----------------------------------------------------------
namespace {
	inline void CalcSphFromCar(double xDir, double yDir, double zDir,
							   double &zenith, double &azimuth)
	{
		if (std::isnan(zDir)) {zenith=NAN; azimuth=NAN; return;}
		
		// Stolen from I3Direction:
		// Calculate Spherical coordinates from Cartesian
		// Direction is stored on disk in Spherical coordinates only
		// zenith=pi-theta and azimuth=phi+pi in these IceCube coordinates.
		// assume r==1!
		double theta = 0;
		if (std::abs(zDir)<=1.) {
			theta=std::acos(zDir);
		} else {
			if (zDir<0.) theta=I3Constants::pi;
		}
		if (theta<0.) theta+=2.*I3Constants::pi;
		double phi=0;
		if (xDir || yDir) phi=std::atan2(yDir,xDir);
		if (phi<0.) phi+=2.*I3Constants::pi;
		zenith = I3Constants::pi-theta;
		azimuth = phi+I3Constants::pi;
		if (zenith>I3Constants::pi) zenith -= I3Constants::pi-(zenith-I3Constants::pi);
		azimuth -= (int)(azimuth/(2.*I3Constants::pi))*(2.*I3Constants::pi);
	}
}

//-----------------------------------------------------------
void I3Orientation::DoCalcDir() const
{
	// rotated z-axis
	rot_.rotatedZAxis(xDir_, yDir_, zDir_);
	
	CalcSphFromCar(xDir_, yDir_, zDir_,
				   zenithDir_, azimuthDir_);
	isDirCalculated_ = true;
}

//-----------------------------------------------------------
void I3Orientation::DoCalcUp() const
{
	// rotated x-axis
	rot_.rotatedXAxis(xUp_, yUp_, zUp_);
	CalcSphFromCar(xUp_, yUp_, zUp_,
				   zenithUp_, azimuthUp_);
	isUpCalculated_ = true;
}

//-----------------------------------------------------------
void I3Orientation::DoCalcRight() const
{
	// rotated y-axis
	rot_.rotatedYAxis(xRight_, yRight_, zRight_);
	CalcSphFromCar(xRight_, yRight_, zRight_,
				   zenithRight_, azimuthRight_);
	isRightCalculated_ = true;
}

//-----------------------------------------------------------
std::ostream& I3Orientation::Print(std::ostream& os) const{
	if(!isDirCalculated_)
		DoCalcDir();
	if(!isUpCalculated_)
		DoCalcUp();
	if(!isRightCalculated_)
		DoCalcRight();
	os << "I3Orientation: \n"
	<< "    Dir: (" << xDir_ << ',' << yDir_ << ',' << zDir_ << ")\n"
	<< "     Up: (" << xUp_ << ',' << yUp_ << ',' << zUp_ << ")\n"
	<< "  Right: (" << xRight_ << ',' << yRight_ << ',' << zRight_ << ")";
	return(os);
}
