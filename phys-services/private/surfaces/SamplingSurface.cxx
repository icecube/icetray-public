/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#include <phys-services/surfaces/SamplingSurface.h>
#include <dataclasses/I3Position.h>
#include <dataclasses/I3Direction.h>
#include <phys-services/I3RandomService.h>

namespace I3Surfaces {

SamplingSurface::~SamplingSurface() {}

double
SamplingSurface::SampleImpactRay(I3Position &impact, I3Direction &dir, I3RandomService &rng,
    double cosMin, double cosMax) const
{
	dir = SampleDirection(rng, cosMin, cosMax);
	impact = SampleImpactPosition(dir, rng);

	// Calculate projected area
	return GetArea(dir);
}

I3Direction
SamplingSurface::SampleDirection(I3RandomService &rng,
    double cosMin, double cosMax) const
{
	// Sample a direction proportional to the projected area 
	// of the surface.
	double maxarea = GetMaximumArea();
	I3Direction sampled_dir;
	do {
		sampled_dir = I3Direction(acos(rng.Uniform(cosMin, cosMax)),
		    rng.Uniform(0, 2*M_PI));
	} while (rng.Uniform(0, maxarea) > GetArea(sampled_dir));
	
	return sampled_dir;
}

template <typename Archive>
void
SamplingSurface::serialize(Archive &ar, unsigned version)
{
	if (version > 0)
		log_fatal_stream("Version "<<version<<" is from the future");
	
	ar & make_nvp("Surface", base_object<Surface>(*this));
}

}

I3_SERIALIZABLE(I3Surfaces::SamplingSurface);
