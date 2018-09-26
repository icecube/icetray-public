#include <icetray/serialization.h>
#include <dataclasses/physics/I3MCHit.h>
#include <dataclasses/physics/I3Particle.h>
I3MCHit::~I3MCHit() { }

void I3MCHit::SetParticleID(const I3Particle& p) { 
  particleID_ = p.GetMinorID(); 
  particleMajorID_ = p.GetMajorID();
}


template <class Archive>
void I3MCHit::save (Archive &ar, const unsigned version) const
{
  //always write the latest version
  ar & make_nvp("time",time_);
  ar & make_nvp("hitID",hitID_);
  ar & make_nvp("NPE", npe_);
  ar & make_nvp("Charge", charge_);
  ar & make_nvp("ParticleID", particleID_);
  ar & make_nvp("ParticleMajorID", particleMajorID_);
  ar & make_nvp("CherenkovDistance", cherenkovDistance_);
  ar & make_nvp("HitSource", source_);
}

template <class Archive>
void I3MCHit::load (Archive &ar, const unsigned version)
{
  if (version>i3mchit_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3MCHit class.",version,i3mchit_version_);
  
  ar & make_nvp("time",time_);
  ar & make_nvp("hitID",hitID_);
  if(version>2){
    ar & make_nvp("NPE", npe_);
    ar & make_nvp("Charge", charge_);    
  }else{
    double weight;
    ar & make_nvp("Weight", weight);
    npe_ = static_cast<uint64_t>(weight);
  }
  ar & make_nvp("ParticleID", particleID_);
  if(version>0)
    ar & make_nvp("ParticleMajorID", particleMajorID_);
  ar & make_nvp("CherenkovDistance", cherenkovDistance_);
  if(version>1)
    ar & make_nvp("HitSource", source_);

}

std::ostream& I3MCHit::Print(std::ostream& oss) const{
  oss << "[I3MCHit:" << std::endl
      << "  Time          :" << GetTime() << '\n'
      << "  HitID         :" << GetHitID() << '\n'
      << "  NPE           :" << GetNPE() << '\n'
      << "  Charge        :" << GetCharge() << '\n'
      << "  ParticleID    :" << GetParticleMajorID() << "/" << GetParticleMinorID() << '\n'
      << "  CherenkovDist :" << GetCherenkovDistance() << '\n'
      << "  HitSource     :" << GetHitSource()<<" ]";
  return oss;
}

std::ostream& operator<<(std::ostream& oss, const I3MCHit& h){
  return(h.Print(oss));
}

I3_SPLIT_SERIALIZABLE(I3MCHit);
I3_SERIALIZABLE(I3MCHitSeriesMap);
