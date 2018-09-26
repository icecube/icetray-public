#include "icetray/serialization.h"
#include "icetray/I3FrameObject.h"
#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/physics/I3RecoPulse.h"
#include "dataclasses/geometry/I3Geometry.h"

#include "phys-services/I3CascadeCutValues.h"
#include "phys-services/I3Cuts.h"

void I3CascadeCutValues::Calculate(const I3Particle& vertex, 
			    const I3Geometry& geometry, 
			    const I3RecoPulseSeriesMap& pulsemap,
			    const double& begTWindow,
			    const double& endTWindow)
{
  I3Cuts::CascadeCutsCalc(vertex, geometry, pulsemap, begTWindow, endTWindow,
		   Nchan, Nhit, N_1hit, Nstring, Ndir, Nearly, Nlate);
  cog = I3Cuts::COG(geometry, pulsemap);
}

I3CascadeCutValues::~I3CascadeCutValues() { }

static const unsigned int current_i3cascadecutvalues_version=1;

template <class Archive>
void I3CascadeCutValues::serialize(Archive& ar, unsigned version)
{
  if (version>current_i3cascadecutvalues_version)
    log_fatal("Attempting to read version %u from file but running version %u of I3CascadeCutValues class.",
              version,current_i3cascadecutvalues_version);
  if (version<1)
    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  else
    ar & make_nvp("I3CutValuesBase", base_object<I3CutValuesBase>(*this));
  ar & make_nvp("Nchan",Nchan);
  ar & make_nvp("Nhit",Nhit);
  ar & make_nvp("N_1hit",N_1hit);
  ar & make_nvp("Nstring",Nstring);
  ar & make_nvp("Ndir",Ndir);
  ar & make_nvp("Nearly",Nearly);
  ar & make_nvp("Nlate",Nlate);
  ar & make_nvp("cog",cog);
}
  
I3_CLASS_VERSION(I3CascadeCutValues, current_i3cascadecutvalues_version);
I3_SERIALIZABLE(I3CascadeCutValues);

std::ostream& I3CascadeCutValues::Print(std::ostream& os) const{
  os << "[I3CascadeCutValues NChan : " << Nchan << '\n'
     << "                     NHit : " << Nhit << '\n'
     << "                    N1Hit : " << N_1hit << '\n'
     << "                  NString : " << Nstring << '\n'
     << "                     NDir : " << Ndir << '\n'
     << "                   NEarly : " << Nearly << '\n'
     << "                    NLate : " << Nlate << '\n'
     << "                      COG : " << cog << ']';
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3CascadeCutValues& cv){
  return(cv.Print(os));
}
