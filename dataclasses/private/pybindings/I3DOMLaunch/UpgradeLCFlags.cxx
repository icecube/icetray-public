/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#include <vector>

#include <dataclasses/physics/detail/UpgradeLCFlags.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/ostream_overloads.hpp>

using namespace boost::python;

// Need to define a dummy class here in order to get the bitwise operators
// to work properly in python...
struct UpgradeLCFlagsPy
{
  using enum_type = UpgradeLCFlags;

  UpgradeLCFlags value;

  explicit UpgradeLCFlagsPy(enum_type v = enum_type::NoLC)
    : value(v) {}

  // Bitwise operators
  UpgradeLCFlagsPy operator|(const UpgradeLCFlagsPy& other) const {
    return UpgradeLCFlagsPy(value | other.value);
  }

  UpgradeLCFlagsPy operator&(const UpgradeLCFlagsPy& other) const {
    return UpgradeLCFlagsPy(value & other.value);
  }

  UpgradeLCFlagsPy operator^(const UpgradeLCFlagsPy& other) const {
    return UpgradeLCFlagsPy(value ^ other.value);
  }

  UpgradeLCFlagsPy operator~() const {
    return UpgradeLCFlagsPy(~value);
  }

  bool operator==(const UpgradeLCFlagsPy& other) const {
    return value == other.value;
  }

  bool operator!=(const UpgradeLCFlagsPy& other) const {
    return value != other.value;
  }

  explicit operator bool() const {
    return value != enum_type::NoLC;
  }
};

// To pass this python dummy wrapper into C++ modules, we have to
// tell boost::python that they're convertable.
struct UpgradeLCFlags_from_python
{
  static void* convertible(PyObject* obj_ptr)
  {
    // Check whether obj_ptr can be extracted as UpgradeLCFlagsPy
    object obj(handle<>(borrowed(obj_ptr)));
    extract<UpgradeLCFlagsPy> extractor(obj);
    if(extractor.check()){
      return obj_ptr;
    }
    return nullptr;
  }

  static void construct(PyObject* obj_ptr,
      converter::rvalue_from_python_stage1_data* data)
  {
    // Extract wrapper
    object obj(handle<>(borrowed(obj_ptr)));
    UpgradeLCFlagsPy wrapper = extract<UpgradeLCFlagsPy>(obj);

    // Construct enum in-place
    void* storage =
      ((converter::rvalue_from_python_storage<UpgradeLCFlags>*)data)->storage.bytes;

    new (storage) UpgradeLCFlags(wrapper.value);
    data->convertible = storage;
  }

 template <typename Container>
  UpgradeLCFlags_from_python& from_python(){
   converter::registry::push_back(&UpgradeLCFlags_from_python::convertible,
                                 &UpgradeLCFlags_from_python::construct,
                                 boost::python::type_id<UpgradeLCFlags>());
   return *this;
 }
};

// And a to_python converter since we apparently need it...
struct UpgradeLCFlags_to_python
{
  static PyObject* convert(const UpgradeLCFlags& f)
  {
    return incref(object(UpgradeLCFlagsPy(f)).ptr());
  }  
};

inline std::string to_string(const UpgradeLCFlagsPy& f)
{
  std::ostringstream oss;
  oss << f.value;
  return oss.str();
}

// Helper functions to disable dynamic attributes
static void disallow_setattr(UpgradeLCFlagsPy& self, str name, object value) {
    PyErr_SetString(PyExc_AttributeError, 
        "UpgradeLCFlags does not support dynamic attribute assignment");
    throw_error_already_set();
}

static void disallow_delattr(UpgradeLCFlagsPy& self, str name) {
    PyErr_SetString(PyExc_AttributeError, 
        "UpgradeLCFlags does not support attribute deletion");
    throw_error_already_set();
}

void register_UpgradeLCFlags()
{
  scope flags_scope = class_<UpgradeLCFlagsPy>("UpgradeLCFlags", init<>())
    .def(init<UpgradeLCFlags>())
    
    // Operators
    .def(self == self)
    .def(self != self)
    .def("__or__",  &UpgradeLCFlagsPy::operator|)
    .def("__and__", &UpgradeLCFlagsPy::operator&)
    .def("__xor__", &UpgradeLCFlagsPy::operator^)
    .def("__invert__", &UpgradeLCFlagsPy::operator~)
    .def("__str__",  &to_string)
    .def("__repr__", &to_string)
    .def("__bool__", &UpgradeLCFlagsPy::operator bool)    

    // Disable dynamic attributes
    .def("__setattr__", &disallow_setattr)
    .def("__delattr__", &disallow_delattr)
    ;

  // The enum values
  flags_scope.attr("NoLC") = UpgradeLCFlagsPy(UpgradeLCFlags::NoLC);
  flags_scope.attr("OnboardLC") = UpgradeLCFlagsPy(UpgradeLCFlags::OnboardLC);
  flags_scope.attr("SingleModuleLC") = UpgradeLCFlagsPy(UpgradeLCFlags::SingleModuleLC);
  flags_scope.attr("MultiModuleLC") = UpgradeLCFlagsPy(UpgradeLCFlags::MultiModuleLC);
  flags_scope.attr("DEggInclusiveLC") = UpgradeLCFlagsPy(UpgradeLCFlags::DEggInclusiveLC);
  flags_scope.attr("DEggExclusiveLC") = UpgradeLCFlagsPy(UpgradeLCFlags::DEggExclusiveLC);
  flags_scope.attr("NonUniformLC") = UpgradeLCFlagsPy(UpgradeLCFlags::NonUniformLC);

  // Register the converters
  UpgradeLCFlags_from_python().from_python<UpgradeLCFlags>();
  to_python_converter<UpgradeLCFlags,UpgradeLCFlags_to_python>();

}
