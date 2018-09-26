/**
 * copyright  (C) 2013
 * the icecube collaboration
 * @file I3MCTreePhysicsLibrary.hh
 */

#ifndef I3MCTREEPHYSLIB_H_INCLUDED
#define I3MCTREEPHYSLIB_H_INCLUDED

#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/physics/I3MCTree.h"

namespace I3MCTreePhysicsLibrary
{
  /**
   * Returns the InIce particle with highest energy.
   * This is useful for example in extracting "the muon" from the
   * atmospheric neutrino data.  The names below are fairly descriptive
   * and should accurately describe what the function does.
   *
   * If safe_mode is true, return boost::none if there are two or more particles
   * with the same highest energy. This incurs a second O(N) search over the tree.
   */
  I3MCTree::optional_value GetMostEnergeticPrimary(const I3MCTree& t, bool safe_mode = true );
  I3MCTree::optional_value GetMostEnergeticPrimary(I3MCTreeConstPtr t, bool safe_mode = true );

  I3MCTree::optional_value GetMostEnergeticInIce(const I3MCTree& t, bool safe_mode = true );
  I3MCTree::optional_value GetMostEnergeticInIce(I3MCTreeConstPtr t, bool safe_mode = true );

  I3MCTree::optional_value GetMostEnergetic(const I3MCTree& t, I3Particle::ParticleType pt, bool safe_mode = true );
  I3MCTree::optional_value GetMostEnergetic(I3MCTreeConstPtr t, I3Particle::ParticleType pt, bool safe_mode = true );

  I3MCTree::optional_value GetMostEnergeticTrack(const I3MCTree& t, bool safe_mode = true );
  I3MCTree::optional_value GetMostEnergeticTrack(I3MCTreeConstPtr t, bool safe_mode = true );

  I3MCTree::optional_value GetMostEnergeticCascade(const I3MCTree& t, bool safe_mode = true );
  I3MCTree::optional_value GetMostEnergeticCascade(I3MCTreeConstPtr t, bool safe_mode = true );

  I3MCTree::optional_value GetMostEnergeticInIceCascade(const I3MCTree& t, bool safe_mode = true );
  I3MCTree::optional_value GetMostEnergeticInIceCascade(I3MCTreeConstPtr t, bool safe_mode = true );

  I3MCTree::optional_value GetMostEnergeticNeutrino(const I3MCTree& t, bool safe_mode = true );
  I3MCTree::optional_value GetMostEnergeticNeutrino(I3MCTreeConstPtr t, bool safe_mode = true );

  I3MCTree::optional_value GetMostEnergeticMuon(const I3MCTree& t, bool safe_mode = true );
  I3MCTree::optional_value GetMostEnergeticMuon(I3MCTreeConstPtr t, bool safe_mode = true );

  I3MCTree::optional_value GetMostEnergeticNucleus(const I3MCTree& t, bool safe_mode = true );
  I3MCTree::optional_value GetMostEnergeticNucleus(I3MCTreeConstPtr t, bool safe_mode = true );
}

#endif


