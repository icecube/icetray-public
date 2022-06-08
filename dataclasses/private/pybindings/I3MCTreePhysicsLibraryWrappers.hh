#ifndef I3MCTREE_PHYSICS_LIBRARY_WRAPPERS_H
#define I3MCTREE_PHYSICS_LIBRARY_WRAPPERS_H
//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id: I3MCTree.cxx 126481 2014-12-02 22:45:21Z david.schultz $
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

#include <dataclasses/physics/I3MCTreePhysicsLibrary.hh>

I3ParticlePtr get_most_energetic_primary(const I3MCTree& t){
  I3MCTree::optional_value p = I3MCTreePhysicsLibrary::GetMostEnergeticPrimary(t);
  if(p) return I3ParticlePtr(new I3Particle(p.get()));
  else return I3ParticlePtr();
}

I3ParticlePtr get_most_energetic_inice(const I3MCTree& t){
  I3MCTree::optional_value p = I3MCTreePhysicsLibrary::GetMostEnergeticInIce(t);
  if(p) return I3ParticlePtr(new I3Particle(p.get()));
  else return I3ParticlePtr();
}

I3ParticlePtr get_most_energetic(const I3MCTree& t, I3Particle::ParticleType pt){
  I3MCTree::optional_value p = I3MCTreePhysicsLibrary::GetMostEnergetic(t,pt);
  if(p) return I3ParticlePtr(new I3Particle(p.get()));
  else return I3ParticlePtr();
}
 
I3ParticlePtr get_most_energetic_track(const I3MCTree& t){
  I3MCTree::optional_value p = I3MCTreePhysicsLibrary::GetMostEnergeticTrack(t);
  if(p) return I3ParticlePtr(new I3Particle(p.get()));
  else return I3ParticlePtr();
}

I3ParticlePtr get_most_energetic_cascade(const I3MCTree& t){
  I3MCTree::optional_value p = I3MCTreePhysicsLibrary::GetMostEnergeticCascade(t);
  if(p) return I3ParticlePtr(new I3Particle(p.get()));
  else return I3ParticlePtr();
}

I3ParticlePtr get_most_energetic_inice_cascade(const I3MCTree& t){
  I3MCTree::optional_value p = I3MCTreePhysicsLibrary::GetMostEnergeticInIceCascade(t);
  if(p) return I3ParticlePtr(new I3Particle(p.get()));
  else return I3ParticlePtr();
}

I3ParticlePtr get_most_energetic_neutrino(const I3MCTree& t){
  I3MCTree::optional_value p = I3MCTreePhysicsLibrary::GetMostEnergeticNeutrino(t);
  if(p) return I3ParticlePtr(new I3Particle(p.get()));
  else return I3ParticlePtr();
}

I3ParticlePtr get_most_energetic_muon(const I3MCTree& t){
  I3MCTree::optional_value p = I3MCTreePhysicsLibrary::GetMostEnergeticMuon(t);
  if(p) return I3ParticlePtr(new I3Particle(p.get()));
  else return I3ParticlePtr();
}

I3ParticlePtr get_most_energetic_nucleus(const I3MCTree& t){
  I3MCTree::optional_value p = I3MCTreePhysicsLibrary::GetMostEnergeticNucleus(t);
  if(p) return I3ParticlePtr(new I3Particle(p.get()));
  else return I3ParticlePtr();
}


#endif
