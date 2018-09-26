/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#include "tableio/I3Datatype.h"
#include "type_helpers.h"

namespace bp = boost::python;

void register_I3Datatype() {
    
    bp::scope dtype_scope = 
    bp::class_<I3Datatype, boost::shared_ptr<I3Datatype> >("I3Datatype")
        .def("__init__",bp::make_constructor(&I3Datatype_from_PyObject))
        .def_readwrite("kind",&I3Datatype::kind)
        .def_readwrite("size",&I3Datatype::size)
        .def_readwrite("is_signed",&I3Datatype::is_signed)
        .def_readwrite("description",&I3Datatype::description)
        
    ;
    
    bp::enum_<I3Datatype::TypeClass>("TypeClass")
    #define TYPE_CLASS (Float)(Int)(Bool)(Enum)
    BOOST_PP_SEQ_FOR_EACH(WRAP_ENUM_VALUE, I3Datatype, TYPE_CLASS)
    ;
    
    bp::class_<std::vector<I3Datatype> >("vector_I3Datatype")
        .def(bp::vector_indexing_suite<std::vector<I3Datatype> >())
    ;
}
