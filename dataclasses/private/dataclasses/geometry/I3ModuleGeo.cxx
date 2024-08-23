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
radR_(NAN),
radZ_(NAN),
h_(NAN)
{
}

I3ModuleGeo::~I3ModuleGeo()
{
}

bool I3ModuleGeo::operator==(const I3ModuleGeo& rhs) const
{
    if (rhs.moduleType_ != moduleType_) return false;
    if (rhs.radR_ != radR_) return false;
    if (rhs.radZ_ != radZ_) return false;
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
    ar & make_nvp("radiusR", radR_);
    ar & make_nvp("radiusZ", radZ_);
    ar & make_nvp("height", h_);
}

I3_SERIALIZABLE(I3ModuleGeo);
I3_SERIALIZABLE(I3ModuleGeoMap);

std::ostream& I3ModuleGeo::Print(std::ostream& os) const{
    os << "[I3ModuleGeo ModuleType: " << moduleType_ << '\n'
       << "               Position: " << pos_ << '\n'
       << "            Orientation: " << orientation_ << '\n'
       << "                RadiusR: " << radR_ << "\n"
       << "                RadiusZ: " << radZ_ << "\n"
       << "                 Height: " << h_ << " ]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const I3ModuleGeo& g){
    return(g.Print(os));
}


void I3ModuleGeo::SetModuleType(ModuleType newType)
{
	moduleType_=newType;


	switch (moduleType_) {
	case I3ModuleGeo::AMANDA:
	case I3ModuleGeo::IceCube:

		// Surface detectors
	case I3ModuleGeo::IceTop:
	case I3ModuleGeo::Scintillator:
	case I3ModuleGeo::IceAct:

		// New module types
	case I3ModuleGeo::PDOM:
		I3ModuleGeo::SetRadius(0.1651*I3Units::m); // assume 13" diameter
		break;

	case I3ModuleGeo::WOM:
		I3ModuleGeo::SetCylinder(0.057*I3Units::m, 2*0.650*I3Units::m);
		break;

	case I3ModuleGeo::FOM:
		I3ModuleGeo::SetCylinder(0.1524*I3Units::m, 2*0.6096*I3Units::m);
		break;

		// New module types
	case I3ModuleGeo::mDOM:
		I3ModuleGeo::SetSpheroid(0.178*I3Units::m, 0.2055*I3Units::m);
		break;
	case I3ModuleGeo::DEgg:
		// See presentation by R. Hmaid https://drive.google.com/file/d/1O-sYyqqjmg_LhcZU9xKBsr9VwpKBGwv1/view
		// See presentation by Y. Kobayashi https://drive.google.com/file/d/13wfTWbeIIDmZuuoEdztmWnlVT_S97bmu/view
		I3ModuleGeo::SetSpheroid(0.204*I3Units::m, 0.267*I3Units::m);
		break;
	case I3ModuleGeo::LOM:
	case I3ModuleGeo::LOM16:
		I3ModuleGeo::SetSpheroid(0.156*I3Units::m, 0.222*I3Units::m);
		break;
	case I3ModuleGeo::LOM18:
		I3ModuleGeo::SetSpheroid(0.159*I3Units::m, 0.270*I3Units::m);
		break;

	default:
		log_debug("Unknown input ModuleType number %u. Using I3ModuleGeo::ModuleType \"Unknown\" with spherical radius 0.1651",
							static_cast<unsigned int>(moduleType_));
		I3ModuleGeo::SetRadius(0.1651*I3Units::m); // assume 13" diameter
		break;
	}
}
