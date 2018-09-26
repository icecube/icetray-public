/**
 * I3TableRow.cxx (pybindings)
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

#include <tableio/I3TableRow.h>
#include <dataclasses/I3Vector.h>
#include "type_helpers.h"
#include "const_ptr_helpers.h"
#include "I3TableRow_detail.h"

size_t index_for_field(I3TableRowDescriptionConstPtr desc, const std::string& field) {
    size_t index = desc->GetFieldColumn(field);
    if (index >= desc->GetNumberOfFields()) {
        PyErr_SetString(PyExc_KeyError,field.c_str());
        bp::throw_error_already_set();
    }
    return index;
}

// ========================================================
// = A python-friendly interface to I3TableRowSet<type>() =
// ========================================================
void set_field(I3TableRow& self, const std::string& field, bp::object value, bool all = false) {
    
   I3TableRowDescriptionConstPtr desc = self.GetDescription();
   size_t index = index_for_field(desc,field);
   I3Datatype dtype = desc->GetFieldTypes().at(index);
   size_t array_length = desc->GetFieldArrayLengths().at(index);
   
   bool success = false;
   
   // = Case 1: field holds a scalar type =
   if (array_length == 1) {
       success = true;
       set_index<I3DatatypeDispatcher<set_scalar> >(self,dtype,index,value,all);
   
   // = Case 2: field holds a vector type =
   } else { // handle vectors
       // = Case 2.1: passed object is an array.array or numpy.ndarray =
       success = try_set_ndarray(self,dtype,index,array_length,value,all);
       
       // = Case 2.2: passed object is a wrapped I3Vector =
       if (!success) {
           success = set_index<I3VectorDispatcher<set_vector> >(self,dtype,index,value,all);           
       }  
   }
   // = If nothing could handle the value, raise ValueError =
   if (!success) {
      std::ostringstream msg;
      std::string classname = bp::extract<std::string>(value.attr("__class__").attr("__name__"));
      msg << "I3TableRow.set() can't interpret Python type '" << classname << "'.";
      PyErr_SetString(PyExc_ValueError, msg.str().c_str());
      throw bp::error_already_set();
   }
}

BOOST_PYTHON_FUNCTION_OVERLOADS(set_overloads,set_field,3,4);



// =========================================================
// = A python-friendly interface to I3TableRow.Get<type>() =
// =========================================================
bp::object getitem(I3TableRow& self, const std::string& field) {
   
   I3TableRowDescriptionConstPtr desc = self.GetDescription();
   size_t index = index_for_field(desc,field);
   I3Datatype dtype = desc->GetFieldTypes().at(index);
   
   // = Case 1: memory chunk holds a scalar =
   if (desc->GetFieldArrayLengths().at(index) == 1) { // handle scalars
       return get_object<get_scalar>(self,dtype,index);
   // = Case 2: memory chunk holds a vector =
   } else {
       return get_object<get_list>(self,dtype,index);
   }
}

// ======================
// = Get a list of keys =
// ======================
bp::list keys(I3TableRow& self) {
   I3TableRowDescriptionConstPtr desc = self.GetDescription();
   const std::vector<std::string>& fields = desc->GetFieldNames();
   std::vector<std::string>::const_iterator it;
   bp::list l;
   for (it=fields.begin();it!=fields.end();it++) {
      l.append(*it);
   }
   return l;
}

// =========================================================================
// = Initialize I3TableRow with an unsigned int, which Python doesn't have =
// =========================================================================
static boost::shared_ptr<I3TableRow> int_init( I3TableRowDescriptionPtr desc, int num_rows) {
   if (num_rows < 0) {
      PyErr_SetString(PyExc_ValueError, "num_rows must be >= 0");
      throw bp::error_already_set();
   } 
   return boost::shared_ptr<I3TableRow>(new I3TableRow(desc,static_cast<size_t>(num_rows)));
} 

// ================================
// = set_field with 'all = false' =
// ================================
void setitem(I3TableRow& self, const std::string& field, bp::object value) {
   set_field(self,field,value);
}


void register_I3TableRow() {
      
   bp::class_<I3TableRow,
      boost::shared_ptr<I3TableRow> >
      ("I3TableRow",
"\n\
An I3TableRow is a container for generic tabular data, i.e. columns and      \n\
rows. The associated I3TableRowDescriptionObject describes the format of the \n\
columns. Each column has a name and a datatype. The following basic data     \n\
types are supported::                                                        \n\
                                                                             \n\
     +------+----------------+------------------------+                      \n\
     | Code | Native type    | Equivalent Python type |                      \n\
     +======+================+========================+                      \n\
     |  'c' | char           |                        |                      \n\
     |  'b' | signed char    |                        |                      \n\
     |  'B' | unsigned char  |                        |                      \n\
     |  'h' | signed short   |                        |                      \n\
     |  'H' | unsigned short |                        |                      \n\
     |  'i' | signed int     | int                    |                      \n\
     |  'I' | unsigned int   |                        |                      \n\
     |  'l' | signed long    | long                   |                      \n\
     |  'L' | unsigned long  |                        |                      \n\
     |  'f' | float          |                        |                      \n\
     |  'd' | double         | float                  |                      \n\
     |  'o' | bool           | bool                   |                      \n\
     +------+----------------+------------------------+                      \n\
                                                                             \n\
Data can be filled into or extracted from the rows of the object using the   \n\
standard dictionary notation:                                                \n\
                                                                             \n\
   desc = I3TableRowDescription()                                            \n\
   desc.add_field('foo','i','radians','The cut of foo\'s jib.')              \n\
   rows = I3TableRow(desc,5)                                                 \n\
                                                                             \n\
   for i in range(rows.number_of_rows)                                       \n\
      rows.current_row = i                                                   \n\
      rows['foo'] = i*5                                                      \n\
                                                                             \n\
Columns may also hold fixed-length vectors of homogenous data. If the        \n\
I3TableRow object was created thusly:                                        \n\
                                                                             \n\
   desc = I3TableRowDescription()                                            \n\
   desc.add_field('raw_waveform','H','counts','Digitizer counts',128)        \n\
   rows = I3TableRow(desc,1)                                                 \n\
                                                                             \n\
then the column 'raw_waveform' can be filled with a wrapped instance of      \n\
I3Vector                                                                     \n\
                                                                             \n\
   vec = dataclasses.I3VectorInt()                                           \n\
   for i in range(128): vec.append(i)                                        \n\
   rows['raw_waveform'] = vec                                                \n\
                                                                             \n\
or an array from the standard library array module                           \n\
                                                                             \n\
   import array                                                              \n\
   arr = array.array('H',range(128))                                         \n\
   rows['raw_waveform'] = arr                                                \n\
                                                                             \n\
or even a 1-d Numpy array                                                    \n\
                                                                             \n\
   import numpy                                                              \n\
   numpy_arr = numpy.array(range(128),'H')                                   \n\
   rows['raw_waveform'] = numpy_arr                                          \n\
                                                                             \n\
The datatype of the array must match the datatype of the column, or an error \n\
will be raised.                                                              \n\
",\
      bp::no_init)
   .def("__init__",bp::make_constructor(&int_init))
   #define PROPS (CurrentRow)(NumberOfRows)
   #define PROPS_RO (Description)
   BOOST_PP_SEQ_FOR_EACH(WRAP_PROP,I3TableRow,PROPS)
   BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO,I3TableRow,PROPS_RO)
   .def("set",set_field,set_overloads(bp::args("self","field","value","all")))
   .def("__setitem__",setitem)
   .def("__getitem__",getitem)
   .def("keys",keys)
   ;
   
   // register implicit conversions for const pointers
   utils::register_const_ptr<I3TableRow>();
}
