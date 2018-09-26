/**
 * Helper macros for I3Converter pybindings
 *
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#ifndef PYBINDINGS_H_GNEDBL5C
#define PYBINDINGS_H_GNEDBL5C

#include "tableio/I3Converter.h"
#include <boost/python/class.hpp>
#include <boost/python/scope.hpp>
#include <boost/python/str.hpp>
#include <boost/python/dict.hpp>
#include <boost/make_shared.hpp>

namespace bp = boost::python;

// sets up a namespace "proj.converters" into which pybound converters
// can be exported. this allows you to e.g. export converters defined
// in tableio into the dataclasses module

#define I3CONVERTER_NAMESPACE(proj)                                                                                \
    bp::object module(bp::handle<>(bp::borrowed(PyImport_AddModule("icecube." BOOST_PP_STRINGIZE(proj)))));\
    bp::object converter_module(bp::handle<>(bp::borrowed(PyImport_AddModule("icecube." BOOST_PP_STRINGIZE(proj) ".converters"))));\
    module.attr("converters") = converter_module;\
    bp::object tableio(bp::handle<>(PyImport_Import(bp::str("icecube.tableio").ptr())));\
    bp::object registry(tableio.attr("I3ConverterRegistry"));\
    bp::scope converter_scope = converter_module

// the minimal incantations to export pybindings for converters
// converter must be a legal python identifier (you can typedef this if necessary)

#define I3CONVERTER_EXPORT(converter,docstring)                    \
    register_converter<converter>(registry,I3CONVERTER_EXPORT_IMPL(converter,docstring),false)
    
#define I3CONVERTER_EXPORT_DEFAULT(converter,docstring)             \
    register_converter<converter>(registry,I3CONVERTER_EXPORT_IMPL(converter,docstring),true)

#define I3CONVERTER_EXPORT_IMPL(converter,docstring)               \
    bp::class_<converter,                                          \
               boost::shared_ptr<converter>,                       \
               bp::bases<I3Converter>,                             \
               boost::noncopyable >(BOOST_PP_STRINGIZE(converter),docstring)

// The I3CONVERTER_EXPORT__WITH_CONVERTER_OBJ and
// I3CONVERTER_EXPORT_DEFAULT__WITH_CONVERTER_OBJ macros exports the specified
// converter the same way as the I3CONVERTER_EXPORT and
// I3CONVERTER_EXPORT_DEFAULT macros do, respectively, but they do also store
// the object of the registered converter in order to be able to add more stuff
// to the converter python class using the I3CONVERTER_CONVERTER_OBJ and
// I3CONVERTER_CONVERTER_NAMESPACE macros.

// The I3CONVERTER_CONVERTER_OBJ macro gets the python class C++ object of the
// converter.
// NOTE: In order to use this macro, the converter has to be exported
//       using the I3CONVERTER_EXPORT__WITH_CONVERTER_OBJ or the
//       I3CONVERTER_EXPORT_DEFAULT__WITH_CONVERTER_OBJ macro!
#define I3CONVERTER_CONVERTER_OBJ(converter) \
    converter ## _obj

// Creates a boost::python scope inside the python class of the specified
// converter. This macro should be surrounded by curly brackets including the
// stuff that should be added to the namespace of the converter class!
// NOTE: In order to use this macro, the converter has to be exported
//       using the I3CONVERTER_EXPORT__WITH_CONVERTER_OBJ or the
//       I3CONVERTER_EXPORT_DEFAULT__WITH_CONVERTER_OBJ macro!
#define I3CONVERTER_CONVERTER_NAMESPACE(converter) \
    bp::scope converter ## _obj_scope = I3CONVERTER_CONVERTER_OBJ(converter);

#define I3CONVERTER_EXPORT__WITH_CONVERTER_OBJ(converter,docstring) \
    typedef bp::class_<converter, boost::shared_ptr<converter>, bp::bases<I3Converter>, boost::noncopyable> class_ ## converter ## _t; \
    class_ ## converter ## _t I3CONVERTER_CONVERTER_OBJ(converter) = I3CONVERTER_EXPORT(converter,docstring)

#define I3CONVERTER_EXPORT_DEFAULT__WITH_CONVERTER_OBJ(converter,docstring) \
    typedef bp::class_<converter, boost::shared_ptr<converter>, bp::bases<I3Converter>, boost::noncopyable> class_ ## converter ## _t; \
    class_ ## converter ## _t I3CONVERTER_CONVERTER_OBJ(converter) = I3CONVERTER_EXPORT_DEFAULT(converter,docstring)


// put the converter in a python-side registry, then return the class
// scope for more method-adding.
template<typename Converter,class W,class X1,class X2,class X3> // template args for bp::class_
bp::class_<W,X1,X2,X3> register_converter(bp::object& registry, bp::class_<W,X1,X2,X3> classy,
    bool is_default) {
    bp::object type = bp::object(typename Converter::booked_type()).attr("__class__");
    bp::scope class_scope = classy;
    registry.attr("register")(class_scope,type,is_default);
    return classy;
};

namespace I3ConverterRegistry {

template <typename T>
I3ConverterPtr
GetDefaultConverter()
{
	bp::object tableio(bp::handle<>(PyImport_Import(bp::str("icecube.tableio").ptr())));
	bp::dict registry(tableio.attr("I3ConverterRegistry").attr("defaults"));
	bp::object pytype(bp::object(boost::make_shared<T>()).attr("__class__"));
	
	if (!registry.has_key(pytype))
		return I3ConverterPtr();
	else
		return bp::extract<I3ConverterPtr>(registry[pytype]());
}

};

#endif  
