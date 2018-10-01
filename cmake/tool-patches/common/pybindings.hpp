#ifndef ICETRAY_PYBINDINGS_HPP_INCLUDED
#define ICETRAY_PYBINDINGS_HPP_INCLUDED

#include <cstring>
#include <cctype>
#include <boost/shared_ptr.hpp>
#include "icetray/I3FrameObject.h"
#include "I3/name_of.h"

class freeze : public boost::python::def_visitor< freeze >{
  friend class boost::python::def_visitor_access;
private:
  static int HasAttrWithException(PyObject *v, PyObject *name){
    PyObject *res = PyObject_GetAttr(v, name);
    if (res != NULL) {
      Py_DECREF(res);
      return 1;
    }   
    
    if ((PyErr_Occurred() != NULL) &&
        PyErr_ExceptionMatches(PyExc_RuntimeError)) {
      PyErr_Clear();
      return 1;
    }   

    PyErr_Clear();
    return 0;
  };  
  // also had to make this public
public:
  template <class classT>
  void visit( classT& c) const {
    c
      .def("__setattr__", &setattr_with_dynamism_disabled)
      ;
    };

  static void setattr_with_dynamism_disabled( const boost::python::object& obj, 
				       const boost::python::object& name, 
				       const boost::python::object& value){
    if( HasAttrWithException(obj.ptr(), name.ptr()) ){
      if( PyObject_GenericSetAttr( obj.ptr(), name.ptr(), value.ptr()) == -1)
        throw boost::python::error_already_set();
    }else{
      std::stringstream ss;
      std::string clname = boost::python::extract<std::string>( obj.attr("__class__").attr("__name__") );
      std::string attname = boost::python::extract<std::string>( name );
      ss<<"*** The dynamism of this class has been disabled"<<std::endl;
      ss<<"*** Attribute ("<<attname<<") does not exist in class "<<clname<<std::endl; 
      PyErr_SetString(PyExc_AttributeError, ss.str().c_str());
      throw boost::python::error_already_set();
    }
  }
};


// Tell python about basic conversions to/from const and FrameObject pointers
//
template <typename T>
void
register_pointer_conversions()
{
  using boost::python::implicitly_convertible;

  implicitly_convertible<boost::shared_ptr<T>, boost::shared_ptr<I3FrameObject> >();
  implicitly_convertible<boost::shared_ptr<T>, boost::shared_ptr<const T> >();
  implicitly_convertible<boost::shared_ptr<T>, boost::shared_ptr<const I3FrameObject> >();
}

template <typename T> 
T 
identity_(T t) { return t; }


//
//  macros for general use
//
// GETSET implementation.  Pass the name of the Type, 
// the name of the member to access via Get*** Set***,
// and the return value policy.
// Used by the two macros below
#define GETSET_IMPL(ObjType, GotType, Name, CallPolicies)		\
  .def("Get" BOOST_PP_STRINGIZE(Name),					\
       (GotType (ObjType::*) ()) &ObjType:: BOOST_PP_CAT(Get,Name),	\
       CallPolicies)							\
  .def("Set" BOOST_PP_STRINGIZE(Name),					\
       &ObjType:: BOOST_PP_CAT(Set,Name))

// A version suitable for use with sequences
#define GETSET_IMPL_NOTYPE(ObjType, Name, CallPolicies)			\
  GET_IMPL_NOTYPE(ObjType, Name, CallPolicies)				\
  .def("Set" BOOST_PP_STRINGIZE(Name),					\
       &ObjType:: BOOST_PP_CAT(Set,Name))

#define GET_IMPL_NOTYPE(ObjType, Name, CallPolicies)			\
  .def("Get" BOOST_PP_STRINGIZE(Name),					\
       &ObjType:: BOOST_PP_CAT(Get,Name),				\
       CallPolicies)							\

// This is for member functions that return a Whatever&,
// where the actual Whatever is owned by the class.
#define GETSET_INTERNAL_REFERENCE(ObjType, GotType, Name)		\
  GETSET_IMPL(ObjType, GotType, Name, return_internal_reference<1>())

// This is for getters and setters that return PODS by value
#define GETSET(Objtype, GotType, Name)			\
  GETSET_IMPL(Objtype, GotType, Name, default_call_policies())

#define PROPERTY(Class, Prop, Fn) .add_property(BOOST_PP_STRINGIZE(Prop), &Class::Get##Fn, &Class::Set##Fn)
#define PROPERTY_TYPE(Class, Prop, GotType, Fn) .add_property(BOOST_PP_STRINGIZE(Prop), (GotType (Class::*) ()) &Class::Get##Fn, &Class::Set##Fn)

/** convert from upper to lower CamelCase:
 * ParticleType => particleType
 * ATWDBinSize => atwdBinSize
 *
 * The return value must be free()'d by the user.
 * 
 * @param str the camel case string to change
 * @return a pointer to the new malloc()'d string
 */
static inline char * lowerCamelCase(const char * str) 
{
  int len,i;
  char* out;
  len = strlen(str);
  out = (char*)malloc(len+1);
  memset(out, '\0', len+1);
  strncat(out,str,len);
  out[0] = tolower(out[0]);
  for (i=1;i<len-1;i++) {
    if (!(isupper(out[i]) && isupper(out[i+1]))) break;
    out[i] = tolower(out[i]);
  }
  return out;
}

//
//  This deletes the string returned by snake_case when it goes out of
//  scope.  Otherwise the WRAP_PROP stuff below leaks.
//
struct string_deleter
{
  char* s;
  string_deleter(char* s_) : s(s_) { }
  operator char*() { return s; }
  ~string_deleter() { free(s); }
};

/** convert from upper CamelCase to snake_case
 * ATWDBinSize => atwd_bin_size
 * NBinsATWD0 => n_bins_atwd_0
 *
 * @param str the camel case string to change
 * @return a pointer to the new string
 */
static inline string_deleter snake_case(const char * str) 
{
  int i,j,len; 
  char* out; 
  j=0; 
  len = strlen(str); 
  out = (char*)malloc(2*len+1); 
  memset(out, '\0', 2*len+1);
  for (i=0;i<len;i++) {
    out[i+j] = tolower(str[i]); 
    if ( (!isupper(str[i]) && isupper(str[i+1])) || 
	 ((i<len-1) && isupper(str[i+1]) && islower(str[i+2])) || 
	 (!isalpha(str[i+1]) && (str[i+1] != '\0')) ) {
      j++; 
      out[i+j] = '_'; 
    } 
  } 
  out[len+j] = '\0'; 
  out = (char*)realloc(out,len+j+1); 
  return string_deleter(out); 
} 

#define I3_PYTHON_MODULE(N) BOOST_PYTHON_MODULE(N)

#include <boost/preprocessor/seq.hpp>

// Trinary macros suitable for use with BOOST_PP_SEQ_FOR_EACH
#define WRAP_PROP(R, Class, Fn) .add_property(snake_case(BOOST_PP_STRINGIZE(Fn)), BOOST_PP_CAT(&Class::Get,Fn), BOOST_PP_CAT(&Class::Set,Fn))
#define WRAP_PROP_BOOL(R, Class, Fn) .add_property(snake_case(BOOST_PP_STRINGIZE(Fn)), &Class::Fn)
#define WRAP_EVIL_PROP(R, Class, Fn) .def(snake_case(BOOST_PP_STRINGIZE(Fn)), boost::python::make_indexed_property(BOOST_PP_CAT(&Class::Get,Fn), BOOST_PP_CAT(&Class::Set,Fn)))
#define WRAP_PROP_INTERNAL_REFERENCE(R, Class, Fn) 						\
   .add_property(snake_case(BOOST_PP_STRINGIZE(Fn)),					\
                 boost::python::make_function(BOOST_PP_CAT(&Class::Get,Fn), 			\
                                              boost::python::return_internal_reference<1>()), 	\
                 BOOST_PP_CAT(&Class::Set,Fn))
#define WRAP_PROP_RO(R, Class, Fn) .add_property(snake_case(BOOST_PP_STRINGIZE(Fn)), BOOST_PP_CAT(&Class::Get,Fn))
#define WRAP_EVIL_PROP_RO(R, Class, Fn) .def(snake_case(BOOST_PP_STRINGIZE(Fn)), boost::python::make_indexed_property(BOOST_PP_CAT(&Class::Get,Fn)))
#define WRAP_PROP_RO_INTERNAL_REFERENCE(R, Class, Fn) 						\
   .add_property(snake_case(BOOST_PP_STRINGIZE(Fn)),					\
                 boost::python::make_function(BOOST_PP_CAT(&Class::Get,Fn), 			\
                                              boost::python::return_internal_reference<1>())) 
#define WRAP_RW(R, Class, Member) .def_readwrite(BOOST_PP_STRINGIZE(Member), &Class::Member)
#define WRAP_RW_RECASE(R, Class, Member) .def_readwrite(snake_case(BOOST_PP_STRINGIZE(Member)), &Class::Member)
#define WRAP_RO(R, Class, Member) .def_readonly(BOOST_PP_STRINGIZE(Member), &Class::Member)
#define WRAP_DEF(R, Class, Fn) .def(BOOST_PP_STRINGIZE(Fn), &Class::Fn)
#define WRAP_DEF_RECASE(R, Class, Fn) .def(snake_case(BOOST_PP_STRINGIZE(Fn)), &Class::Fn)
#define WRAP_GET(R, Class, Name) GET_IMPL_NOTYPE(Class, Name, default_call_policies())
#define WRAP_GETSET(R, Class, Name) GETSET_IMPL_NOTYPE(Class, Name, default_call_policies())
#define WRAP_GET_INTERNAL_REFERENCE(R, Class, Name) GET_IMPL_NOTYPE(Class, Name, return_internal_reference<1>())
#define WRAP_GETSET_INTERNAL_REFERENCE(R, Class, Name) GETSET_IMPL_NOTYPE(Class, Name, return_internal_reference<1>())
#define WRAP_ENUM_VALUE(R, Class, Name) .value(BOOST_PP_STRINGIZE(Name),Class::Name)

// Raise a specific built-in Python exception with a formatted message
#define RAISE(exception, msg) { std::ostringstream s; s << msg; \
    PyErr_SetString(PyExc_##exception, s.str().c_str()); throw boost::python::error_already_set(); }

#endif
