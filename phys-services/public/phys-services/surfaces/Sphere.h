/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#ifndef I3SURFACES_SPHERE_H_INCLUDED
#define I3SURFACES_SPHERE_H_INCLUDED

#include <phys-services/surfaces/Surface.h>

namespace I3Surfaces {

/**
 * @brief A sphere with its origin at a given vertical depth
 */
class Sphere : public Surface {
public:
	virtual ~Sphere();
	Sphere(double originDepth, double radius) : originDepth_(originDepth), radius_(radius) {};
	virtual std::pair<double, double> GetIntersection(const I3Position &p, const I3Direction &dir) const;
	
private:
	Sphere() {}
	
	friend class icecube::serialization::access;
	template <typename Archive>
	void serialize(Archive &, unsigned);
	
	double originDepth_, radius_;
};

I3_POINTER_TYPEDEFS(Sphere);

}

I3_CLASS_VERSION(I3Surfaces::Sphere, 0);

#endif
