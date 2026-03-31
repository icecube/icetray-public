/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#include <vector>

#include <dataclasses/physics/I3mDOMLaunch.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/ostream_overloads.hpp>

using namespace boost::python;

// Define some wrapper functions to handle "setter" functions with default arguments
// Here we will only expose a single argument, so the functions can be used in pythonic variables via "add_property"
void I3mDOMLaunch_SetTimeWrapper(I3mDOMLaunch& launch, double time) { launch.SetTime(time); } // using default value for "valid" arg
void I3mDOMLaunch_SetBaselineValueWrapper(I3mDOMLaunch& launch, double baselineValue) { launch.SetBaselineValue(baselineValue); } // using default value for "valid" arg
void I3mDOMLaunch_SetADCDataWrapper(I3mDOMLaunch& launch, const std::vector<int>& adcData) { launch.SetADCData(adcData); } // using default value for "lazy" arg

void register_I3mDOMLaunch()
{

    // Manually specifying desired function version for functions with overloads
    const std::vector<int>& (I3mDOMLaunch::*get_adc_data)(void) const = &I3mDOMLaunch::GetADCData;
    const std::vector<I3mDOMLaunch::TOTHit>& (I3mDOMLaunch::*get_tot_hits)(void) const = &I3mDOMLaunch::GetTOTHits;

    // Wrap I3mDOMLaunch class
    // Note: The setter functions return a reference to the object, rather than void which is more 
    // conventional. This is why return_internal_reference is used.
    scope outer = 
    class_<I3mDOMLaunch, boost::shared_ptr<I3mDOMLaunch> >("I3mDOMLaunch")

        // If launch.time = XXX is used, it will assume valid=true. If you wish to set valid=False, use set_time(time, valid)
        .add_property("time", &I3mDOMLaunch::GetTime, make_function(I3mDOMLaunch_SetTimeWrapper, return_internal_reference<>()) )
        .def("set_time", make_function(&I3mDOMLaunch::SetTime, return_internal_reference<>()))
        .add_property("has_valid_time", &I3mDOMLaunch::HasValidTime)
        .add_property("valid_time", &I3mDOMLaunch::GetValidTime) // No setter
       
        // If launch.baseline_value = XXX is used, it will assume valid=true. If you wish to set valid=False, use set_baseline_value(baseline_value, valid)
        .add_property("baseline_value", &I3mDOMLaunch::GetBaselineValue, make_function(I3mDOMLaunch_SetBaselineValueWrapper, return_internal_reference<>()) )
        .def("set_baseline_value", make_function(&I3mDOMLaunch::SetBaselineValue, return_internal_reference<>()))
        .add_property("has_valid_baseline_value", &I3mDOMLaunch::HasValidBaselineValue)
        .add_property("valid_baseline_value", &I3mDOMLaunch::GetValidBaselineValue) // No setter

        .add_property("lc_flags", &I3mDOMLaunch::GetLCFlags, make_function(&I3mDOMLaunch::SetLCFlags, return_internal_reference<>()))
        .add_property("n_presamples", &I3mDOMLaunch::GetNPreSamples, make_function(&I3mDOMLaunch::SetNPreSamples, return_internal_reference<>()))

        .add_property("trigger_type", &I3mDOMLaunch::GetTriggerType, make_function(&I3mDOMLaunch::SetTriggerType, return_internal_reference<>()))

        .add_property("adc_data", make_function(get_adc_data, return_internal_reference<>()), make_function(I3mDOMLaunch_SetADCDataWrapper, return_internal_reference<>()) )
        .def_readonly("adc_sample_times", &I3mDOMLaunch::GetADCSampleTimes)

        .add_property("tot_hits", make_function(get_tot_hits, return_internal_reference<>())) // Currently no setter here, instead user should edit the returned object

        .def(self == self)
        .def(dataclass_suite<I3mDOMLaunch>())
    ;

    // Wrap TOTHit class
    class_<I3mDOMLaunch::TOTHit, boost::shared_ptr<I3mDOMLaunch::TOTHit> >("TOTHit")
        .add_property("time", &I3mDOMLaunch::TOTHit::GetTime, make_function(&I3mDOMLaunch::TOTHit::SetTime, return_internal_reference<>()))
        .add_property("tot", &I3mDOMLaunch::TOTHit::GetTOT, make_function(&I3mDOMLaunch::TOTHit::SetTOT, return_internal_reference<>()))
        .def(self == self)
        .def(dataclass_suite<I3mDOMLaunch::TOTHit>())
    ;

    class_<std::vector<I3mDOMLaunch::TOTHit> >("TOTHitSeries").def(dataclass_suite<std::vector<I3mDOMLaunch::TOTHit> >());

    // Wrap TriggerType enum
    enum_<I3mDOMLaunch::TriggerType>("TriggerType")
        .value("UNDEFINED",I3mDOMLaunch::TriggerType::UNDEFINED)
        .value("SOFTWARE",I3mDOMLaunch::TriggerType::SOFTWARE)
        .value("ADC_THRESHOLD",I3mDOMLaunch::TriggerType::ADC_THRESHOLD)
        .value("DISCRIMINATOR",I3mDOMLaunch::TriggerType::DISCRIMINATOR)
        .value("EXTERNAL",I3mDOMLaunch::TriggerType::EXTERNAL)
        .export_values()
    ;
    def("identity", identity_<I3mDOMLaunch::TriggerType>);

}

void register_I3mDOMLaunchSeries()
{
    class_<std::vector<I3mDOMLaunch> >("I3mDOMLaunchSeries").def(dataclass_suite<std::vector<I3mDOMLaunch> >());
}

void register_I3mDOMLaunchSeriesMap()
{
    class_<I3mDOMLaunchSeriesMap, bases<I3FrameObject>, I3mDOMLaunchSeriesMapPtr>("I3mDOMLaunchSeriesMap").def(dataclass_suite<I3mDOMLaunchSeriesMap>());

    register_pointer_conversions<I3mDOMLaunchSeriesMap>();
}
