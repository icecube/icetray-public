/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#ifndef I3SURFACES_SAMPLINGSURFACE_H_INCLUDED
#define I3SURFACES_SAMPLINGSURFACE_H_INCLUDED

#include <phys-services/surfaces/Surface.h>

class I3RandomService;

namespace I3Surfaces {

/**
 * @brief A surface that can sample from the distribution of impact positions
 *        and directions of rays from an isotropic flux.
 */
class SamplingSurface : public Surface {
public:
	virtual ~SamplingSurface();
	/**
	 * @brief Calculate the projected area in m^2
	 *
	 * @param[in] dir The direction onto which to project the area
	 */
	virtual double GetArea(const I3Direction &dir) const = 0;
	virtual double GetMaximumArea() const = 0;
	virtual double GetAcceptance(double cosMin=0, double cosMax=1) const=0;

	virtual I3Direction SampleDirection(I3RandomService &rng, double cosMin=0, double cosMax=1) const;
	virtual I3Position SampleImpactPosition(const I3Direction &dir, I3RandomService &rng) const = 0;

	 /**
	  * Sample an impact point and direction from an isotropic flux
	  *
	  * @param[out] pos    Impact point
	  * @param[out] dir    direction
	  * @param[in]  rng    Random number generator
	  * @param[in]  cosMin cosine of the maximum zenith angle to consider
	  * @param[in]  cosMax cosine of the minimum zenith angle to consider
	  * @returns the projected area along the chosen zenith angle
	  */
	virtual double SampleImpactRay(I3Position &pos, I3Direction &dir, I3RandomService &rng, double cosMin=0, double cosMax=1) const;
private:
	friend class icecube::serialization::access;
	template <typename Archive>
	void serialize(Archive &, unsigned);
};

I3_POINTER_TYPEDEFS(SamplingSurface);

}

I3_CLASS_VERSION(I3Surfaces::SamplingSurface, 0);

#endif // I3SURFACES_SAMPLINGSURFACE_H_INCLUDED
