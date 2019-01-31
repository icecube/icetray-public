/** $Id$
 * @file
 * @author Jakob van Santen <jakob.van.santen@desy.de>
 *
 * $Revision$
 * $Date$
 */

#ifndef I3SURFACES_CYLINDER_H_INCLUDED
#define I3SURFACES_CYLINDER_H_INCLUDED

#include <phys-services/surfaces/SamplingSurface.h>
#include <phys-services/surfaces/detail/CylinderBase.h>

namespace I3Surfaces {

/**
 * @brief A cylinder aligned with the z axis
 */
class Cylinder : public detail::CylinderBase<SamplingSurface> {
private:
	typedef detail::CylinderBase<SamplingSurface> Base;
public:
	virtual ~Cylinder();
	Cylinder(double length, double radius, I3Position center=I3Position(0,0,0)) : Base(length, radius, center) {};

private:
	Cylinder() {}
	
	friend class icecube::serialization::access;
	template <typename Archive>
	void serialize(Archive &, unsigned);

public:
  std::ostream& Print(std::ostream& os) const;
};

I3_POINTER_TYPEDEFS(Cylinder);

}

std::ostream& operator<<(std::ostream& oss, const I3Surfaces::Cylinder& p);

I3_CLASS_VERSION(I3Surfaces::Cylinder, 0);

#endif // I3SURFACES_CYLINDER_H_INCLUDED
