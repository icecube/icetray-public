/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#include <phys-services/surfaces/AxialCylinder.h>
#include <phys-services/I3RandomService.h>
#include <dataclasses/I3Direction.h>

namespace I3Surfaces {

AxialCylinder::AxialCylinder(double length, double radius, I3Position center)
    : length_(length/2.,length/2.), radius_(radius), center_(center)
{}

AxialCylinder::AxialCylinder(double lengthBefore, double lengthAfter, double radius, I3Position center)
    : length_(lengthBefore,lengthAfter), radius_(radius), center_(center)
{}

std::pair<double, double>
AxialCylinder::GetIntersection(const I3Position &p, const I3Direction &dir) const
{
	// Distance to point of closest approach to the center
	double to_center = (center_ - p)*dir;
	// Check distance of closest approach against cylinder radius
	if ((p + to_center*dir - center_).Magnitude() > radius_)
		return no_intersection();
	else
		return std::make_pair(to_center-length_.first, to_center+length_.second);
}

double
AxialCylinder::GetArea(const I3Direction &dir __attribute__((unused))) const
{
	return M_PI*radius_*radius_;
}

double
AxialCylinder::GetAcceptance(double cosMin, double cosMax) const
{
	return M_PI*radius_*radius_*(cosMax-cosMin);
}

double
AxialCylinder::GetMaximumArea() const
{
	return M_PI*radius_*radius_;
}

I3Direction
AxialCylinder::SampleDirection(I3RandomService &rng, double cosMin, double cosMax) const
{
	return I3Direction(std::acos(rng.Uniform(cosMin, cosMax)), rng.Uniform(0, 2*M_PI));
}

I3Position
AxialCylinder::SampleImpactPosition(const I3Direction &dir, I3RandomService &rng) const
{
	// Choose a position in a circle in axis-centered coordinates
	I3Position impact(std::sqrt(rng.Uniform(0, radius_*radius_)), 0, 0);
	impact.RotateZ(rng.Uniform(0, 2*M_PI));
	
	// Rotate into the transverse plane
	impact.RotateY(dir.GetZenith());
	impact.RotateZ(dir.GetAzimuth());
	// Shift from cylinder-centered to real coordinates
	impact += center_;
	// Shift back to the entry point
	impact -= length_.first*dir;
	
	return impact;
}


  std::ostream& AxialCylinder::Print(std::ostream& os) const
  {
    os << "AxialCylinder("<<length_.first <<", "<< length_.second <<", " << radius_ << ", " << center_ << ")";
    return os;
  }
  
template <typename Archive>
void
AxialCylinder::serialize(Archive &ar, unsigned version)
{
	if (version > 0)
		log_fatal_stream("Version "<<version<<" is from the future");
	
	ar & make_nvp("SamplingSurface", base_object<SamplingSurface>(*this));
	ar & make_nvp("Length", length_);
	ar & make_nvp("Radius", radius_);
	ar & make_nvp("Center", center_);
}

AxialCylinder::~AxialCylinder() {}

}

std::ostream& operator<<(std::ostream& oss, const I3Surfaces::AxialCylinder& p)
{
  return(p.Print(oss));
}

I3_SERIALIZABLE(I3Surfaces::AxialCylinder);
