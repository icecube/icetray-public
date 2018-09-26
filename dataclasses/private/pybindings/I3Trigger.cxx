//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim
//
//   $Id$
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

#include <vector>
#include <icetray/I3Units.h>
#include <dataclasses/physics/I3Trigger.h>
#include <dataclasses/I3Vector.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/ostream_overloads.hpp>

using namespace boost::python;
namespace bp = boost::python;


TriggerKey get_trigkey(const I3Trigger& self)
{
    return self.GetTriggerKey();
}

void set_trigkey(I3Trigger& self, const TriggerKey& tk)
{
    self.GetTriggerKey() = tk;
}



void register_I3Trigger()
{

    class_<I3Trigger, I3TriggerPtr>("I3Trigger")
    PROPERTY(I3Trigger, time, TriggerTime)
    PROPERTY(I3Trigger, length, TriggerLength)
    PROPERTY(I3Trigger, fired, TriggerFired)
    // force copy of trigkey via standalone fn
    .add_property("key", get_trigkey, set_trigkey,"Get TriggerKey")
    .def( self == self )
    .def( self != self )
    .def(dataclass_suite<I3Trigger>())
    .def( freeze() )
    ;

    class_<I3VectorI3Trigger, bases<I3FrameObject>,
        boost::shared_ptr<I3VectorI3Trigger > >("I3VectorI3Trigger")
        .def(dataclass_suite<I3VectorI3Trigger >())
        ;

    register_pointer_conversions<I3VectorI3Trigger>();

}
