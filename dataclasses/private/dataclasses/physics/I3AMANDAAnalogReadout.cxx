/**
 * copyright  (C) 2005
 * the IceCube Collaboration
 * $Id$
 *
 * @file dataclasses/private/dataclasses/physics/I3AMANDAAnalogReadout.cxx
 * @version $Revision$
 * @date $Date$
 * @author deyoung
 */

#include <icetray/serialization.h>
#include "dataclasses/physics/I3AMANDAAnalogReadout.h"
#include <algorithm>
#include <functional>

using namespace std;

I3AMANDAAnalogReadout::~I3AMANDAAnalogReadout() {}

template <class Archive> 
void 
I3AMANDAAnalogReadout::save(Archive& ar, unsigned version) const
{
  if (version>i3amandaanalogreadout_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3AMANDAAnalogReadout class.",version,i3amandaanalogreadout_version_);

  ar & make_nvp("LEs", LEs_);
  ar & make_nvp("TEs", TEs_);
  ar & make_nvp("HitNumbers", HitNumbers_);
  ar & make_nvp("ParentIDs", ParentIDs_);
  ar & make_nvp("ADC", adc_);
  ar & make_nvp("Overflow", overflow_);
  ar & make_nvp("Calib", is_calib_);
}

template <class Archive> 
void 
I3AMANDAAnalogReadout::load(Archive& ar, unsigned version)
{
  if (version>i3amandaanalogreadout_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3AMANDAAnalogReadout class.",version,i3amandaanalogreadout_version_);

  ar & make_nvp("LEs", LEs_);
  ar & make_nvp("TEs", TEs_);
  ar & make_nvp("HitNumbers", HitNumbers_);
  ar & make_nvp("ParentIDs", ParentIDs_);
  ar & make_nvp("ADC", adc_ );
  ar & make_nvp("Overflow", overflow_ );
  ar & make_nvp("Calib", is_calib_ );
    
  processFirstLE_ = true;
  processTOTs_ = true;
}

I3_SPLIT_SERIALIZABLE(I3AMANDAAnalogReadout);
I3_SERIALIZABLE(I3AMANDAAnalogReadoutMap);

std::ostream& I3AMANDAAnalogReadout::Print(std::ostream& os) const{
  os << "[I3AMANDAAnalogReadout:\n"
     << "         LEs: " << LEs_ << '\n'
     << "         TEs: " << TEs_ << '\n'
     << "  HitNumbers: " << HitNumbers_ << '\n'
     << "  Parent IDs: " << ParentIDs_ << '\n'
     << "         ADC: " << adc_ << '\n'
     << "    Overflow: " << overflow_ << '\n'
     << "  Calibrated: " << is_calib_ << " ]";
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3AMANDAAnalogReadout& r){
  return(r.Print(os));
}

double I3AMANDAAnalogReadout::GetFirstLE() const
{
  if(LEs_.empty()) return NAN;

  if(processFirstLE_)
  {
    vector<double> tmp = LEs_;
    sort(tmp.begin(),tmp.end());
    firstLE_ = tmp[0];
    processFirstLE_ = false;
  }
  
  return firstLE_;
}

const vector<double>& I3AMANDAAnalogReadout::GetTOTs() const
{
  if(processTOTs_)
  {
    // this is not true for AMANDA data, but this is fixed in the
    // f2k reader
    if (LEs_.size() != TEs_.size())
      log_fatal("LEs.size() and TEs.size() should have same size");

    TOTs_.resize(LEs_.size());
    transform(TEs_.begin(), TEs_.end(), LEs_.begin(), TOTs_.begin(), minus<double>());
    processTOTs_ = false;
  }
                                                                                                                            
  return TOTs_;
}


