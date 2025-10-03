/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#include <vector>

#include <dataclasses/physics/I3NoiseSuppressionRecord.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/ostream_overloads.hpp>

using namespace boost::python;

void register_I3NoiseSuppressionRecord()
{
    // Wrap I3NoiseSuppressionRecord class
    // Note: The setter functions return a reference to the object, rather than void which is more 
    // conventional. This is why return_internal_reference is used.
    scope outer = 
    class_<I3NoiseSuppressionRecord, boost::shared_ptr<I3NoiseSuppressionRecord> >("I3NoiseSuppressionRecord")
        .add_property("time", &I3NoiseSuppressionRecord::GetTime, make_function(&I3NoiseSuppressionRecord::SetTime, return_internal_reference<>()))
        .add_property("n_noise_hits", &I3NoiseSuppressionRecord::GetNNoiseHits, make_function(&I3NoiseSuppressionRecord::SetNNoiseHits, return_internal_reference<>()))
        .add_property("exclusion_window_size", &I3NoiseSuppressionRecord::GetExclusionWindowSize, make_function(&I3NoiseSuppressionRecord::SetExclusionWindowSize, return_internal_reference<>()))
        .add_property("lc_window_size", &I3NoiseSuppressionRecord::GetLCWindowSize, make_function(&I3NoiseSuppressionRecord::SetLCWindowSize, return_internal_reference<>()))
        .def(self == self)
        .def(dataclass_suite<I3NoiseSuppressionRecord>())
    ;

}

void register_I3NoiseSuppressionRecordSeries()
{
    class_<std::vector<I3NoiseSuppressionRecord> >("I3NoiseSuppressionRecordSeries").def(dataclass_suite<std::vector<I3NoiseSuppressionRecord> >());
}

void register_I3NoiseSuppressionRecordSeriesMap()
{
    class_<I3NoiseSuppressionRecordSeriesMap, bases<I3FrameObject>, I3NoiseSuppressionRecordSeriesMapPtr>("I3NoiseSuppressionRecordSeriesMap").def(dataclass_suite<I3NoiseSuppressionRecordSeriesMap>());

    register_pointer_conversions<I3NoiseSuppressionRecordSeriesMap>();
}
