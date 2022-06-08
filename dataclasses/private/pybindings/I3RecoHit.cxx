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

#include <dataclasses/physics/I3RecoHit.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

void register_I3RecoHit()
{
  class_<I3RecoHit, boost::shared_ptr<I3RecoHit> >("I3RecoHit")
    PROPERTY(I3RecoHit, time, Time)
    PROPERTY(I3RecoHit, id, ID)
    PROPERTY(I3RecoHit, source_index, SourceIndex)
    .def(self == self)
    .def(dataclass_suite<I3RecoHit>())
    ;

  class_<std::vector<I3RecoHit> >("vector_I3RecoHit")
    .def(dataclass_suite<std::vector<I3RecoHit> >())
    ;

  class_<I3RecoHitSeriesMap, bases<I3FrameObject>, I3RecoHitSeriesMapPtr>("I3RecoHitSeriesMap")
    .def(dataclass_suite<I3RecoHitSeriesMap>())
    ;
  register_pointer_conversions<I3RecoHitSeriesMap>();
}
