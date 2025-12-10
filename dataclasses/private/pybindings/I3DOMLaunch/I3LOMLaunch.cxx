/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#include <vector>

#include <dataclasses/physics/I3LOMLaunch.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/ostream_overloads.hpp>

using namespace boost::python;

// Wrapping setters with overloads
void I3LOMLaunch_SetTimeWrapper(I3LOMLaunch& launch, double time) { launch.SetTime(time); } // using default value for "valid" arg

void register_I3LOMLaunch()
{

  {
    // Wrap I3LOMLaunch class
    // Note: The setter functions return a reference to the object, rather than void which is more
    // conventional. This is why return_internal_reference is used.
    scope outer =
    class_<I3LOMLaunch, boost::shared_ptr<I3LOMLaunch> >("I3LOMLaunch")

      // If launch.time = XXX is used, it will assume valid=true. If you wish to set valid=False, use set_time(time, valid)
      .add_property("time", &I3LOMLaunch::GetTime, make_function(I3LOMLaunch_SetTimeWrapper, return_internal_reference<>()) )
      .def("set_time", make_function(&I3LOMLaunch::SetTime, return_internal_reference<>()))
      .add_property("has_valid_time", &I3LOMLaunch::HasValidTime)
      .add_property("valid_time", &I3LOMLaunch::GetValidTime) // No setter

      .def("set_adc_data", make_function(&I3LOMLaunch::SetADCData, return_internal_reference<>())) // Sets high and low gain channels simultaneously
      .add_property("adc_data_high_gain", make_function(&I3LOMLaunch::GetADCDataHighGain, return_internal_reference<>()), make_function(&I3LOMLaunch::SetADCDataHighGain, return_internal_reference<>()))
      .add_property("adc_data_low_gain", make_function(&I3LOMLaunch::GetADCDataLowGain, return_internal_reference<>()), make_function(&I3LOMLaunch::SetADCDataLowGain, return_internal_reference<>()))
      .add_property("adc_data_length", &I3LOMLaunch::GetADCDataLength)
      .add_property("adc_data_filled", &I3LOMLaunch::IsADCDataFilled)

      .def(self == self)
      .def(dataclass_suite<I3LOMLaunch>())
      ;

    }
}


void register_I3LOMLaunchSeries()
{
  class_<std::vector<I3LOMLaunch> >("I3LOMLaunchSeries")
    .def(dataclass_suite<std::vector<I3LOMLaunch> >())
    ;
}


void register_I3LOMLaunchSeriesMap()
{
  class_<I3LOMLaunchSeriesMap, bases<I3FrameObject>, I3LOMLaunchSeriesMapPtr>("I3LOMLaunchSeriesMap")
    .def(dataclass_suite<I3LOMLaunchSeriesMap>())
    ;

  register_pointer_conversions<I3LOMLaunchSeriesMap>();
}
