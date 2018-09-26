/**
 * copyright  (C) 2004
 * the icecube collaboration
 * @version $Id$
 * @file I3MCHit.h
 * @date $Date$
 */

#ifndef I3MCHIT_H_INCLUDED
#define I3MCHIT_H_INCLUDED

#include "dataclasses/I3Vector.h"
#include "dataclasses/I3Map.h"
#include "dataclasses/physics/I3ParticleID.h"
#include "icetray/OMKey.h"

/**
 * @brief Derived I3Hit class with additional 'Weight' and 'Parent
 * track' info
 * 
 * This class records the true (simulated) time at which a
 * photoelectron was produced.  In addition to the time, it contains
 * the identity of the track which was responsible for the hit, and a
 * statistical weight to assign to the hit (default 1).  Also, 
 * CherenkovDistance- direct path distance to the track that generated
 * this hit.
 */
static const unsigned i3mchit_version_ = 3;

/**
 * List the names of enumeration members defined in this file
 * here. These can be used for e.g. pybindings, which require
 * the names of the enumeration members to be known. This list
 * should be updated whenver members or new enums are added to
 * the class.
 */
#define I3MCHIT_H_I3MCHit_HitSource                     \
    (UNKNOWN)(SPE)(RANDOM)(AFTER_PULSE)(PRE_PULSE)      \
    (ELASTIC_LATE_PULSE)(INELASTIC_LATE_PULSE)          \
    (EARLY_AFTER_PULSE)                                 

class I3Particle;

class I3MCHit
{

 public:

  enum HitSource{
    UNKNOWN = 0,
    SPE = 10,
    RANDOM = 20,
    AFTER_PULSE = 30,
    PRE_PULSE = 40,
    ELASTIC_LATE_PULSE = 50,
    INELASTIC_LATE_PULSE = 60,
    EARLY_AFTER_PULSE = 70
  };

  I3MCHit() : 
    time_(NAN), 
    npe_(1), 
    charge_(NAN), 
    hitID_(-1),
    particleID_(-1), 
    particleMajorID_(0), 
    cherenkovDistance_(NAN),
    source_(UNKNOWN) { }

 I3MCHit(uint64_t mid, int id) : 
    time_(NAN), 
    npe_(1),
    charge_(NAN), 
    hitID_(-1),
    particleID_(id), 
    particleMajorID_(mid), 
    cherenkovDistance_(NAN),
    source_(UNKNOWN) { }

  ~I3MCHit();
  
  std::ostream& Print(std::ostream&) const;

  double GetTime() const { return time_;}

  void SetTime(double time){time_ = time;}

  int32_t GetHitID() const { return hitID_;}

  void SetHitID(int hitID){hitID_ = hitID;}

  uint64_t GetNPE() const { return npe_; }

  void SetNPE(uint64_t npe) { npe_ = npe; }

  double GetCharge() const { return charge_; }

  void SetCharge(double charge) { charge_ = charge; }

  int32_t GetParticleMinorID() const { return particleID_; }

  uint64_t GetParticleMajorID() const { return particleMajorID_; }

  void SetParticleID(const I3Particle& );

  void SetParticleID(int32_t minorID, uint64_t majorID) { 
    particleID_ = minorID; 
    particleMajorID_ = majorID; 
  }

  /**
   * @return the direct path distance to the track which caused this hit
   */
  double GetCherenkovDistance() const { return cherenkovDistance_; }

  /**
   * @param CherenkovDistance set the direct path distance to track which 
   * caused this hit
   */
  void SetCherenkovDistance(double CherenkovDistance) { 
    cherenkovDistance_ = CherenkovDistance; 
  }

  /**
   * Sets/Gets the source of the MCHit
   */
  void SetHitSource(HitSource s){ source_= s; }
  HitSource GetHitSource() const { return source_; }

  bool operator==(const I3MCHit& rhs) const {
    return time_ == rhs.time_
      && hitID_ == rhs.hitID_
      && charge_ == rhs.charge_
      && npe_ == rhs.npe_
      && particleID_ == rhs.particleID_
      && particleMajorID_ == rhs.particleMajorID_
      && cherenkovDistance_ == rhs.cherenkovDistance_
      && source_ == rhs.source_;
  }
  
  operator I3ParticleID() const { 
    I3ParticleID id; 
    id.majorID = particleMajorID_;
    id.minorID = particleID_;
    return id; 
  }

 private:

  double time_;
  uint64_t  npe_;
  double  charge_;
  int32_t hitID_;
  int32_t    particleID_;
  uint64_t    particleMajorID_;
  double  cherenkovDistance_;
  HitSource source_;

  friend class icecube::serialization::access;

  template <class Archive> void load(Archive & ar, const unsigned version);
  template <class Archive> void save(Archive & ar, const unsigned version) const ;

  I3_SERIALIZATION_SPLIT_MEMBER()

};

I3_CLASS_VERSION(I3MCHit,i3mchit_version_);

std::ostream& operator<<(std::ostream& oss, const I3MCHit& h);

I3_POINTER_TYPEDEFS(I3MCHit);

typedef std::vector<I3MCHit> I3MCHitSeries;
typedef I3Map<OMKey, I3MCHitSeries> I3MCHitSeriesMap; 

I3_POINTER_TYPEDEFS(I3MCHitSeries);
I3_POINTER_TYPEDEFS(I3MCHitSeriesMap);

#endif //I3MCHIT_H_INCLUDED

