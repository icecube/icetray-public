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

#include <tableio/broadcaster/I3BroadcastTableService.h>

namespace bp = boost::python;

boost::shared_ptr<I3BroadcastTableService> tuple_init(bp::tuple clients) {
    std::vector<I3TableServicePtr> services;
    size_t n = bp::len(clients);
    for (size_t i = 0; i < n; ++i) {
        services.push_back(bp::extract<I3TableServicePtr>(clients.attr("__getitem__")(i)));
    }
    return boost::shared_ptr<I3BroadcastTableService>(new I3BroadcastTableService(services));
}

void register_I3BroadcastTableService() {
   bp::class_<I3BroadcastTableService, 
      boost::shared_ptr<I3BroadcastTableService>, bp::bases<I3TableService> >
      ("I3BroadcastTableService", bp::no_init)
      .def("__init__",bp::make_constructor(&tuple_init))
      ;
}

