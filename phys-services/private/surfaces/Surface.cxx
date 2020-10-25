/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#include <phys-services/surfaces/Surface.h>

namespace I3Surfaces {

Surface::~Surface() {}

template <typename Archive>
void
Surface::serialize(Archive &ar, unsigned version)
{
	if(version > 0)
		ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
}

}

I3_SERIALIZABLE(I3Surfaces::Surface);
