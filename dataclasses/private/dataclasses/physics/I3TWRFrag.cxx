/**
 * (c) 2005
 * the IceCube Collaboration
 * $Id$
 *
 * @file dataclasses/private/dataclasses/physics/I3TWRFrag.cxx
 * @brief implementation of the I3TWRFrag class
 * @version $Revision$
 * @date $Date$
 * @author tschmidt
 */
 
// class header file

#include <vector>
#include <icetray/serialization.h>
#include <dataclasses/physics/I3TWRFrag.h>

#include <math.h>

// namespace declarations


// implementation

I3TWRFrag::~I3TWRFrag()
{
}


template<class Archive>
void 
I3TWRFrag::serialize(Archive& ar, unsigned version)
{
  if (version>i3twrfrag_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3TWRFrag class.",version,i3twrfrag_version_);

  if(version < 1)
  {
    // initialize these temporaries or you get warnings.  
    // Note that we'll never be saving when version < 1
    if (typename Archive::is_saving())
      log_fatal("The impossible has happened... we are saving at version 0");
    int id = 0; 
    ar & make_nvp("ID", id);
    int parentId = 0;
    ar & make_nvp("ParentID", parentId);
    double fragStartTime = 0.0;
    ar & make_nvp("FragStartTime", fragStartTime);

    int fragStartBin = 0;
    ar & make_nvp("FragStartBin", fragStartBin);
    if(fragStartBin < 0) log_fatal("invalid start bin");
    startBin_ = static_cast<unsigned int>(fragStartBin);
    
    std::vector<double> waveform;
    ar & make_nvp("Waveform", waveform);
    rawFADC_.resize(waveform.size());
    for(unsigned int i = 0; i < waveform.size(); ++i)
    {
      waveform[i] = round(waveform[i]);
      if(waveform[i] < 0) log_fatal("invalid waveform value");
      rawFADC_[i] = static_cast<unsigned int>(waveform[i]);
    }
  }
  else
  {
    ar & make_nvp("StartBin", startBin_);
    ar & make_nvp("RawFADC", rawFADC_);
  }
}

std::ostream& I3TWRFrag::Print(std::ostream& os) const{
  os << "[I3TWRFrag:\n"
     << "  Start Bin: " << startBin_ << '\n'
     << "  Raw FADC: [";
  for(auto sample : rawFADC_)
    os << sample << ", ";
  os << "] ]";
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3TWRFrag& f){
  return(f.Print(os));
}

bool operator==(const I3TWRFrag& lhs, const I3TWRFrag& rhs)
{
  return ( lhs.GetStartBin() == rhs.GetStartBin() &&
	   lhs.GetWaveform() == rhs.GetWaveform()
	   );
}

I3_SERIALIZABLE(I3TWRFrag);
