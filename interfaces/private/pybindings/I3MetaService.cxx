/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy$
 */

#include "interfaces/I3MetaService.h"
#include "icetray/I3Frame.h"

#include "install.h"

namespace bp = boost::python;

struct I3MetaServiceWrapper : I3MetaService, bp::wrapper<I3MetaService>
{
  I3FramePtr PopMeta() {  return this->get_override("PopMeta")(); }
};

void
register_I3MetaService()
{	
	bp::class_<I3MetaServiceWrapper, boost::shared_ptr<I3MetaServiceWrapper>,
	    boost::noncopyable>("I3MetaService", bp::init<>())
		.def("pop_meta", &I3MetaServiceWrapper::PopMeta)
		.def("install", &I3InstallService<I3MetaService>().func)
	;
}
