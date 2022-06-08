//
//   Copyright (c) 2008   Troy D. Straszheim and the IceCube Collaboration 
//   
//   This file is part of IceTray.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//   
//   THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
//   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
//   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
//   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
//   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//   SUCH DAMAGE.
//   
//   SPDX-License-Identifier: BSD-2-Clause
//   
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

