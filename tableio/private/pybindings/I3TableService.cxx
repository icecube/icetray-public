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

#include <tableio/I3TableRowDescription.h>
#include <tableio/I3TableService.h>

namespace bp = boost::python;

struct I3TableServiceWrapper : I3TableService, bp::wrapper<I3TableService> {
        virtual I3TablePtr CreateTable(const std::string& tableName, 
                                       I3TableRowDescriptionConstPtr description) {
            return this->get_override("CreateTable")(tableName,description);
        };
        virtual void CloseFile() {
            this->get_override("CloseFile")();
        };
   };

void register_I3TableService() {

	// expose the the python-inheritable wrapper class instead of the virtual base class
    bp::class_<I3TableServiceWrapper,
               boost::shared_ptr<I3TableServiceWrapper>,
               boost::noncopyable>("I3TableService")
    .def("CreateTable",&I3TableServiceWrapper::CreateTable)
    .def("GetTable",&I3TableServiceWrapper::GetTable)
    .def("CloseFile",&I3TableServiceWrapper::CloseFile)
    .def("Finish",&I3TableServiceWrapper::Finish)
    
    ;
}
