/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */
#include <vector>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/raw_function.hpp>

#include <dataclasses/physics/UpgradeReadout.h>
#include <dataclasses/geometry/I3OMGeo.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

//--------------------------------------------------
// Wrapper functions for the UpgradeReadout constructors
// since boost::python can't deal with std::variant
//--------------------------------------------------
boost::shared_ptr<UpgradeReadout> from_I3DEggLaunch(const I3DEggLaunch& input,
                                                    I3OMGeo::OMType omtype){                                                    
  return boost::make_shared<UpgradeReadout>(input, omtype);
}

boost::shared_ptr<UpgradeReadout> from_I3mDOMLaunch(const I3mDOMLaunch& input,
                                                    I3OMGeo::OMType omtype){                                                    
  return boost::make_shared<UpgradeReadout>(input, omtype);
}

boost::shared_ptr<UpgradeReadout> from_I3XDOMHit(const I3XDOMHit& input,
                                                 I3OMGeo::OMType omtype){
  return boost::make_shared<UpgradeReadout>(input, omtype);
}

//--------------------------------------------------
// Wrapper functions for the UpgradeReadoutSeriesMap
//-------------------------------------------------
using UpgradeReadoutSeriesMapPtr = boost::shared_ptr<UpgradeReadoutSeriesMap>;
UpgradeReadoutSeriesMapPtr from_I3DEggLaunchMap(const I3DEggLaunchSeriesMap& input,
                                                const I3OMGeoMap& omgeo){
  return boost::make_shared<UpgradeReadoutSeriesMap>(UpgradeReadout::From<I3DEggLaunchSeriesMap>(input, omgeo));
}

UpgradeReadoutSeriesMapPtr from_I3mDOMLaunchMap(const I3mDOMLaunchSeriesMap& input,
                                                const I3OMGeoMap& omgeo){
  return boost::make_shared<UpgradeReadoutSeriesMap>(UpgradeReadout::From<I3mDOMLaunchSeriesMap>(input, omgeo));
}

UpgradeReadoutSeriesMapPtr from_I3XDOMHitMap(const I3XDOMHitSeriesMap& input,
                                             const I3OMGeoMap& omgeo){   
  return boost::make_shared<UpgradeReadoutSeriesMap>(UpgradeReadout::From<I3XDOMHitSeriesMap>(input, omgeo));
}

//--------------------------------------------------
// Now put it all together for the bindings
//--------------------------------------------------
void register_UpgradeReadout()
{
  class_<UpgradeReadout, boost::shared_ptr<UpgradeReadout> >("UpgradeReadout", no_init)
    .def("__init__", make_constructor(&from_I3DEggLaunch))
    .def("__init__", make_constructor(&from_I3mDOMLaunch))
    .def("__init__", make_constructor(&from_I3XDOMHit))
    .add_property("time",     &UpgradeReadout::GetTime)
    .add_property("lc_flags", &UpgradeReadout::GetLCFlags)
    .def("is_I3DEggLaunch",  &UpgradeReadout::Contains<I3DEggLaunch>)
    .def("is_I3mDOMLaunch",  &UpgradeReadout::Contains<I3mDOMLaunch>)
    .def("is_I3XDOMHit",     &UpgradeReadout::Contains<I3XDOMHit>)
    .def("get_I3DEggLaunch", &UpgradeReadout::Get<I3DEggLaunch>, return_internal_reference<>())
    .def("get_I3mDOMLaunch", &UpgradeReadout::Get<I3mDOMLaunch>, return_internal_reference<>())
    .def("get_I3XDOMHit",    &UpgradeReadout::Get<I3XDOMHit>, return_internal_reference<>())
    ;
  
  class_<UpgradeReadoutSeries>("UpgradeReadoutSeries")
    .def(vector_indexing_suite<UpgradeReadoutSeries>())
    ;
  
  class_<UpgradeReadoutSeriesMap, boost::shared_ptr<UpgradeReadoutSeriesMap>>("UpgradeReadoutSeriesMap")
    .def(init<>())
    .def("__init__", make_constructor(&from_I3DEggLaunchMap))
    .def("__init__", make_constructor(&from_I3mDOMLaunchMap))
    .def("__init__", make_constructor(&from_I3XDOMHitMap))
    .def("to_I3DEggLaunchSeriesMap", &UpgradeReadout::To<I3DEggLaunchSeriesMap>)
    .def("to_I3mDOMLaunchSeriesMap", &UpgradeReadout::To<I3mDOMLaunchSeriesMap>)
    .def("to_I3XDOMHitSeriesMap",    &UpgradeReadout::To<I3XDOMHitSeriesMap>)
    .def("merge", &UpgradeReadout::MergeInto)
    .def(map_indexing_suite<UpgradeReadoutSeriesMap>())
    ;
}
