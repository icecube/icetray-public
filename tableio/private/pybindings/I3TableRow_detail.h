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

#ifndef TABLEIO_I3TABLEROWDETAIL_H_INCLUDED
#define TABLEIO_I3TABLEROWDETAIL_H_INCLUDED

#include <tableio/I3TableRow.h>
#include <dataclasses/I3Vector.h>
#include "type_helpers.h"

 namespace bp = boost::python;

/****************************************************************************/
// helpful error messages

void throw_unless_match(I3Datatype& value_dtype, I3Datatype& field_dtype) {
    if (value_dtype != field_dtype) {
        std::ostringstream mesg;
        mesg << "Can't copy values of type " << value_dtype.AsString() << " to a field with type " << field_dtype.AsString() << ".";
        PyErr_SetString(PyExc_TypeError,mesg.str().c_str());
        bp::throw_error_already_set();
    }
}

void throw_unless_fits(size_t array_size, size_t field_size) {
    if (array_size > field_size) {
        std::ostringstream mesg;
        mesg << "Can't fit a " << array_size << "-element vector into a " << field_size << "-element array-field.";
        PyErr_SetString(PyExc_ValueError,mesg.str().c_str());
        bp::throw_error_already_set();
    }
}

/****************************************************************************/

// A visitor for setting scalar values
 struct set_scalar {
     I3TableRow& self;
     size_t index;
     bp::object value;
     bool all;
     bool success;
     set_scalar(I3TableRow& s,size_t i,bp::object v,bool a) 
         : self(s), index(i), value(v), all(a), success(true) {};

     template <typename T>
     void call() {
         T v = bp::extract<T>(value);
         // temporarily turn off enum-checking
         self.SetEnumsAreInts(true);
         self.Set(index,v,all);
         self.SetEnumsAreInts(false);
     };

     void fail(std::string message) {
         success = false;
         log_fatal("Badness: %s",message.c_str());
     };

 };
 
 /****************************************************************************/

// A visitor for setting from wrapped I3Vectors
 struct set_vector {
     I3TableRow& self;
     size_t index;
     bp::object value;
     bool all;
     bool success;
     set_vector(I3TableRow& s,size_t i,bp::object v,bool a) 
         : self(s), index(i), value(v), all(a), success(true) {};
     template <typename T>
     void call() {
         std::vector<T> v = bp::extract<I3Vector<T> >(value);
         I3TableRowDescriptionConstPtr desc = self.GetDescription();
         
         // check that datatypes match
         I3Datatype dtype = desc->GetFieldTypes().at(index);
         I3Datatype vec_dtype = I3DatatypeFromNativeType<T>();
         throw_unless_match(vec_dtype,dtype);
         
         // check for an overflow
         throw_unless_fits(v.size(), desc->GetFieldArrayLengths().at(index));
        
         T* block = self.GetPointer<T>(index);
         std::copy(v.begin(),v.end(),block);
     };

     void fail(std::string message) {
         success = false;
         log_fatal("Badness: %s",message.c_str());
     };

 };
 
 /****************************************************************************/

// a function to set field values with a visitor and dispatcher
 template<typename Dispatcher>
 bool set_index(I3TableRow& self, I3Datatype& dtype, size_t index, bp::object value, bool all) {
     Dispatcher dispatcher;
     typename Dispatcher::visitor_type visitor(self,index,value,all);
     dispatcher.route(dtype,visitor);
     return visitor.success;
 };
 
 /****************************************************************************/
 
 // =======================================================
 // = Copy the contents of a buffer into the memory chunk =
 // =======================================================
 bool try_set_ndarray(I3TableRow& self, I3Datatype dtype, size_t index, size_t array_length, bp::object value, bool all = false) {
     bool is_array = (value.attr("__class__").attr("__name__") == std::string("array"));
     bool is_ndarray = (value.attr("__class__").attr("__name__") == std::string("ndarray"));
     boost::shared_ptr<I3Datatype> arr_dtype;
     // const std::string field = self.GetDescription()->GetFieldNames().at(index);
     // ==============================================================
     // = Case 2.1: passed object is an array.array or numpy.ndarray =
     // ==============================================================
     if (is_array || is_ndarray) {
        if (is_array) {
           std::string typecode = bp::extract<std::string>(value.attr("typecode"));
           char arr_typecode = typecode[0];
           arr_dtype = I3Datatype_from_PyArrayTypecode(arr_typecode);
        } else {
           arr_dtype = I3Datatype_from_NumpyDtype(value.attr("dtype"));
        }
        // ==============================================
        // = Check the type and byte order of the array =
        // ==============================================
        if (!arr_dtype) log_fatal("Type of array could not be recognized.");
        throw_unless_match(*arr_dtype,dtype);

        Py_buffer view;
        int ret = PyObject_GetBuffer(value.ptr(), &view, PyBUF_SIMPLE);
        if (ret == 0) {
            // =======================================================
            // = Copy the buffer (an array in native representation) 
            //   directly into the memory chunk                      =
            // =======================================================
            throw_unless_fits(static_cast<size_t>(view.len)/(dtype.size),array_length);
            
            size_t start,stop,i;
            void* pointy;
            if (all) {
                start = 0;
                stop = self.GetNumberOfRows();
            } else {
                start = self.GetCurrentRow();
                stop = start+1;
            }
            for (i = start; i < stop; i++) {
                pointy = self.GetPointerToField(index,i);
                memcpy(pointy,view.buf,view.len);
            }
        }
        PyBuffer_Release(&view);
        return true;
    } else {
        return false;
    }
 }
 
 /****************************************************************************/


// ==========================================================
// = Visitor that gets a scalar value from the memory chunk =
// ==========================================================
struct get_scalar {
    I3TableRow& self;
    size_t index;
    bp::object result;
    get_scalar(I3TableRow& s,size_t i) : self(s), index(i) {};
    
    template <typename T>
    void call() {
        self.SetEnumsAreInts(true);
        result = bp::object(self.Get<T>(index));
        self.SetEnumsAreInts(false);
    };
    
    void fail(std::string message) {
        log_fatal("Badness: %s",message.c_str());
    };

};

 /****************************************************************************/

// ===========================================================
// = Visitor that fills contents of memory chunk into a list =
// ===========================================================
struct get_list {
    I3TableRow& self;
    size_t index;
    bp::object result;
    get_list(I3TableRow& s,size_t i) : self(s), index(i) {};
    
    template <typename T>
    void call() {
        bp::list l;
        I3TableRowDescriptionConstPtr desc = self.GetDescription();
        size_t length = desc->GetFieldArrayLengths().at(index);
        T* block = self.GetPointer<T>(index);
        for (size_t i=0; i<length; i++) l.append(block[i]);
        result = l;
    };
    
    void fail(std::string message) {
        log_fatal("Badness: %s",message.c_str());
    };

};

// Get the value of a field using the specified visitor struct
template<typename T>
bp::object get_object(I3TableRow& self,I3Datatype& dtype,size_t index) {
    I3DatatypeDispatcher<T> dispatcher;
    T visitor(self,index);
    dispatcher.route(dtype,visitor);
    return visitor.result;
}

#endif // TABLEIO_I3TABLEROWDETAIL_H_INCLUDED
