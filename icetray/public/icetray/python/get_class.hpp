// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef ICETRAY_PYTHON_GET_CLASS_HPP_INCLUDED
#define ICETRAY_PYTHON_GET_CLASS_HPP_INCLUDED

#include "pybindings.hpp"
#include <boost/python/converter/registry.hpp>
#include <set>

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
    if (pytype != nullptr) {
      return bp::object(bp::handle<PyTypeObject>(bp::borrowed(const_cast<PyTypeObject*>(pytype))));
    } else {
      // no unique from_python type
      std::set<PyTypeObject const*> pool;
      for(auto* r = registration->rvalue_chain; r ; r=r->next) {
        // each source type may itself have multiple implicit conversions,
        // which we can't see through
        const PyTypeObject *unique_type;
        if(r->expected_pytype && (unique_type = r->expected_pytype()) != nullptr) {
          pool.insert(unique_type);
        }
      }
      if (!pool.empty()) {
        bp::list union_members;
        for (const PyTypeObject *pytype: pool) {
          union_members.append(bp::handle<PyTypeObject>(bp::borrowed(const_cast<PyTypeObject*>(pytype))));
        }
        return bp::tuple(union_members);
      }
    }

    return bp::object();
}

}}

#endif // ICETRAY_PYTHON_GET_CLASS_HPP_INCLUDED
