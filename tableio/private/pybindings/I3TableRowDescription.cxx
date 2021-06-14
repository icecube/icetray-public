/**
 * I3TableRowDescription.cxx (pybindings)
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
#include "const_ptr_helpers.h"

namespace bp = boost::python;

void add_field(I3TableRowDescription& self, const std::string& name, I3Datatype type,
               const std::string& unit, const std::string& doc, size_t array_length = 1) {
    self.AddField(name,type,unit,doc,array_length);
}

BOOST_PYTHON_FUNCTION_OVERLOADS(add_field_overloads,add_field,5,6)

void register_I3TableRowDescription() {


   bp::class_<I3TableRowDescription, 
      boost::shared_ptr<I3TableRowDescription>
      >
      ("I3TableRowDescription")
      #define RO_VEC_PROPERTIES (FieldNames)(FieldTypes)(FieldTypeSizes)(FieldByteOffsets)(FieldArrayLengths)(FieldUnits)(FieldDocStrings)
      #define RO_PROPERTIES (TotalByteSize)(TotalChunkSize)(NumberOfFields)
      #define PROPERTIES (IsMultiRow)
      BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO_INTERNAL_REFERENCE,I3TableRowDescription,RO_VEC_PROPERTIES)
      BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO,I3TableRowDescription,RO_PROPERTIES)
      BOOST_PP_SEQ_FOR_EACH(WRAP_PROP,I3TableRowDescription,PROPERTIES)
      
      .def("add_field", &add_field,
         add_field_overloads(
            bp::args("name","dtype","units","docstring","array_size"),
            
"\nAdd a field to the table description.                                      \n\
                                                                              \n\
Parameters                                                                    \n\
----------                                                                    \n\
name : str                                                                    \n\
   The name of the field. This should be a valid Python identifier.           \n\
dtype : I3Datatype                                                            \n\
   The datatype to be stored in the field. I3Datatypes can be constructed     \n\
   from wrapped enumerated types (Boost.Python.enum), which case the names    \n\
   of the enum members will be extracted and stored in the final table.       \n\
units : str                                                                   \n\
   The units of the values stored in the field (e.g. 'mV', 'm', 'Gm/s')       \n\
docstring : str                                                               \n\
   A description of quantity stored in the field (e.g. where it comes from and\n\
   how it was calculated).                                                    \n\
array_size : int                                                              \n\
   The number of elements in the field. If this is 1 (default), then the field\n\
   is a scalar. Otherwise, the field will store a fixed-length vector         \n\
   quantity.                                                                  \n\
"))
   ;
   
   // register implicit conversions for const pointers
   utils::register_const_ptr<I3TableRowDescription>();

}
