#include <string>
#include <dataclasses/external/CompareFloatingPoint.h>
#include <dataclasses/physics/I3MCTreePhysicsLibrary.hh>
#include "dataclasses/physics/I3ParticleID.h"
#include "dataclasses/physics/I3MCTreeUtils.h"

#include <boost/function.hpp>

using CompareFloatingPoint::Compare;
using I3MCTreeUtils::GetBestFilter;
using I3MCTreeUtils::GetFilter;
using I3MCTreeUtils::GetBestFilterPtr;

namespace{
  bool MoreEnergetic(const I3Particle& a, const I3Particle& b){
    float a_energy = a.GetEnergy();
    float b_energy = b.GetEnergy();
    if (std::isnan(a_energy))
        return false;
    else if (std::isnan(b_energy))
        return true;
    else
        return a_energy > b_energy;
  }

  bool IsCascade(const I3Particle& p){ return p.IsCascade(); }
  bool IsNeutrino(const I3Particle& p){ return p.IsNeutrino(); }
  bool IsNucleus(const I3Particle& p){ return p.IsNucleus(); }
  bool IsTrack(const I3Particle& p){ return p.IsTrack(); }
  bool IsInIceCascade(const I3Particle& p){
      return p.GetLocationType() == I3Particle::InIce && p.IsCascade(); }
  bool IsInIce(const I3Particle& p){return p.GetLocationType() == I3Particle::InIce; }
  bool IsMuon(const I3Particle& p){
    return p.GetType() == I3Particle::MuPlus
      || p.GetType() == I3Particle::MuMinus;
  }

  struct IsParticle{
    I3Particle::ParticleType type;
    IsParticle(const I3Particle::ParticleType t):type(t){};
    bool operator()(const I3Particle& p){ return p.GetType() == type;}
  };

  // check whether there is another distinct candidate particle
  // with the same energy as test_value, so that test_value is ambiguous
  I3MCTree::optional_value
    checked_value(const std::vector<I3Particle>& candidates,
                  boost::optional<I3Particle> test_value){
    for(std::vector<I3Particle>::const_iterator i = candidates.begin();
        i != candidates.end(); ++i){
      if( (i->GetID() != test_value->GetID())
          && (Compare(test_value->GetEnergy(), i->GetEnergy(), (int64_t) 1))){
        return boost::none;
      }
    }
    return test_value;
  }
}

I3MCTree::optional_value
I3MCTreePhysicsLibrary::GetMostEnergeticPrimary(const I3MCTree& t, bool safe_mode){
  if(t.size() == 0) 
    return I3MCTree::optional_value();

  std::vector<I3Particle> primaries = t.get_heads();
  I3MCTree::optional_value rval(primaries.front());
  BOOST_FOREACH(const I3Particle& p, primaries){
    if(p.GetEnergy() > rval->GetEnergy()){
      rval = p;
    }
  }

  if(rval && safe_mode)
    return checked_value(primaries, rval);
  return rval;
}
I3MCTree::optional_value
I3MCTreePhysicsLibrary::GetMostEnergeticPrimary(I3MCTreeConstPtr t, bool safe_mode){
  return GetMostEnergeticPrimary(*t);
}

I3MCTree::optional_value
I3MCTreePhysicsLibrary::GetMostEnergeticInIce(const I3MCTree& t, bool safe_mode){
  I3MCTree::optional_value rval = GetBestFilter(t, IsInIce, MoreEnergetic);
  if(rval && safe_mode)
    return checked_value(GetFilter(t, IsInIce), rval);
  return rval;
}
I3MCTree::optional_value
I3MCTreePhysicsLibrary::GetMostEnergeticInIce(I3MCTreeConstPtr t, bool safe_mode){
  return GetMostEnergeticInIce(*t);
}

I3MCTree::optional_value
I3MCTreePhysicsLibrary::GetMostEnergetic(const I3MCTree& t, I3Particle::ParticleType pt, bool safe_mode){
  IsParticle is_particle(pt);
  I3MCTree::optional_value rval = GetBestFilter(t, is_particle, MoreEnergetic);
  if(rval && safe_mode)
    return checked_value(GetFilter(t, is_particle), rval);
  return rval;
}
I3MCTree::optional_value
I3MCTreePhysicsLibrary::GetMostEnergetic(I3MCTreeConstPtr t, I3Particle::ParticleType pt, bool safe_mode){
  return GetMostEnergetic(*t, pt);
}

I3MCTree::optional_value
I3MCTreePhysicsLibrary::GetMostEnergeticTrack(const I3MCTree& t, bool safe_mode){
  I3MCTree::optional_value rval = GetBestFilter(t, IsTrack, MoreEnergetic);
  if(rval && safe_mode)
    return checked_value(GetFilter(t, IsTrack), rval);
  return rval;
}

I3MCTree::optional_value
I3MCTreePhysicsLibrary::GetMostEnergeticTrack(I3MCTreeConstPtr t, bool safe_mode){
  return GetMostEnergeticTrack(*t);
}

I3MCTree::optional_value
I3MCTreePhysicsLibrary::GetMostEnergeticCascade(const I3MCTree& t, bool safe_mode){
  I3MCTree::optional_value rval = GetBestFilter(t, IsCascade, MoreEnergetic);
  if(rval && safe_mode)
    return checked_value(GetFilter(t, IsCascade), rval);
  return rval;
}
I3MCTree::optional_value
I3MCTreePhysicsLibrary::GetMostEnergeticCascade(I3MCTreeConstPtr t, bool safe_mode){
  return GetMostEnergeticCascade(*t);
}

I3MCTree::optional_value
I3MCTreePhysicsLibrary::GetMostEnergeticInIceCascade(const I3MCTree& t, bool safe_mode){
  I3MCTree::optional_value rval = GetBestFilter(t, IsInIceCascade, MoreEnergetic);
  if(rval && safe_mode)
    return checked_value(GetFilter(t, IsInIceCascade), rval);
  return rval;
}

I3MCTree::optional_value
I3MCTreePhysicsLibrary::GetMostEnergeticInIceCascade(I3MCTreeConstPtr t, bool safe_mode){
  return GetMostEnergeticInIceCascade(*t);
}

I3MCTree::optional_value
I3MCTreePhysicsLibrary::GetMostEnergeticNeutrino(const I3MCTree& t, bool safe_mode){
  I3MCTree::optional_value rval = GetBestFilter(t, IsNeutrino, MoreEnergetic);
  if(rval && safe_mode)
    return checked_value(GetFilter(t, IsNeutrino), rval);
  return rval;
}

I3MCTree::optional_value
I3MCTreePhysicsLibrary::GetMostEnergeticNeutrino(I3MCTreeConstPtr t, bool safe_mode){
  return GetMostEnergeticNeutrino(*t);
}

I3MCTree::optional_value
I3MCTreePhysicsLibrary::GetMostEnergeticMuon(const I3MCTree& t, bool safe_mode){
  I3MCTree::optional_value rval = GetBestFilter(t, IsMuon, MoreEnergetic);
  if(rval && safe_mode)
    return checked_value(GetFilter(t, IsMuon), rval);
  return rval;
}
I3MCTree::optional_value
I3MCTreePhysicsLibrary::GetMostEnergeticMuon(I3MCTreeConstPtr t, bool safe_mode){
  return GetMostEnergeticMuon(*t);
}

I3MCTree::optional_value
I3MCTreePhysicsLibrary::GetMostEnergeticNucleus(const I3MCTree& t, bool safe_mode){
  I3MCTree::optional_value rval = GetBestFilter(t, IsNucleus, MoreEnergetic);
  if(rval && safe_mode)
    return checked_value(GetFilter(t, IsNucleus), rval);
  return rval;
}
I3MCTree::optional_value
I3MCTreePhysicsLibrary::GetMostEnergeticNucleus(I3MCTreeConstPtr t, bool safe_mode){
  return GetMostEnergeticNucleus(*t);
}

