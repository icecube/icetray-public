/** $Id: Cylinder.cxx 137064 2015-08-31 18:24:47Z jvansanten $
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision: 137064 $
 * $Date: 2015-08-31 13:24:47 -0500 (Mon, 31 Aug 2015) $
 */

#include <phys-services/surfaces/Cup.h>

namespace I3Surfaces {

Cup::~Cup() {}

template <typename Archive>
void
Cup::serialize(Archive &ar, unsigned version)
{
	if (version > 0)
		log_fatal_stream("Version "<<version<<" is from the future");

	ar & make_nvp("Base", base_object<Base>(*this));
}

}

// explicitly instantiate the base classes used
template class I3Surfaces::detail::CupBase<I3Surfaces::SamplingSurface>;

I3_SERIALIZABLE(I3Surfaces::Cup);
