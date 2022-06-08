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

#include <interfaces/I3GeometryService.h>
#include <dataclasses/I3Time.h>
#include <dataclasses/geometry/I3Geometry.h>

#include "install.h"

struct I3GeometryServiceWrapper : I3GeometryService, wrapper<I3GeometryService>
{
  I3GeometryConstPtr GetGeometry(I3Time t) 
  {  
    return this->get_override("GetGeometry")(t); 
  }
};

void register_I3GeometryService()
{
  implicitly_convertible<boost::shared_ptr<I3GeometryServiceWrapper>, boost::shared_ptr<const I3GeometryService> >();
  implicitly_convertible<boost::shared_ptr<I3GeometryServiceWrapper>, boost::shared_ptr<I3GeometryService> >();
  implicitly_convertible<boost::shared_ptr<I3GeometryServiceWrapper>, boost::shared_ptr<const I3GeometryServiceWrapper> >();

  class_<I3GeometryServiceWrapper, boost::shared_ptr<I3GeometryServiceWrapper>, 
    boost::noncopyable>("I3GeometryService", 
			"Service returns geometries depending on time", 
			init<>())
    .def("get_geometry", &I3GeometryServiceWrapper::GetGeometry, bp::arg("time"))
    .def("install", &I3InstallService<I3GeometryService>().func)
    ;
}

