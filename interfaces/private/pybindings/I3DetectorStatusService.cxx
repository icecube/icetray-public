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

#include <interfaces/I3DetectorStatusService.h>
#include <dataclasses/I3Time.h>
#include <dataclasses/status/I3DetectorStatus.h>

#include "install.h"

struct I3DetectorStatusServiceWrapper : I3DetectorStatusService, wrapper<I3DetectorStatusService>
{
  I3DetectorStatusConstPtr GetDetectorStatus(I3Time t) 
  {  
    return this->get_override("GetDetectorStatus")(t); 
  }
};

void register_I3DetectorStatusService()
{
  implicitly_convertible<boost::shared_ptr<I3DetectorStatusServiceWrapper>, boost::shared_ptr<const I3DetectorStatusService> >();
  implicitly_convertible<boost::shared_ptr<I3DetectorStatusServiceWrapper>, boost::shared_ptr<I3DetectorStatusService> >();
  implicitly_convertible<boost::shared_ptr<I3DetectorStatusServiceWrapper>, boost::shared_ptr<const I3DetectorStatusServiceWrapper> >();

  class_<I3DetectorStatusServiceWrapper, boost::shared_ptr<I3DetectorStatusServiceWrapper>, 
    boost::noncopyable>("I3DetectorStatusService", 
			"Service returns geometries depending on time", 
			init<>())
    .def("get_detector_status", &I3DetectorStatusServiceWrapper::GetDetectorStatus, bp::arg("time"))
    .def("install", &I3InstallService<I3DetectorStatusService>().func)
    ;
}

