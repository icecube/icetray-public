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
Surface::serialize(Archive &ar __attribute__ ((unused)), unsigned version __attribute__ ((unused)))
{}

}

I3_SERIALIZABLE(I3Surfaces::Surface);
