// SPDX-FileCopyrightText: 2025 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dataclasses/calibration/I3AntennaCalibration.h>
#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

void register_I3AntennaCal() {

  {
    bp::scope antcal = bp::class_<I3AntennaCal, boost::shared_ptr<I3AntennaCal> >("I3AntennaCal")
    .def(bp::init<>())
    .add_property("antennaType", &I3AntennaCal::GetAntennaType, &I3AntennaCal::SetAntennaType)
    .add_property("cableType", &I3AntennaCal::GetCableType, &I3AntennaCal::SetCableType)
    .add_property("daqType", &I3AntennaCal::GetDaqType, &I3AntennaCal::SetDaqType)
    .add_property("cableLength", &I3AntennaCal::GetCableLength, &I3AntennaCal::SetCableLength)
    .def(bp::dataclass_suite<I3AntennaCal>())
    ;

    bp::enum_<I3AntennaCal::AntennaType>("AntennaType")
    .value("Unknown", I3AntennaCal::AntennaType::Unknown)
    .value("SKALA2", I3AntennaCal::AntennaType::SKALA2)
    .value("SKALA4", I3AntennaCal::AntennaType::SKALA4)
    ;

    bp::enum_<I3AntennaCal::CableType>("CableType")
    .value("Unknown", I3AntennaCal::CableType::Unknown)
    .value("LMR400", I3AntennaCal::CableType::LMR400)
    ;

    bp::enum_<I3AntennaCal::DaqType>("DaqType")
    .value("Unknown", I3AntennaCal::DaqType::Unknown)
    .value("Board1_Taxi3_0", I3AntennaCal::DaqType::Board1_Taxi3_0)
    .value("Board2_Taxi3_2", I3AntennaCal::DaqType::Board2_Taxi3_2)
    ;
  }

  bp::class_<I3AntennaCalMap, bp::bases<I3FrameObject>, I3AntennaCalMapPtr>("I3AntennaCalMap")
  .def(bp::dataclass_suite<I3AntennaCalMap>())
  ;

  register_pointer_conversions<I3AntennaCalMap>();
}

void register_I3AntennaCalibration()
{
  bp::class_<I3AntennaCalibration, bp::bases<I3FrameObject>, boost::shared_ptr<I3AntennaCalibration> >("I3AntennaCalibration")
    .def(bp::copy_suite<I3AntennaCalibration>())
    #define I3CALPROPS (startTime)(endTime)(antennaCalMap)
    BOOST_PP_SEQ_FOR_EACH(WRAP_RW_RECASE, I3AntennaCalibration, I3CALPROPS)
    #undef I3CALPROPS
    .def(bp::dataclass_suite<I3AntennaCalibration>())
    ;

  register_pointer_conversions<I3AntennaCalibration>();
}
