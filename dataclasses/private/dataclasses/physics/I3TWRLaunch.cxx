/**
 * (c) 2005
 * the IceCube Collaboration
 * $Id$
 *
 * @file dataclasses/private/dataclasses/physics/I3TWRLaunch.cxx
 * @brief implementation of the I3TWRLaunch class
 * @version $Revision$
 * @date $Date$
 * @author tschmidt
 */
 
// class header file

#include <icetray/serialization.h>
#include <dataclasses/physics/I3TWRLaunch.h>

#include <math.h>

// namespace declarations


// implementation

I3TWRLaunch::~I3TWRLaunch()
{
}


template<class Archive> 
void 
I3TWRLaunch::serialize (Archive& ar, unsigned version)
{
  if (version>i3twrlaunch_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3TWRLaunch class.",version,i3twrlaunch_version_);

  if(version < 1)
  {
    if (typename Archive::is_saving())
      log_fatal("Impossible: we are saving at version 0");
    I3Vector<I3TWRFrag> tmp;
    ar & make_nvp("I3TWRLaunch", tmp);
    fragments_.swap(tmp);
  
    int id = 0;
    ar & make_nvp("ID", id);
    double binSize = 0.0;
    ar & make_nvp("BinSize", binSize);

    double baseline = 0.0;
    ar & make_nvp("Baseline", baseline);
    baseline = round(baseline);
    if(baseline < 0) log_fatal("invalid baseline value");
    baseline_ = static_cast<unsigned int>(baseline);

    int stopDelay = 0;
    ar & make_nvp("StopDelay", stopDelay);
    int threshold = 0;
    ar & make_nvp("Threshold", threshold);
    bool optical = 0;
    ar & make_nvp("Optical", optical);
  }
  else
  {
    ar & make_nvp("Baseline", baseline_);
    ar & make_nvp("Fragments", fragments_);
  }
}


bool operator==(const I3TWRLaunch& lhs, const I3TWRLaunch& rhs)
{
  return ( lhs.GetBaseline() ==rhs.GetBaseline() &&
	   lhs.GetFragments()==rhs.GetFragments()
	   );
}

std::ostream& I3TWRLaunch::Print(std::ostream& os) const{
  os << "[I3TWRLaunch Baseline: " << baseline_ << '\n'
     << "            Fragments: ";
  for(const auto& frag : fragments_)
    os << frag << '\n';
  os << ']';
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3TWRLaunch& l){
  return(l.Print(os));
}

I3_SERIALIZABLE(I3TWRLaunch);
I3_SERIALIZABLE(I3TWRLaunchSeriesMap);
