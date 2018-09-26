/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#include "tableio/converter/I3ParticleConverter.h"

I3TableRowDescriptionPtr I3ParticleConverter::CreateDescription(const I3Particle& particle) {
    I3TableRowDescriptionPtr desc(new I3TableRowDescription());
    desc->AddField<double>("x",      "m",     "x-position of particle");
    desc->AddField<double>("y",      "m",     "y-position of particle");
    desc->AddField<double>("z",      "m",     "z-position of particle");
    desc->AddField<double>("time",   "ns",    "origin time of particle");
    desc->AddField<double>("zenith", "radian","zenith angle of particle origin");
    desc->AddField<double>("azimuth","radian","azimuthal angle of particle origin");
    desc->AddField<double>("energy", "GeV",   "energy of particle");
    desc->AddField<double>("speed",  "Gm/s",  "particle speed");
    desc->AddField<double>("length", "m",     "length of track");
    #ifdef I3PARTICLE_SUPPORTS_PDG_ENCODINGS
    desc->AddField<int32_t>("pdg_encoding", "",     "PDG encoding of particle type");
    #endif
    // skip: major_id/minor_id
    
    MAKE_ENUM_VECTOR(type,I3Particle,I3Particle::ParticleType,I3PARTICLE_H_I3Particle_ParticleType);
    MAKE_ENUM_VECTOR(shape,I3Particle,I3Particle::ParticleShape,I3PARTICLE_H_I3Particle_ParticleShape);
    MAKE_ENUM_VECTOR(location,I3Particle,I3Particle::LocationType,I3PARTICLE_H_I3Particle_LocationType);
    MAKE_ENUM_VECTOR(fit_status,I3Particle,I3Particle::FitStatus,I3PARTICLE_H_I3Particle_FitStatus);
    
    desc->AddEnumField<I3Particle::ParticleType> ("type",      type,"","");
    desc->AddEnumField<I3Particle::ParticleShape>("shape",     shape,"","");
    desc->AddEnumField<I3Particle::LocationType> ("location",  location,"","");
    desc->AddEnumField<I3Particle::FitStatus>    ("fit_status",fit_status,"","");
    
    return desc;
};

size_t I3ParticleConverter::FillRows(const I3Particle& particle, I3TableRowPtr rows) {
    
    rows->Set<double>("x",      particle.GetX());
    rows->Set<double>("y",      particle.GetY());
    rows->Set<double>("z",      particle.GetZ());
    rows->Set<double>("time",   particle.GetTime());
    rows->Set<double>("zenith", particle.GetZenith());
    rows->Set<double>("azimuth",particle.GetAzimuth());
    rows->Set<double>("energy", particle.GetEnergy());
    rows->Set<double>("speed",  particle.GetSpeed());
    rows->Set<double>("length", particle.GetLength());
    #ifdef I3PARTICLE_SUPPORTS_PDG_ENCODINGS
    rows->Set<int32_t>("pdg_encoding", particle.GetPdgEncoding());
    #endif
    
    rows->Set<I3Particle::ParticleType> ("type",      particle.GetType());
    rows->Set<I3Particle::ParticleShape>("shape",     particle.GetShape());
    rows->Set<I3Particle::LocationType> ("location",  particle.GetLocationType());
    rows->Set<I3Particle::FitStatus>    ("fit_status",particle.GetFitStatus());

    return 1;
};
