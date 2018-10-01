
#ifndef ICETRAY_PYTHON_GET_CLASS_HPP_INCLUDED
#define ICETRAY_PYTHON_GET_CLASS_HPP_INCLUDED

#include <boost/python/converter/registry.hpp>

namespace boost { namespace python {

/// @brief return Python class corresponding to 
template <typename T>
boost::python::object get_class()
{
    namespace bp = boost::python;
    const bp::converter::registration* registration = bp::converter::registry::query(bp::type_id<T>());
    if (registration == NULL)
      return bp::object();
    const PyTypeObject *pytype = registration->expected_from_python_type();
    bp::handle<PyTypeObject> handle(bp::borrowed(const_cast<PyTypeObject*>(pytype)));
    return bp::object(handle);
}

}}

#endif // ICETRAY_PYTHON_GET_CLASS_HPP_INCLUDED
