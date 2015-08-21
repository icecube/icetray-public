//
//   Copyright (c) 2008   Troy D. Straszheim and the IceCube Collaboration 
//   
//   This file is part of IceTray.
//
//   IceTray is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
//   IceTray is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

using namespace boost::python;
namespace bp = boost::python;

#include <interfaces/I3CalibrationService.h>
#include <dataclasses/I3Time.h>
#include <dataclasses/calibration/I3Calibration.h>

#include "install.h"

struct I3CalibrationServiceWrapper : I3CalibrationService, wrapper<I3CalibrationService>
{
  I3CalibrationConstPtr GetCalibration(I3Time t) 
  {  
    return this->get_override("GetCalibration")(t); 
  }
};

void register_I3CalibrationService()
{
  implicitly_convertible<boost::shared_ptr<I3CalibrationServiceWrapper>, boost::shared_ptr<const I3CalibrationService> >();
  implicitly_convertible<boost::shared_ptr<I3CalibrationServiceWrapper>, boost::shared_ptr<I3CalibrationService> >();
  implicitly_convertible<boost::shared_ptr<I3CalibrationServiceWrapper>, boost::shared_ptr<const I3CalibrationServiceWrapper> >();

  class_<I3CalibrationServiceWrapper, boost::shared_ptr<I3CalibrationServiceWrapper>, 
    boost::noncopyable>("I3CalibrationService", 
			"Service returns geometries depending on time", 
			init<>())
    .def("get_calibration", &I3CalibrationServiceWrapper::GetCalibration, bp::arg("time"))
    .def("install", &I3InstallService<I3CalibrationService>().func)
    ;
}

