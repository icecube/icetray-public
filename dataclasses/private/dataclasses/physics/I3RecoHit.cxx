#include <icetray/serialization.h>
#include <dataclasses/physics/I3RecoHit.h>
#include <dataclasses/external/CompareFloatingPoint.h>

I3RecoHit::~I3RecoHit() {}

template <class Archive>
void 
I3RecoHit::serialize(Archive& ar, unsigned version){
  if (version>i3recohit_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3RecoHit class.",version,i3recohit_version_);

  ar & make_nvp("time",time_);
  ar & make_nvp("hitID",hitID_);
  if(version > 0)
    ar & make_nvp("sourceIndex",sourceIndex_);
}

std::ostream& I3RecoHit::Print(std::ostream& os) const{
  os << "Time: " << time_ << " ID: " << hitID_ << " SourceIndex: " << sourceIndex_;
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3RecoHit& h){
  return(h.Print(os));
}

bool
I3RecoHit::operator==(const I3RecoHit& rhs) const{
  return CompareFloatingPoint::Compare(time_ , rhs.time_ )
      && hitID_ == rhs.hitID_
      && sourceIndex_ == rhs.sourceIndex_;
}

I3_SERIALIZABLE(I3RecoHit);

I3_SERIALIZABLE(I3RecoHitSeriesMap);
