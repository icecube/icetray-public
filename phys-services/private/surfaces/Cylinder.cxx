/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#include <phys-services/surfaces/Cylinder.h>

namespace I3Surfaces {

Cylinder::~Cylinder() {}

template <typename Archive>
void
Cylinder::serialize(Archive &ar, unsigned version)
{
	if (version > 0)
		log_fatal_stream("Version "<<version<<" is from the future");

	ar & make_nvp("Base", base_object<Base>(*this));
}

}

// explicitly instantiate the base classes used
template class I3Surfaces::detail::CylinderBase<I3Surfaces::SamplingSurface>;

I3_SERIALIZABLE(I3Surfaces::Cylinder);
