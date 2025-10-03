/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#include <vector>

#include <dataclasses/physics/I3DEggLaunch.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/ostream_overloads.hpp>

using namespace boost::python;

// Define some wrapper functions to handle "setter" functions with default arguments
// Here we will only expose a single argument, so the functions can be used in pythonic variables via "add_property"
void I3DEggLaunch_SetTimeWrapper(I3DEggLaunch& launch, double time) { launch.SetTime(time); } // using default value for "valid" arg
void I3DEggLaunch_SetBaselineValueWrapper(I3DEggLaunch& launch, double baselineValue) { launch.SetBaselineValue(baselineValue); } // using default value for "valid" arg
void I3DEggLaunch_SetADCDataWrapper(I3DEggLaunch& launch, const std::vector<int>& adcData) { launch.SetADCData(adcData); } // using default value for "lazy" arg

void register_I3DEggLaunch()
{

    // Manually specifying desired function version for functions with overloads
    const std::vector<int>& (I3DEggLaunch::*get_adc_data)(void) const = &I3DEggLaunch::GetADCData;

    // Wrap I3DEggLaunch class
    // Note: The setter functions return a reference to the object, rather than void which is more 
    // conventional. This is why return_internal_reference is used.
    scope outer = 
    class_<I3DEggLaunch, boost::shared_ptr<I3DEggLaunch> >("I3DEggLaunch")

        // If launch.time = XXX is used, it will assume valid=true. If you wish to set valid=False, use set_time(time, valid)
        .add_property("time", &I3DEggLaunch::GetTime, make_function(I3DEggLaunch_SetTimeWrapper, return_internal_reference<>()) )
        .def("set_time", make_function(&I3DEggLaunch::SetTime, return_internal_reference<>()))
        .add_property("has_valid_time", &I3DEggLaunch::HasValidTime)
        .add_property("valid_time", &I3DEggLaunch::GetValidTime) // No setter
       
        // If launch.baseline_value = XXX is used, it will assume valid=true. If you wish to set valid=False, use set_baseline_value(baseline_value, valid)
        .add_property("baseline_value", &I3DEggLaunch::GetBaselineValue, make_function(I3DEggLaunch_SetBaselineValueWrapper, return_internal_reference<>()) )
        .def("set_baseline_value", make_function(&I3DEggLaunch::SetBaselineValue, return_internal_reference<>()))
        .add_property("has_valid_baseline_value", &I3DEggLaunch::HasValidBaselineValue)
        .add_property("valid_baseline_value", &I3DEggLaunch::GetValidBaselineValue) // No setter

        .add_property("lc_bit", &I3DEggLaunch::GetLCBit, make_function(&I3DEggLaunch::SetLCBit, return_internal_reference<>()))
        .add_property("n_presamples", &I3DEggLaunch::GetNPreSamples, make_function(&I3DEggLaunch::SetNPreSamples, return_internal_reference<>()))

        .add_property("adc_data", make_function(get_adc_data, return_internal_reference<>()), make_function(I3DEggLaunch_SetADCDataWrapper, return_internal_reference<>()) )

        .def(self == self)
        .def(dataclass_suite<I3DEggLaunch>())
    ;

}

void register_I3DEggLaunchSeries()
{
    class_<std::vector<I3DEggLaunch> >("I3DEggLaunchSeries").def(dataclass_suite<std::vector<I3DEggLaunch> >());
}

void register_I3DEggLaunchSeriesMap()
{
    class_<I3DEggLaunchSeriesMap, bases<I3FrameObject>, I3DEggLaunchSeriesMapPtr>("I3DEggLaunchSeriesMap").def(dataclass_suite<I3DEggLaunchSeriesMap>());

    register_pointer_conversions<I3DEggLaunchSeriesMap>();
}
