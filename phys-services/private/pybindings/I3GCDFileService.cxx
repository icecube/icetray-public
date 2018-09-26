#include "phys-services/source/I3GCDFileService.h"

#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/calibration/I3Calibration.h>
#include <dataclasses/status/I3DetectorStatus.h>

namespace bp = boost::python;

static I3GeometryPtr
get_geometry(I3GCDFileGeometryService &service, I3Time &time)
{
  I3GeometryConstPtr geo = service.GetGeometry(time);
  return boost::const_pointer_cast<I3Geometry>(geo);
}

static I3CalibrationPtr
get_calibration(I3GCDFileCalibrationService &service, I3Time &time)
{
  I3CalibrationConstPtr calib = service.GetCalibration(time);
  return boost::const_pointer_cast<I3Calibration>(calib);
}

static I3DetectorStatusPtr
get_status(I3GCDFileDetectorStatusService &service, I3Time &time)
{
  I3DetectorStatusConstPtr status = service.GetDetectorStatus(time);
  return boost::const_pointer_cast<I3DetectorStatus>(status);
}


void
register_I3GCDFileService()
{
  bp::class_<I3GCDFileGeometryService, bp::bases<I3GeometryService>, boost::shared_ptr<I3GCDFileGeometryService>, boost::noncopyable>
    ("I3GCDFileGeometryService", bp::init<std::string>())
    .def("get_geometry", &get_geometry, bp::arg("time"))
    ;

  bp::class_<I3GCDFileCalibrationService, bp::bases<I3CalibrationService>, boost::shared_ptr<I3GCDFileCalibrationService>, boost::noncopyable>
    ("I3GCDFileCalibrationService", bp::init<std::string>())
    .def("get_calibration", &get_calibration, bp::arg("time"))
    ;

  bp::class_<I3GCDFileDetectorStatusService, bp::bases<I3DetectorStatusService>, boost::shared_ptr<I3GCDFileDetectorStatusService>, boost::noncopyable>
    ("I3GCDFileDetectorStatusService", bp::init<std::string>())
    .def("get_detector_status", &get_status, bp::arg("time"))
    ;
}
