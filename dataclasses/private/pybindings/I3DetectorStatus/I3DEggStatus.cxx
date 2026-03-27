/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Pybindings for I3DEggStatus and I3DEggStatusMap Classes
 *
 * @file I3DEggStatus.cxx
 * @date 2026-03-26
 * @author lbloom12
 *
 */


#include <dataclasses/status/I3DEggStatus.h>
#include <icetray/python/dataclass_suite.hpp>


using namespace boost::python;



// define a Getter function which converts the firCoefficients C++ array into a Python list
list get_coeffs(const I3DEggStatus& self) {
  list coeffs;
  for (unsigned i = 0; i < 16; ++i) {
    coeffs.append(self.firCoefficients[i]);
  }
  return coeffs;
}



// define a Setter function which converts a Python list into a C++ array for firCoefficients
void set_coeffs(I3DEggStatus& self, const object& obj) {
  list py_list = extract<list>(obj);

  // throw an error if the provided Python list does not have 16 elements
  if (len(py_list) != 16) {
    PyErr_SetString(PyExc_ValueError, "List must have 16 elements");
    throw_error_already_set();
  }
  
  for (unsigned i = 0; i < 16; i++) {
    self.firCoefficients[i] = extract<int8_t>(py_list[i]); 
  }
}



// Create pybindings to read/write all values in I3DEggStatus
void register_I3DEggStatus()
{
  scope outer =
    class_<I3DEggStatus, boost::shared_ptr<I3DEggStatus> >("I3DEggStatus")
      #define DEGGSTATUS_VALS (enabled)(trigMode)(trigThreshold)(readoutMode) \
                              (preSamples)(postSamples)(pmtHV)(baselineDAC)
      BOOST_PP_SEQ_FOR_EACH(WRAP_RW_RECASE, I3DEggStatus, DEGGSTATUS_VALS)
      #undef DEGGSTATUS_VALS

      // expose the Getter and Setter functions for 'firCoefficients'
      .add_property("fir_coefficients", &get_coeffs, &set_coeffs)
  
      .def(dataclass_suite<I3DEggStatus>())
      ;


  // define the DEggPMTTrigMode enum
  enum_<I3DEggStatus::DEggPMTTrigMode>("DEggPMTTrigMode")
    .value("UnknownTrigMode", I3DEggStatus::UnknownTrigMode)
    .value("ADC", I3DEggStatus::ADC)
    .value("FIR", I3DEggStatus::FIR)
    .export_values()
    ;
  def("identity", identity_<I3DEggStatus::DEggPMTTrigMode>);


  // define the DEggReadoutMode enum
  enum_<I3DEggStatus::DEggReadoutMode>("DEggReadoutMode")
    .value("UnknownReadoutMode", I3DEggStatus::UnknownReadoutMode)
    .value("FIXED_LENGTH", I3DEggStatus::FIXED_LENGTH)
    .value("VARIABLE_LENGTH", I3DEggStatus::VARIABLE_LENGTH)
    .export_values()
    ;
  def("identity", identity_<I3DEggStatus::DEggReadoutMode>);
}



// Create pybindings for I3DEggStatusMap
void register_I3DEggStatusMap()
{
  class_<I3DEggStatusMap, bases<I3FrameObject> >("I3DEggStatusMap")
  .def(dataclass_suite<I3DEggStatusMap>())
  ;
}
