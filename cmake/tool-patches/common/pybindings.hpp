#ifndef ICETRAY_PYBINDINGS_HPP_INCLUDED
#define ICETRAY_PYBINDINGS_HPP_INCLUDED

#include <icetray/I3FrameObject.h>
#include <I3/name_of.h>

//
// Tell python about basic conversions to/from const and FrameObject pointers
//
template <typename T>
void
register_pointer_conversions()
{
  using boost::python::implicitly_convertible;

  implicitly_convertible<shared_ptr<T>, shared_ptr<I3FrameObject> >();
  implicitly_convertible<shared_ptr<T>, shared_ptr<const T> >();
  implicitly_convertible<shared_ptr<T>, shared_ptr<const I3FrameObject> >();

  //  implicitly_convertible<shared_ptr<I3FrameObject>, shared_ptr<T> >();
  //  implicitly_convertible<shared_ptr<const I3FrameObject>, shared_ptr<const T> >();
  //  boost::python::register_ptr_to_python<boost::shared_ptr<T> >();
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

#include <ctype.h>
#include <string.h>
// convert from upper to lower CamelCase:
// ParticleType => particleType
// ATWDBinSize => atwdBinSize
static inline char * lowerCamelCase(const char * str) 
{
  int len,i;
  char* out;
  len = strlen(str);
  out = (char*)malloc(len+1);
  strcpy(out,str);
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

// convert from upper CamelCase to snake_case
// ATWDBinSize => atwd_bin_size
// NBinsATWD0 => n_bins_atwd_0
static inline string_deleter snake_case(const char * str) 
{
  int i,j,len; 
  char* out; 
  j=0; 
  len = strlen(str); 
  out = (char*)malloc(2*len+1); 
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
#define WRAP_PROP_INTERNAL_REFERENCE(R, Class, Fn) 						\
   .add_property(snake_case(BOOST_PP_STRINGIZE(Fn)),					\
                 boost::python::make_function(BOOST_PP_CAT(&Class::Get,Fn), 			\
                                              boost::python::return_internal_reference<1>()), 	\
                 BOOST_PP_CAT(&Class::Set,Fn))
#define WRAP_PROP_RO(R, Class, Fn) .add_property(snake_case(BOOST_PP_STRINGIZE(Fn)), BOOST_PP_CAT(&Class::Get,Fn))
#define WRAP_PROP_RO_INTERNAL_REFERENCE(R, Class, Fn) 						\
   .add_property(snake_case(BOOST_PP_STRINGIZE(Fn)),					\
                 boost::python::make_function(BOOST_PP_CAT(&Class::Get,Fn), 			\
                                              boost::python::return_internal_reference<1>())) 
#define WRAP_RW(R, Class, Member) .def_readwrite(BOOST_PP_STRINGIZE(Member), &Class::Member)
#define WRAP_RO(R, Class, Member) .def_readonly(BOOST_PP_STRINGIZE(Member), &Class::Member)
#define WRAP_DEF(R, Class, Fn) .def(BOOST_PP_STRINGIZE(Fn), &Class::Fn)
#define WRAP_DEF_RECASE(R, Class, Fn) .def(snake_case(BOOST_PP_STRINGIZE(Fn)), &Class::Fn)
#define WRAP_GET(R, Class, Name) GET_IMPL_NOTYPE(Class, Name, default_call_policies())
#define WRAP_GETSET(R, Class, Name) GETSET_IMPL_NOTYPE(Class, Name, default_call_policies())
#define WRAP_GET_INTERNAL_REFERENCE(R, Class, Name) GET_IMPL_NOTYPE(Class, Name, return_internal_reference<1>())
#define WRAP_GETSET_INTERNAL_REFERENCE(R, Class, Name) GETSET_IMPL_NOTYPE(Class, Name, return_internal_reference<1>())
#define WRAP_ENUM_VALUE(R, Class, Name) .value(BOOST_PP_STRINGIZE(Name),Class::Name)

// Class registration stuff
#define I3_REGISTRATION_FN_DECL(r, data, t) void BOOST_PP_CAT(register_,t)();

// Hey jvs:  this conflicts with the I3_REGISTER in project icetray
// #define I3_REGISTER(r, data, t) BOOST_PP_CAT(register_,t)();



#endif
