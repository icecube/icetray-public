#ifndef I3MCTREE_PHYSICS_LIBRARY_WRAPPERS_H
#define I3MCTREE_PHYSICS_LIBRARY_WRAPPERS_H
//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id: I3MCTree.cxx 126481 2014-12-02 22:45:21Z david.schultz $
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
