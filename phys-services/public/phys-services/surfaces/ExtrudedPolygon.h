/** $Id$
 * @file
 * @author Jakob van Santen <jakob.van.santen@desy.de>
 *
 * $Revision$
 * $Date$
 */

#ifndef I3SURFACES_EXTRUDEDPOLYGON_H_INCLUDED
#define I3SURFACES_EXTRUDEDPOLYGON_H_INCLUDED

#include <phys-services/surfaces/SamplingSurface.h>
#include <phys-services/surfaces/detail/ExtrudedPolygonBase.h>

namespace I3Surfaces {

class ExtrudedPolygon : public ExtrudedPolygonBase<SamplingSurface> {
private:
	typedef ExtrudedPolygonBase<SamplingSurface> Base;
public:
	ExtrudedPolygon(const std::vector<I3Position> &points, double padding=0.) : Base(points, padding) {};
	~ExtrudedPolygon();
private:
	ExtrudedPolygon() {}
	friend class icecube::serialization::access;
	template <typename Archive>
	void serialize(Archive &, unsigned);
};

I3_POINTER_TYPEDEFS(ExtrudedPolygon);

}

I3_CLASS_VERSION(I3Surfaces::ExtrudedPolygon, 0);

#endif // I3SURFACES_EXTRUDEDPOLYGON_H_INCLUDED
