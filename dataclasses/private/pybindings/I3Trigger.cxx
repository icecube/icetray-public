//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim
//
//   $Id$
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
