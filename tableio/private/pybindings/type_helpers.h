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

#ifndef TYPE_HELPERS_H_M3WNFWYF
#define TYPE_HELPERS_H_M3WNFWYF

#include "tableio/I3Datatype.h"
#include "dataclasses/I3Vector.h"
#include <sstream>

char PyArrayTypecode_from_I3Datatype(const I3Datatype& dtype);
boost::shared_ptr<I3Datatype> I3Datatype_from_PyArrayTypecode(char code);
boost::shared_ptr<I3Datatype> I3Datatype_from_PyObject(boost::python::object obj);
boost::shared_ptr<I3Datatype> I3Datatype_from_NumpyDtype(boost::python::object obj);

/*
 * A dispatcher to call the native template matching an I3Datatype
 *
 */

template <typename T>
struct I3DatatypeDispatcher
{
  typedef T visitor_type;  
  I3DatatypeDispatcher() { }

  void route(I3Datatype& dtype, T& visitor)
  {
      std::ostringstream message;
      switch (dtype.kind) {
           case I3Datatype::Bool:
               visitor.template call<bool>();
               break;
           case I3Datatype::Enum:
               // fall through to the normal integers
           case I3Datatype::Int:
               if (dtype.is_signed) {
                   if (dtype.size == 1)      visitor.template call<int8_t>();
                   else if (dtype.size == 2) visitor.template call<int16_t>();
                   else if (dtype.size == 4) visitor.template call<int32_t>();
                   else if (dtype.size == 8) visitor.template call<int64_t>();
                   else {
                       message << dtype.size << "-byte integers are not supported";
                       visitor.fail(message.str());
                   }
               } else {
                   if (dtype.size == 1)      visitor.template call<uint8_t>();
                   else if (dtype.size == 2) visitor.template call<uint16_t>();
                   else if (dtype.size == 4) visitor.template call<uint32_t>();
                   else if (dtype.size == 8) visitor.template call<uint64_t>();
                   else {
                       message << dtype.size << "-byte integers are not supported";
                       visitor.fail(message.str());
                   }
               }
               break;
           case I3Datatype::Float:
               if (dtype.size == 4)      visitor.template call<float>();
               else if (dtype.size == 8) visitor.template call<double>();
               else {
                      message << dtype.size << "-byte floats are not supported";
                      visitor.fail(message.str());
               }
               break;
           default:
               visitor.fail(std::string("I don't know how to handle your datatype."));
      }
  };
};


/*
 * A dispatcher to call the proper extract() template for
 * a wrapped I3Vector.
 *
 */
 
template <typename T>
struct I3VectorDispatcher
{
  typedef T visitor_type;
  I3VectorDispatcher() { }

#define EXTRACTOR(type) boost::python::extract<I3Vector<type> > type##_e(obj)
#define CHECK_CALL(type) else if (dtype == I3DatatypeFromNativeType<type>() && type##_e.check()) visitor.template call<type>()

  void route(I3Datatype& dtype, T& visitor)
  {
      boost::python::object obj = visitor.value;
      EXTRACTOR(bool);
      EXTRACTOR(int8_t);
      EXTRACTOR(int16_t);
      EXTRACTOR(int32_t);
      EXTRACTOR(int64_t);
      EXTRACTOR(uint8_t);
      EXTRACTOR(uint16_t);
      EXTRACTOR(uint32_t);
      EXTRACTOR(uint64_t);
      EXTRACTOR(float);
      EXTRACTOR(double);
      
      if (false) {}
      CHECK_CALL(bool);
      CHECK_CALL(int8_t);
      CHECK_CALL(int16_t);
      CHECK_CALL(int32_t);
      CHECK_CALL(int64_t);
      CHECK_CALL(uint8_t);
      CHECK_CALL(uint16_t);
      CHECK_CALL(uint32_t);
      CHECK_CALL(uint64_t);
      CHECK_CALL(float);
      CHECK_CALL(double);
      else {
          visitor.fail(std::string("I don't know how to handle your datatype."));
      }
      
  };

#undef EXTRACTOR
#undef CHECK_CALL
  
};

namespace I3Datatypes {
  enum NativeType {
    UNKNOWN,
    NATIVE_SCHAR,
    NATIVE_SHORT,
    NATIVE_INT,
    NATIVE_LONG,
    NATIVE_LLONG,
    NATIVE_UCHAR,
    NATIVE_USHORT,
    NATIVE_UINT,
    NATIVE_ULONG,
    NATIVE_ULLONG,
    NATIVE_FLOAT,
    NATIVE_DOUBLE,
    NATIVE_BOOL
  };
  NativeType GetNativeType(const I3Datatype& dtype);  
};

#endif /* end of include guard: TYPE_HELPERS_H_M3WNFWYF */

