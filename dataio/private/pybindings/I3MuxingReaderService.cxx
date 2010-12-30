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

namespace bp = boost::python;

#include "dataio/I3MuxingReaderService.h"

void
register_I3MuxingReaderService()
{
	bp::class_<I3MuxingReaderService, boost::shared_ptr<I3MuxingReaderService>,
	    bp::bases<I3EventService,
	              I3GeometryService,
	              I3CalibrationService,
	              I3DetectorStatusService,
	              I3MetaService >,
	    boost::noncopyable>("I3MuxingReaderService",
	    bp::init<const std::vector<std::vector<std::string> >&,
	             const std::vector<std::string>&,
	             bool>())
	;
}