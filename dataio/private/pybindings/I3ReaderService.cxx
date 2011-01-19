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

#include "dataio/I3ReaderService.h"

void
register_I3ReaderService()
{
	bp::class_<I3ReaderService, boost::shared_ptr<I3ReaderService>,
	    bp::bases<I3EventService,
	              I3GeometryService,
	              I3CalibrationService,
	              I3DetectorStatusService,
	              I3MetaService >,
	    boost::noncopyable>("I3ReaderService",
	    bp::init<const std::vector<std::string>&,
	             const std::vector<std::string>&,
	             bool>())
	;
}