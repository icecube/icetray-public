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

#include <interfaces/I3EventService.h>
#include <icetray/I3Frame.h>
#include <dataclasses/I3Time.h>

#include "install.h"

struct I3EventServiceWrapper : I3EventService, wrapper<I3EventService>
{
  bool MoreEvents() {  return this->get_override("MoreEvents")(); }
  I3Time PopEvent(I3Frame& frame) { return this->get_override("PopEvent")(frame); }
};

void register_I3EventService()
{
  implicitly_convertible<boost::shared_ptr<I3EventServiceWrapper>, 
    boost::shared_ptr<const I3EventService> >();
  implicitly_convertible<boost::shared_ptr<I3EventServiceWrapper>, 
    boost::shared_ptr<I3EventService> >();
  implicitly_convertible<boost::shared_ptr<I3EventServiceWrapper>, 
    boost::shared_ptr<const I3EventServiceWrapper> >();

  class_<I3EventServiceWrapper, boost::shared_ptr<I3EventServiceWrapper>, 
    boost::noncopyable>("I3EventService", 
			"Service returns geometries depending on time", 
			init<>())
    .def("more_events", &I3EventServiceWrapper::MoreEvents)
    .def("pop_event", &I3EventServiceWrapper::PopEvent, bp::arg("frame"))
    .def("install", &I3InstallService<I3EventService>().func)

    ;
}
