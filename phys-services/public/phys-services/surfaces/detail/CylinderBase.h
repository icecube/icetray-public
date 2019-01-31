/** $Id$
 * @file
 * @author Jakob van Santen <jakob.van.santen@desy.de>
 *
 * $Revision$
 * $Date$
 */

#ifndef I3SURFACES_CYLINDERBASE_H_INCLUDED
#define I3SURFACES_CYLINDERBASE_H_INCLUDED

#include <dataclasses/I3Position.h>
#include <dataclasses/I3Direction.h>
#include <phys-services/I3RandomService.h>

namespace {

inline void
sort(std::pair<double, double> &pair)
{
	if (pair.first > pair.second) {
		double aux = pair.first;
		pair.first = pair.second;
		pair.second = aux;
	}
}
}

namespace I3Surfaces { namespace detail {

template <typename Base>
class CylinderBase : public Base {
public:
	CylinderBase(double length, double radius, I3Position center=I3Position(0,0,0))
	    : length_(length), radius_(radius), center_(center)
	{}

	// I3Surfaces::Surface interface
	// Find the distances to the points of intersection with a cylinder 
	// aligned along the z axis. Adapted from:
	// http://code.icecube.wisc.edu/svn/projects/mmc/trunk/src/tfa/Amanda.java
	// (D. Chirkin)
	std::pair<double, double> GetIntersection(const I3Position &p, const I3Direction &dir) const
	{
		std::pair<double, double> h(Surface::no_intersection()), r(Surface::no_intersection());
	
		double x = p.GetX()-center_.GetX();
		double y = p.GetY()-center_.GetY();
		double z = p.GetZ()-center_.GetZ();
	
		double sinph = sin(dir.GetAzimuth());
		double cosph = cos(dir.GetAzimuth());
		double sinth = sin(dir.GetZenith());
		double costh = cos(dir.GetZenith());
	
		double b = x*cosph + y*sinph;
		double d = b*b + radius_*radius_ - x*x - y*y;
	
		if (d > 0) {
			d = sqrt(d);
			// down-track distance to the endcaps
			if (costh != 0) {
				h.first  = (z - length_/2)/costh;
				h.second = (z + length_/2)/costh;
				sort(h);
			}
			// down-track distance to the side surfaces
			if (sinth != 0) {
				r.first  = (b - d)/sinth;
				r.second = (b + d)/sinth;
				sort(r);
			}
			// Perfectly horizontal tracks never intersect the endcaps
			if (costh == 0) {
				if ((z > -length_/2) && (z < length_/2))
					h = r;
				else
					h = Surface::no_intersection();
			// Perfectly vertical tracks never intersect the sides
			} else if (sinth == 0) {
				if (hypot(x, y) >= radius_)
					h = Surface::no_intersection();
			// For general tracks, take the last entrace and first exit
			} else {
				if (h.first >= r.second || h.second <= r.first)
					h = Surface::no_intersection();
				else {
					h.first = std::max(r.first, h.first);
					h.second = std::min(r.second, h.second);
				}
			}
		}
	
		return h;
	}

	// I3Surfaces::SamplingSurface interfact
	double GetArea(const I3Direction &dir) const
	{
		return GetAreaForZenith(-dir.GetZ());
	}
	double GetMaximumArea() const
	{
		double thetaMax = atan(2*length_/(M_PI*radius_));
		return GetAreaForZenith(cos(thetaMax));
	}
	virtual double GetAcceptance(double cosMin=0, double cosMax=1) const
	{
          double a=cosMin;
          double b=cosMax;                
          double cap = M_PI*radius_*radius_;
          double sides = 2*radius_*length_;
          return M_PI*(cap*(b*fabs(b)-a*fabs(a)) +
                       sides*(acos(a) - acos(b) - sqrt(1-a*a)*a + sqrt(1-b*b)*b));                
	}
	
	// SamplingSurface interface
	I3Direction SampleDirection(I3RandomService &rng, double cosMin=0, double cosMax=1) const
	{
		// Sample a direction proportional to the projected area 
		// of the surface.
		double coszen;
		double maxarea = GetMaximumArea();
		do {
			coszen = rng.Uniform(cosMin, cosMax);
		} while (rng.Uniform(0, maxarea) > GetAreaForZenith(coszen));
	
		return I3Direction(acos(coszen), rng.Uniform(0, 2*M_PI));
	}
	I3Position SampleImpactPosition(const I3Direction &dir, I3RandomService &rng) const
	{
		// The projection of a cylinder onto a plane whose
		// normal is inclined by `zenith` w.r.t to the cylinder
		// axis is a rectangle of width 2*r and height h*sin(theta)
		// capped with two half-ellipses of major axis r and
		// minor axis r*cos(theta). Pick a point from a uniform
		// distribution over this area.
		double a = sin(dir.GetZenith())*length_/2.;
		double b = fabs(cos(dir.GetZenith()))*radius_;
		double x, y;
		do {
			x = radius_*rng.Uniform(-1, 1);
			y = (a + b)*rng.Uniform(-1, 1);
		} while (fabs(y) > a + b*sqrt(1 - (x*x)/(radius_*radius_)));
		// Rotate into the transverse plane
		I3Position impact(y, x, 0);
		impact.RotateY(dir.GetZenith());
		impact.RotateZ(dir.GetAzimuth());
		// Shift from cylinder-centered to real coordinates
		impact.SetX(impact.GetX() + center_.GetX());
		impact.SetY(impact.GetY() + center_.GetY());
		impact.SetZ(impact.GetZ() + center_.GetZ());
		// Project back to the entry point
		double l = GetIntersection(impact, dir).first;
		impact.SetX(impact.GetX() + l*dir.GetX());
		impact.SetY(impact.GetY() + l*dir.GetY());
		impact.SetZ(impact.GetZ() + l*dir.GetZ());
	
		return impact;
	}
	void SetLength(double v) { length_ = v; }
	double GetLength() const { return length_; }
	
	void SetRadius(double v) { radius_ = v; }
	double GetRadius() const { return radius_; }
	
	void SetCenter(const I3Position &v) { center_ = v; }
	I3Position GetCenter() const { return center_; }

protected:
	CylinderBase() {}

private:
	double GetAreaForZenith(double coszen) const
	{
		double cap = M_PI*radius_*radius_;
		double sides = 2*radius_*length_;
		return cap*fabs(coszen) + sides*sqrt(1.-coszen*coszen);
	}

	friend class icecube::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, unsigned version)
	{
		if (version > 0)
			log_fatal_stream("Version "<<version<<" is from the future");
	
		ar & make_nvp("Base", base_object<Base>(*this));
		ar & make_nvp("Length", length_);
		ar & make_nvp("Radius", radius_);
		ar & make_nvp("Center", center_);
	}

	double length_, radius_;
	I3Position center_;
};

}}

#endif // I3SURFACES_CYLINDERBASE_H_INCLUDED
