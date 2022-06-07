/**
 *  $Id$
 *  
 *  Copyright  (C) 2008, 2009
 *  Claudio Kopper <claudio.kopper@physik.uni-erlangen.de>
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *  
 *  SPDX-License-Identifier: BSD-2-Clause
 *  
 */
#ifndef I3QUATERNION_H_INCLUDED
#define I3QUATERNION_H_INCLUDED

#include <iostream>
#include <cmath>

#include <dataclasses/I3Direction.h>
#include <dataclasses/I3Position.h>
#include "icetray/I3FrameObject.h"

/**
 * @brief This class implements quaternions which can be used
 * for vector rotations.
 *
 * @version $Id$
 */

static const unsigned i3quaternion_version_ = 0;

class I3Quaternion : public I3FrameObject
	{
	public:
		
		/**
		 * Default constructor. Sets all elements to zero.
		 */
		I3Quaternion() : x_(0.), y_(0.), z_(0.), w_(0.) {}
		
		/**
		 * Element-wise constructor. Creates a new quaternion
		 * based on the elements given as arguments.
		 */
		I3Quaternion(double x, double y, double z, double w) : x_(x), y_(y), z_(z), w_(w) {}
		
		/**
		 * Copy constructor.
		 */
		I3Quaternion(const I3Quaternion &q) : x_(q.x_), y_(q.y_), z_(q.z_), w_(q.w_) {}
		
		~I3Quaternion() {}
		
		std::ostream& Print(std::ostream&) const override;
		
		/**
		 * Element-wise setter.
		 */
		inline void set(double x, double y, double z, double w) 
		{
			x_ = x; y_ = y; z_ = z; w_ = w;
		}

		inline double getX() const {return x_;}
		inline double getY() const {return y_;}
		inline double getZ() const {return z_;}
		inline double getW() const {return w_;}
		
		I3Quaternion operator=(const I3Quaternion& q)
		{
			x_ = q.x_; y_ = q.y_; z_ = q.z_; w_ = q.w_;
			return (*this);
		}
		
		I3Quaternion operator+(const I3Quaternion& q) const
		{
			return I3Quaternion(x_+q.x_, y_+q.y_, z_+q.z_, w_+q.w_);
		}
		
		I3Quaternion operator-(const I3Quaternion& q) const
		{
			return I3Quaternion(x_-q.x_, y_-q.y_, z_-q.z_, w_-q.w_);
		}
		
		I3Quaternion operator*(const I3Quaternion& q) const
		{
			return I3Quaternion(  x_*q.w_ + y_*q.z_ - z_*q.y_ + w_*q.x_,
								y_*q.w_ + z_*q.x_ - x_*q.z_ + w_*q.y_,
								z_*q.w_ + x_*q.y_ - y_*q.x_ + w_*q.z_,
								- x_*q.x_ - y_*q.y_ - z_*q.z_ + w_*q.w_);
		}
		
		I3Quaternion operator/(const I3Quaternion& q) const
		{
			return ((*this) * (q.inverse()));
		}
		
		I3Quaternion operator*(double s) const
		{
			return I3Quaternion(x_*s, y_*s, z_*s, w_*s);
		}
		
		I3Quaternion operator/(double s) const
		{
			return ((*this) * (1./s));
		}
		
		I3Quaternion operator+=(const I3Quaternion& q)
		{
			x_ += q.x_; y_ += q.y_; z_ += q.z_; w_ += q.w_;
			return (*this);
		}
		
		I3Quaternion operator-=(const I3Quaternion& q)
		{
			x_ -= q.x_; y_ -= q.y_; z_ -= q.z_; w_ -= q.w_;
			return (*this);
		}
		
                /**
                 * @note This infix operator implements a *right* multiplication.
                 * There is NO infix operator for *left* multiplication.
                 */
		I3Quaternion operator*=(const I3Quaternion& q)
		{
			double newX =   x_*q.w_ + y_*q.z_ - z_*q.y_ + w_*q.x_;
			double newY =   y_*q.w_ + z_*q.x_ - x_*q.z_ + w_*q.y_;
			double newZ =   z_*q.w_ + x_*q.y_ - y_*q.x_ + w_*q.z_;
			double newW = - x_*q.x_ - y_*q.y_ - z_*q.z_ + w_*q.w_;
			x_ = newX; y_ = newY; z_ = newZ; w_ = newW;
			return (*this);
		}
		
                /**
                 * @note This infix operator implements a *right* division.
                 * There is NO infix operator for *left* division (q.inverse()*(*this)).
                 */
		I3Quaternion operator/=(const I3Quaternion& q)
		{
			(*this) = (*this) * q.inverse();
			return (*this);
		}
		
		I3Quaternion operator*=(double s)
		{
			x_*=s; y_*=s; z_*=s; w_*=s;
			return (*this);
		}
		
		I3Quaternion operator/=(double s)
		{
			(*this) = (*this) * (1./s);
			return (*this);
		}
		
		bool operator!=(const I3Quaternion& q) const
		{
			return ((x_!=q.x_) || (y_!=q.y_) || (z_!=q.z_) || (w_!=q.w_));
		}
		
		bool operator==(const I3Quaternion& q) const
		{
			return ((x_==q.x_) && (y_==q.y_) && (z_==q.z_) && (w_==q.w_));
		}
		
		/**
		 * returns the norm of this quaternion
		 */
		inline double norm() const
		{
			return (x_*x_ + y_*y_ + z_*z_ + w_*w_);
		}
		
		/**
		 * returns the magnitude of this quaternion
		 */
		inline double magnitude() const
		{
			return std::sqrt(norm());
		}
		
		/**
		 * returns a new quaternion where each entry is multiplied
		 * by a scalar s
		 */
		inline I3Quaternion scale(double s) const
		{
			return I3Quaternion(x_*s, y_*s, z_*s, w_*s);
		}
		
		/**
		 * returns the conjugate of this quaternion
		 */
		inline I3Quaternion conjugate() const
		{
			return I3Quaternion(-x_, -y_, -z_, w_);
		}
		
		/**
		 * returns the inverse of this quaternion
		 */
		inline I3Quaternion inverse() const
		{
			return conjugate().scale(1./norm());
		}
		
		/**
		 * returns the unit quaternion of this quaternion
		 */
		inline I3Quaternion unit() const
		{
			return (*this).scale(1./(*this).magnitude());
		}
		
		/**
		 * rotates a 3-vector v by this quaternion.
		 * The rotation is performed in-place, the
		 * old vector is destroyed.
		 */
		inline void rot3VectorInPlace(double v[3]) const
		{
			I3Quaternion qv(v[0], v[1], v[2], 0.);
			I3Quaternion qm = (*this) * qv * (*this).inverse();
			v[0] = qm.x_; v[1] = qm.y_; v[2] = qm.z_;  
		}

		/**
		 * rotates a 3-vector v by this quaternion.
		 * The rotation is performed in-place, the
		 * old vector is destroyed.
		 */
		inline void rot3VectorInPlace(double &v0, double &v1, double &v2) const
		{
			I3Quaternion qv(v0, v1, v2, 0.);
			I3Quaternion qm = (*this) * qv * (*this).inverse();
			v0 = qm.x_; v1 = qm.y_; v2 = qm.z_;  
		}
		
		/**
		 * Rotates the vector (1,0,0) using this quaternion.
		 * Will return invalid data if the quaternion is not normalized.
		 */
		inline void rotatedXAxis(double &xRes, double &yRes, double &zRes) const
		{
			xRes = 1. - 2.*(y_*y_ + z_*z_);
			yRes = 2.*( w_*z_ + x_*y_);
			zRes = 2.*(-w_*y_ + x_*z_);
		}

		/**
		 * Rotates the vector (0,1,0) using this quaternion.
		 * Will return invalid data if the quaternion is not normalized.
		 */
		inline void rotatedYAxis(double &xRes, double &yRes, double &zRes) const
		{
			xRes = 2.*(-w_*z_ + x_*y_);
			yRes = 1. - 2.*(x_*x_ + z_*z_);
			zRes = 2.*(w_*x_ + y_*z_);
		}

		/**
		 * Rotates the vector (0,0,1) using this quaternion.
		 * Will return invalid data if the quaternion is not normalized.
		 */
		inline void rotatedZAxis(double &xRes, double &yRes, double &zRes) const
		{
			xRes = 2.*(w_*y_ + x_*z_);
			yRes = 2.*(-w_*x_ + y_*z_);
			zRes = 1. - 2.*(x_*x_ + y_*y_);
		}
		
		/**
		 * Rotates a given I3Direction by this quaternion
		 * and returns the result as a new object.
		 */
		inline I3Direction rotate(const I3Direction &dir) const
		{
			I3Quaternion qv(dir.GetX(), dir.GetY(), dir.GetZ(), 0.);
			I3Quaternion qm = (*this) * qv * (*this).inverse();
			return I3Direction(qm.x_, qm.y_, qm.z_);
		}
		
		/**
		 * Rotates a given I3Position by this quaternion
		 * and returns the result as a new object.
		 */
		inline I3Position rotate(const I3Position &pos) const
		{
			I3Quaternion qv(pos.GetX(), pos.GetY(), pos.GetZ(), 0.);
			I3Quaternion qm = (*this) * qv * (*this).inverse();
			return I3Position(qm.x_, qm.y_, qm.z_);
		}
		
	private:
		double x_,y_,z_,w_;
		
		friend class icecube::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, unsigned version);
		
	};

I3Quaternion operator*(double, const I3Quaternion&);
std::ostream& operator<<(std::ostream&, const I3Quaternion&);

I3_POINTER_TYPEDEFS(I3Quaternion);
I3_CLASS_VERSION(I3Quaternion, i3quaternion_version_);

#endif
