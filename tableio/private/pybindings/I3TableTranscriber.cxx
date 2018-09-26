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

#include <tableio/utils/I3TableTranscriber.h>

namespace bp = boost::python;

void execute(I3TableTranscriber& self, int num = -1) {
    if (num < 0) {
        self.Execute();
    } else {
        self.Execute(num);
    }
} 

BOOST_PYTHON_FUNCTION_OVERLOADS(execute_overloads,execute,1,2)

void register_I3TableTranscriber() {
    bp::class_<I3TableTranscriber>("I3TableTranscriber",
        bp::init<I3TableServicePtr,I3TableServicePtr>(bp::args("input_service,output_service")))
    .def("Execute",execute,execute_overloads(bp::args("num")))
    ;
}
