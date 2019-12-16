#ifndef I3NUGEN_INFO_H_INCLUDED
#define I3NUGEN_INFO_H_INCLUDED

#include "icetray/I3DefaultName.h"
#include "dataclasses/physics/I3Particle.h"

static const unsigned i3nugen_info_version_ = 0;

struct I3NuGenInfo : public I3FrameObject{

  uint32_t run_id;
  uint32_t n_events;  
  I3Particle::ParticleType primary_type;
  double flavor_ratio;  
  double cylinder_height;
  double cylinder_radius;
  double min_zenith;  
  double max_zenith;
  double min_azimuth;  
  double max_azimuth;
  double min_energy_log;
  double max_energy_log;
  double power_law_index;

  std::ostream& Print(std::ostream&) const override;

  friend class icecube::serialization::access;

  template <class Archive>
  void serialize(Archive& ar, unsigned version);
};

std::ostream& operator<<(std::ostream& oss, const I3NuGenInfo& n);

I3_POINTER_TYPEDEFS(I3NuGenInfo);
I3_DEFAULT_NAME(I3NuGenInfo);
I3_CLASS_VERSION(I3NuGenInfo, i3nugen_info_version_);

#endif //I3NUGEN_INFO_H_INCLUDED
