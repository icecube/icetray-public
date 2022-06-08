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
 * 
 * Please note: using "LOM" here to refer to LOM-16. The plan
 * is to eventually be able to merge LOM + LOM18 into a single
 * enum for long term use
 */
#define I3MODULEGEO_H_I3ModuleGeo_ModuleType        \
  (UnknownType)(AMANDA)(IceCube)(IceTop)(mDOM)(Scintillator)(IceAct)\
  (PDOM)(DEgg)(WOM)(FOM)(DMIce)(LOM)(LOM16)(LOM18)(RadioReceiver)	\
  (POCAM)(PencilBeam)(RadioEmitter)(AcousticEmitter)(AbaloneHub)(FibreComm)

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
    enum ModuleType {UnknownType = 0, AMANDA = 10, IceCube = 20, IceTop = 30, 
		     mDOM = 40, Scintillator = 45, // These do NOT match the enums in I3OMGeo! 
		     IceAct = 50,
		     // OMType > 100 are Gen2 R&D optical modules
		     PDOM = 110, DEgg = 120, WOM = 140, FOM = 150,
		     DMIce = 160, LOM = 170, LOM16 = 171, LOM18 = 172,
		     RadioReceiver = 180,
		     // OMType > 200 for the various calibration devices
		     POCAM = 200, PencilBeam = 210, 
		     RadioEmitter = 220, 
		     AcousticEmitter = 230, 
		     AbaloneHub = 240,
		     FibreComm = 250,
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
