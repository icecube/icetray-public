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

#ifndef I3MODULEGEO_H_INCLUDED
#define I3MODULEGEO_H_INCLUDED

#include <vector>
#include <icetray/I3DefaultName.h>
#include "icetray/I3FrameObject.h"
#include "dataclasses/ModuleKey.h"
#include "dataclasses/I3Position.h"
#include "dataclasses/I3Orientation.h"
#include "dataclasses/I3Map.h"

static const unsigned i3modulegeo_version_ = 0;

/**
 * List the names of enumeration members defined in this file
 * here. These can be used for e.g. pybindings, which require
 * the names of the enumeration members to be known. This list
 * should be updated whenver members or new enums are added to
 * the class.
 */
#define I3MODULEGEO_H_I3ModuleGeo_ModuleType        \
    (UnknownType)(AMANDA)(IceCube)(IceTop)(mDOM)


/**
 * @brief Describes an optical module. This is supposed
 * to contain all information about the module itself, not
 * the PMT(s) inside of it.
 *
 * For IceCube/AMANDA DOMs, the position and orientation
 * will be the same as for the PMTs inside this module
 * (described by I3OMGeo).
 */
class I3ModuleGeo : public I3FrameObject
{
public:
    enum ModuleType {
        UnknownType = 0,
        AMANDA = 10,
        IceCube = 20,
        IceTop = 30,
        mDOM = 40
    };
    
    I3ModuleGeo();
    ~I3ModuleGeo();
    
    std::ostream& Print(std::ostream&) const override;
    
    bool operator==(const I3ModuleGeo& rhs) const;
    
    ModuleType GetModuleType() const {return moduleType_;}
    void SetModuleType(ModuleType newType) {moduleType_=newType;}
    
    const I3Position &GetPos() const {return pos_;}
    void SetPos(const I3Position& p) {pos_=p;}
    void SetPos(double x, double y, double z) {pos_=I3Position(x,y,z);}

    const I3Orientation &GetOrientation() const {return orientation_;}
    void SetOrientation(const I3Orientation& o) {orientation_=o;}
    I3Direction GetDir() const {return orientation_.GetDir();}

    double GetRadius() const {return radius_;}
    void SetRadius(double radius) {radius_=radius;}
     
private:
    ModuleType moduleType_;
    I3Position pos_;
    I3Orientation orientation_;
    double radius_;
    
    friend class icecube::serialization::access;
    template <class Archive> void serialize(Archive & ar, unsigned version);
};

std::ostream& operator<<(std::ostream&, const I3ModuleGeo&);

I3_CLASS_VERSION(I3ModuleGeo, i3modulegeo_version_);

typedef I3Map<ModuleKey, I3ModuleGeo> I3ModuleGeoMap; 

I3_POINTER_TYPEDEFS(I3ModuleGeo);
I3_POINTER_TYPEDEFS(I3ModuleGeoMap);


#endif // I3MODULEGEO_H_INCLUDED
