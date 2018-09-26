/**
 * I3Table.cxx (pybindings)
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

#include <tableio/I3Table.h>
#include <tableio/I3TableService.h>

namespace bp = boost::python;

struct I3TableWrapper : public I3Table, public bp::wrapper<I3Table> {
        I3TableWrapper(I3TableService& service, 
                         std::string name,
                         I3TableRowDescriptionConstPtr description) : I3Table(service, name, description) {};
        virtual void WriteRows(I3TableRowConstPtr row) {
            this->get_override("WriteRows")(row);
        };
};

void register_I3Table() {
    
 // expose the the python-inheritable wrapper class instead of the virtual base class
    bp::class_<I3TableWrapper,
               boost::shared_ptr<I3TableWrapper>,
               boost::noncopyable>("I3Table",
                        bp::init<I3TableService&, std::string,
                        I3TableRowDescriptionConstPtr>())
    .def("WriteRows",&I3TableWrapper::WriteRows)
    
    
    ;
    
    bp::register_ptr_to_python< boost::shared_ptr<I3Table> >();
    
}

