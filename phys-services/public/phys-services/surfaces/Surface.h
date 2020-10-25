/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#ifndef I3SURFACES_SURFACE_H_INCLUDED
#define I3SURFACES_SURFACE_H_INCLUDED

#include <icetray/I3PointerTypedefs.h>
#include <icetray/I3FrameObject.h>
#include <icetray/serialization.h>

class I3Direction;
class I3Position;

namespace I3Surfaces {

/**
 * @brief A closed surface
 *
 * Surface knows how to find the intersections of a ray with itself.
 */
  class Surface: public I3FrameObject {
public:
	virtual ~Surface();
	/**
	 * Find the points where a ray intersects the surface
	 *
	 * @param[in] p   The origin of the ray
	 * @param[in] dir The direction of the ray
	 * @returns a pair of distances from the origin of the ray to the
	 *          intersections with the surface. A distance of NAN means
	 *          that the ray never intersects the surface, and negative
	 *          distances mean that the intersection in question is
	 *          "behind" the origin.
	 */
	virtual std::pair<double, double> GetIntersection(const I3Position &p, const I3Direction &dir) const = 0;
	std::pair<double, double> no_intersection() const
	{
		return std::make_pair(NAN, NAN);
	}

private:
	friend class icecube::serialization::access;
	template <typename Archive>
	void serialize(Archive &, unsigned);
};

I3_POINTER_TYPEDEFS(Surface);

}

I3_CLASS_VERSION(I3Surfaces::Surface, 1);

#endif
