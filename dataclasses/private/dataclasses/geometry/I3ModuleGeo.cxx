//
//   Copyright (c) 2012   the IceCube collaboration
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
