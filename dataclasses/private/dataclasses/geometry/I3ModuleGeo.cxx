//
//   Copyright (c) 2012   the IceCube collaboration
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

#include <icetray/serialization.h>
#include <dataclasses/geometry/I3ModuleGeo.h>

I3ModuleGeo::I3ModuleGeo()
:
moduleType_(UnknownType),
radius_(NAN)
{
}

I3ModuleGeo::~I3ModuleGeo()
{
}

bool I3ModuleGeo::operator==(const I3ModuleGeo& rhs) const
{
    if (rhs.moduleType_ != moduleType_) return false;
    if (rhs.radius_ != radius_) return false;
    if (!(rhs.pos_ == pos_)) return false;
    if (!(rhs.orientation_ == orientation_)) return false;
    return true;
}


template <class Archive>
void 
I3ModuleGeo::serialize(Archive& ar, unsigned version)
{
    if (version>i3modulegeo_version_)
        log_fatal("Attempting to read version %u from file but running version %u of I3ModuleGeo class.", version, i3modulegeo_version_);
    
    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    
    ar & make_nvp("moduleType", moduleType_);
    ar & make_nvp("pos", pos_);
    ar & make_nvp("orientation", orientation_);
    ar & make_nvp("radius", radius_);
}

I3_SERIALIZABLE(I3ModuleGeo);
I3_SERIALIZABLE(I3ModuleGeoMap);

std::ostream& I3ModuleGeo::Print(std::ostream& os) const{
    os << "[I3ModuleGeo ModuleType: " << moduleType_ << '\n'
       << "               Position: " << pos_ << '\n'
       << "            Orientation: " << orientation_ << '\n'
       << "                 Radius: " << radius_ << " ]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const I3ModuleGeo& g){
    return(g.Print(os));
}
