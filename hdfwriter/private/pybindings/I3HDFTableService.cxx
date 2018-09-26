/**
 * I3TableService.cxx (pybindings)
 *
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy$
 */

#include <hdfwriter/I3HDFTableService.h>

namespace bp = boost::python;


void register_I3HDFTableService() {
	
   typedef bp::init<const std::string&, int, char> ctor;
   typedef bp::init<I3::dataio::shared_filehandle, int, char> fh_ctor;
   bp::class_<I3HDFTableService, 
      boost::shared_ptr<I3HDFTableService>, bp::bases<I3TableService> >
      ("I3HDFTableService", ctor((bp::args("filename"),
          bp::arg("compression_level")=1, bp::arg("mode")='w')))
      .def(fh_ctor((bp::args("filehandle"),
          bp::arg("compression_level")=1, bp::arg("mode")='w')))
      ;
}
