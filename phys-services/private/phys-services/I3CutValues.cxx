#include "icetray/serialization.h"
#include "icetray/I3FrameObject.h"
#include "phys-services/I3Cuts.h"
#include "phys-services/I3CutValues.h"
#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/physics/I3RecoPulse.h"
#include "dataclasses/geometry/I3Geometry.h"

void I3CutValues::Calculate(const I3Particle& track, 
			    const I3Geometry& geometry, 
			    const I3RecoPulseSeriesMap& pulsemap,
			    const double& begTWindow,
			    const double& endTWindow)
{
  I3Cuts::CutsCalc(track, geometry, pulsemap, begTWindow, endTWindow,
		   Nchan, Nhit, Nstring, Ndir, Ldir, Sdir, Sall);
  cog = I3Cuts::COG(geometry, pulsemap);
}

I3CutValues::~I3CutValues() { }

static const unsigned int current_i3cutvalues_version=2;

template <class Archive>
void I3CutValues::serialize(Archive& ar, unsigned version)
{
  if (version>current_i3cutvalues_version)
    log_fatal("Attempting to read version %u from file but running version %u of I3CutValues class.",
              version,current_i3cutvalues_version);
  if (version<2)
    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  else
    ar & make_nvp("I3CutValuesBase", base_object<I3CutValuesBase>(*this));
  ar & make_nvp("Nchan",Nchan);
  ar & make_nvp("Nhit",Nhit);
  if (version > 0) 
    ar & make_nvp("Nstring",Nstring);
  ar & make_nvp("Ndir",Ndir);
  ar & make_nvp("Ldir",Ldir);
  ar & make_nvp("Sdir",Sdir);
  if (version > 0) 
    ar & make_nvp("Sall",Sall);
  ar & make_nvp("cog",cog);
}
  
I3_CLASS_VERSION(I3CutValues, current_i3cutvalues_version);
I3_SERIALIZABLE(I3CutValues);

std::ostream& I3CutValues::Print(std::ostream& os) const{
  os << "[I3CutValues NChan : " << Nchan << '\n'
     << "              NHit : " << Nhit << '\n'
     << "           NString : " << Nstring << '\n'
     << "              NDir : " << Ndir << '\n'
     << "              LDir : " << Ldir << '\n'
     << "              SDir : " << Sdir << '\n'
     << "              SAll : " << Sall << '\n'
     << "               COG : " << cog << ']';
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3CutValues& cv){
  return(cv.Print(os));
}

template <class Archive>
void I3CutValuesBase::serialize(Archive& ar, unsigned version)
{
	ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
}

I3_CLASS_VERSION(I3CutValuesBase, 0);
I3_SERIALIZABLE(I3CutValuesBase);
