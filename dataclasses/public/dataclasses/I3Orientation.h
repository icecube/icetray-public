/**
 * @version $Id$
 * @file I3Orientation.h
 * @date $Date$
 */

#ifndef I3ORIENTATION_H_INCLUDED
#define I3ORIENTATION_H_INCLUDED

#include <math.h>
#include <sstream>
#include "icetray/I3FrameObject.h"
#include "Utility.h"
#include "dataclasses/I3Constants.h"
#include "dataclasses/I3Quaternion.h"
#include "dataclasses/I3Direction.h"

static const unsigned i3orientation_version_ = 0;

/**
 * Stores an orientation. This is more than a direction
 * in that you also include the rotation around the direction
 * axis. Picture the three-finger right hand rule.
 * Your thumb points to "front" (the direction), your index finger points
 * to "up" and your middle finger to "right".
 */
class I3Orientation : public I3FrameObject
	{
	public:
		
		I3Orientation();
		
		/**
		 * Additional constructor: 6 arguments mean construct dir. with dir(x,y,z) and up(x,y,z)
		 */
		I3Orientation(double x_dir, double y_dir, double z_dir, double x_up, double y_up, double z_up);
		
		/**
		 * Additional constructor: 6 arguments mean construct dir. with dir(x,y,z) and up(x,y,z)
		 */
		I3Orientation(const I3Direction &dir, const I3Direction &up);
		
		/**
		 * Copy constructor
		 */
		I3Orientation(const I3Orientation& d);

		/**
		 * Assignment constructor from direction (will generate an arbitrary rotation angle)
		 */
		I3Orientation(const I3Direction& d)
		{
			SetOrientation(d);
		}
		
		~I3Orientation();
		
		std::ostream& Print(std::ostream&) const override;
		
		//--------------

		/**
		 * Assignment operator from direction (will generate an arbitrary rotation angle)
		 */
		inline I3Orientation operator=(const I3Orientation& d)
		{
			SetOrientation(d);
			return (*this);
		}
		
		/**
		 * Assignment operator from orientation (will generate an arbitrary rotation angle)
		 */
		inline I3Orientation operator=(const I3Direction& d)
		{
			SetOrientation(d);
			return (*this);
		}

        /**
		 * Test for equality
		 */
		inline bool operator==(const I3Orientation& other) const
		{
            return (rot_ == other.rot_);
		}

		//--------------
		
		/**
		 * Store direction from orientation d
		 */
		void SetOrientation(const I3Orientation& d);

		/**
		 * Store direction from a single direction d
		 * (this will generate an arbitrary "up" direction)
		 */
		void SetOrientation(const I3Direction& d);
		
		/**
		 * Store direction with x, y, z (3 arguments)
		 */
		void SetOrientation(double x_dir, double y_dir, double z_dir, double x_up, double y_up, double z_up);

		/**
		 * Store direction with x, y, z (3 arguments)
		 */
		void SetOrientation(const I3Direction &dir, const I3Direction &up);

		/**
		 * Store direction from a unit quaternion rotation the (0,0,1) vector
		 */
		void SetOrientationFromQuaternion(const I3Quaternion &q);
		
		/**
		 * Reset all elements of I3Orientation to NAN
		 */
		void ResetOrientation();

		//--------------

		/**
		 * Rotate a vector inside a coordinate system where z-axis==(0,0,1), x-axis==(1,0,0), y-axis=(0,1,0)
		 * to the system where z-axis==dir, x-axis==up, y-axis==right. If inverse
		 * This is the in-place version with 3 doubles for x,y,z.
		 */
		inline void RotVectorInPlace(double &x, double &y, double &z, bool inverse=false) const {
			if (!inverse)
				rot_.conjugate().rot3VectorInPlace(x,y,z);
			else
				rot_.rot3VectorInPlace(x,y,z);
		}

		/**
		 * Rotate a vector from a coordinate system where z-axis==(0,0,1), x-axis==(1,0,0), y-axis=(0,1,0)
		 * to the system where z-axis==dir, x-axis==up, y-axis==right.
		 */
		template <typename T>
		T RotateIn(const T& vec) const {
			double x=vec.GetX();
			double y=vec.GetY();
			double z=vec.GetZ();

			RotVectorInPlace(x,y,z,false);

			return T(x,y,z);
		}
		
		/**
		 * Rotate a vector from a coordinate system where z-axis==dir, x-axis==up, y-axis==right
		 * to the system where z-axis==(0,0,1), x-axis==(1,0,0), y-axis=(0,1,0).
		 */
		template <typename T>
		T RotateOut(const T& vec) const {
			double x=vec.GetX();
			double y=vec.GetY();
			double z=vec.GetZ();

			RotVectorInPlace(x,y,z,true);

			return T(x,y,z);
		}
		

		//--------------
		
		/**
		 * Provide Zenith of direction
		 */
		inline double GetZenith() const {
			if (!isDirCalculated_) DoCalcDir();
			return zenithDir_;
		}

		inline double GetDirZenith() const {
			return GetZenith();
		}
		
		/**
		 * Provide Azimuth of direction
		 */
		inline double GetAzimuth() const {
			if (!isDirCalculated_) DoCalcDir();
			return azimuthDir_;
		}

		inline double GetDirAzimuth() const {
			return GetAzimuth();
		}
		
		/**
		 * Provide X of direction in cartesian ref frame
		 */
		inline double GetX() const {
			if (!isDirCalculated_) DoCalcDir();
			return xDir_;
		}

		inline double GetDirX() const {
			return GetX();
		}
		
		/**
		 * Provide Y of direction in cartesian ref frame
		 */
		inline double GetY() const {
			if (!isDirCalculated_) DoCalcDir();
			return yDir_;
		}

		inline double GetDirY() const {
			return GetY();
		}
		
		/**
		 * Provide Z of direction in cartesian ref frame
		 */
		inline double GetZ() const {
			if (!isDirCalculated_) DoCalcDir();
			return zDir_;
		}

		inline double GetDirZ() const {
			return GetZ();
		}

		/**
		 * Calculate "dir" direction
		 */
		inline I3Direction GetDir() const {
			if (!isDirCalculated_) DoCalcDir();
			return I3Direction(zenithDir_, azimuthDir_);
		}
		
		/**
		 * Calculate Theta of "dir" direction
		 */
		inline double CalcDirTheta() const {
			if (!isDirCalculated_) DoCalcDir();
			double theta = I3Constants::pi - zenithDir_;
			return theta;
		}
		
		/**
		 * Calculate Phi of "dir" direction
		 */
		inline double CalcDirPhi() const {
			if (!isDirCalculated_) DoCalcDir();
			double phi = I3Constants::pi + azimuthDir_;
			if (phi >= 2*I3Constants::pi) phi -= 2*I3Constants::pi;
			return phi;
		}
		
		/**
		 * Provide Zenith of "up" direction
		 */
		inline double GetUpZenith() const {
			if (!isUpCalculated_) DoCalcUp();
			return zenithUp_;
		}
		
		/**
		 * Provide Azimuth of "up" direction
		 */
		inline double GetUpAzimuth() const {
			if (!isUpCalculated_) DoCalcUp();
			return azimuthUp_;
		}
		
		/**
		 * Provide X of "up" direction in cartesian ref frame
		 */
		inline double GetUpX() const {
			if (!isUpCalculated_) DoCalcUp();
			return xUp_;
		}
		
		/**
		 * Provide Y of "up" direction in cartesian ref frame
		 */
		inline double GetUpY() const {
			if (!isUpCalculated_) DoCalcUp();
			return yUp_;
		}
		
		/**
		 * Provide Z of "Up" direction in cartesian ref frame
		 */
		inline double GetUpZ() const {
			if (!isUpCalculated_) DoCalcUp();
			return zUp_;
		}
		
		/**
		 * Calculate "up" direction
		 */
		inline I3Direction GetUp() const {
			if (!isUpCalculated_) DoCalcUp();
			return I3Direction(zenithUp_, azimuthUp_);
		}
		
		/**
		 * Calculate Theta of "up" direction
		 */
		inline double CalcUpTheta() const {
			if (!isUpCalculated_) DoCalcUp();
			double theta = I3Constants::pi - zenithUp_;
			return theta;
		}
		
		/**
		 * Calculate Phi of "up" direction
		 */
		inline double CalcUpPhi() const {
			if (!isUpCalculated_) DoCalcUp();
			double phi = I3Constants::pi + azimuthUp_;
			if (phi >= 2*I3Constants::pi) phi -= 2*I3Constants::pi;
			return phi;
		}
		
		/**
		 * Provide Zenith of "right" direction
		 */
		inline double GetRightZenith() const {
			if (!isRightCalculated_) DoCalcRight();
			return zenithRight_;
		}
		
		/**
		 * Provide Azimuth of "right" direction
		 */
		inline double GetRightAzimuth() const {
			if (!isRightCalculated_) DoCalcRight();
			return azimuthRight_;
		}
		
		/**
		 * Provide X of "right" direction in cartesian ref frame
		 */
		inline double GetRightX() const {
			if (!isRightCalculated_) DoCalcRight();
			return xRight_;
		}
		
		/**
		 * Provide Y of "right" direction in cartesian ref frame
		 */
		inline double GetRightY() const {
			if (!isRightCalculated_) DoCalcRight();
			return yRight_;
		}
		
		/**
		 * Provide Z of "right" direction in cartesian ref frame
		 */
		inline double GetRightZ() const {
			if (!isRightCalculated_) DoCalcRight();
			return zRight_;
		}
		
		/**
		 * Calculate "right" direction
		 */
		inline I3Direction GetRight() const {
			if (!isRightCalculated_) DoCalcRight();
			return I3Direction(zenithRight_, azimuthRight_);
		}
		
		/**
		 * Calculate Theta of "right" direction
		 */
		inline double CalcRightTheta() const {
			if (!isRightCalculated_) DoCalcRight();
			double theta = I3Constants::pi - zenithRight_;
			return theta;
		}
		
		/**
		 * Calculate Phi of "right" direction
		 */
		inline double CalcRightPhi() const {
			if (!isRightCalculated_) DoCalcRight();
			double phi = I3Constants::pi + azimuthRight_;
			if (phi >= 2*I3Constants::pi) phi -= 2*I3Constants::pi;
			return phi;
		}
		
		//--------------
		
		/**
		 * Rotate orientation around X axis by angle
		 */
		//void RotateX(double angle);
		
		/**
		 * Rotate orientation around Y axis by angle
		 */
		//void RotateY(double angle);
		
		/**
		 * Rotate orientation around Z axis by angle
		 */
		//void RotateZ(double angle);
		
		
	protected:
		/**
		 * the actual rotation w.r.t (0,0,1)
		 */ 
		I3Quaternion rot_;
		
		/**
		 * direction "dir" coordinates
		 */ 
		mutable double xDir_;	//!
		mutable double yDir_;	//!
		mutable double zDir_;	//!
		mutable double zenithDir_;	//!
		mutable double azimuthDir_;	//!

		/**
		 * direction "up" coordinates
		 */ 
		mutable double xUp_;	//!
		mutable double yUp_;	//!
		mutable double zUp_;	//!
		mutable double zenithUp_;	//!
		mutable double azimuthUp_;	//!

		/**
		 * direction "right" coordinates
		 */ 
		mutable double xRight_;	//!
		mutable double yRight_;	//!
		mutable double zRight_;	//!
		mutable double zenithRight_;	//!
		mutable double azimuthRight_;	//!
		
		/**
		 * Did we calculate the directions before?
		 */
		mutable bool isDirCalculated_;	//!
		mutable bool isUpCalculated_;	//!
		mutable bool isRightCalculated_;	//!
		
	private:
		/**
		 * Calculate all coordinates from the internal rotation
		 */
		void DoCalcDir() const;
		void DoCalcUp() const;
		void DoCalcRight() const;
		
		friend class icecube::serialization::access;
		
		template <class Archive>
		void serialize(Archive& ar, unsigned version);
	};

std::ostream& operator<<(std::ostream&, const I3Orientation&);

I3_POINTER_TYPEDEFS(I3Orientation);
I3_CLASS_VERSION(I3Orientation, i3orientation_version_);

typedef I3Vector<I3Orientation> I3OrientationVect;
I3_POINTER_TYPEDEFS(I3OrientationVect);

#endif //I3ORIENTATION_H_INCLUDED
