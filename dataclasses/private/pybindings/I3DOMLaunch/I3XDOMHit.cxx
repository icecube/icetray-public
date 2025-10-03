/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#include <vector>

#include <dataclasses/physics/I3XDOMHit.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/ostream_overloads.hpp>

using namespace boost::python;

void register_I3XDOMHit()
{
    // Wrap I3XDOMHit class
    // Note: The setter functions return a reference to the object, rather than void which is more 
    // conventional. This is why return_internal_reference is used.
    scope outer = 
        class_<I3XDOMHit, boost::shared_ptr<I3XDOMHit> >("I3XDOMHit")
          .add_property("time", &I3XDOMHit::GetTime, make_function(&I3XDOMHit::SetTime, return_internal_reference<>()))
          .add_property("lc_bit", &I3XDOMHit::GetLCBit, make_function(&I3XDOMHit::SetLCBit, return_internal_reference<>()))
          .add_property("charge", &I3XDOMHit::GetCharge, make_function(&I3XDOMHit::SetCharge, return_internal_reference<>()))
          .def(self == self)
          .def(dataclass_suite<I3XDOMHit>())
    ;
}

void register_I3XDOMHitSeries()
{
    class_<std::vector<I3XDOMHit> >("I3XDOMHitSeries").def(dataclass_suite<std::vector<I3XDOMHit> >());
}

void register_I3XDOMHitSeriesMap()
{
    class_<I3XDOMHitSeriesMap, bases<I3FrameObject>, I3XDOMHitSeriesMapPtr>("I3XDOMHitSeriesMap").def(dataclass_suite<I3XDOMHitSeriesMap>());

    register_pointer_conversions<I3XDOMHitSeriesMap>();
}
