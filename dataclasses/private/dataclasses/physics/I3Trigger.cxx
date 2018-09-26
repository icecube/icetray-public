/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file dataclasses/private/dataclasses/physics/I3Trigger.cxx
 * @version $Revision$
 * @date $Date$
 */
#include <icetray/serialization.h>
#include <dataclasses/physics/I3Trigger.h>
#include <icetray/I3Units.h>
#include <dataclasses/external/CompareFloatingPoint.h>

const double I3Trigger::FROM_TENTH_NS_TO_NS_CORRECTION_FACTOR = 0.1;


I3Trigger::~I3Trigger() {}


template <class Archive> void I3Trigger::serialize(Archive& ar, unsigned version)  
{
  if (version>i3trigger_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3Trigger class.",version,i3trigger_version_);
 
  ar & make_nvp("TriggerTime", time_);
  ar & make_nvp("TriggerLength", length_);
  if(version < 1) // this corrects for the fact that trigger time and length was given
  {               // in the past using units of 0.1 ns
    time_ *= FROM_TENTH_NS_TO_NS_CORRECTION_FACTOR;
    length_ *= FROM_TENTH_NS_TO_NS_CORRECTION_FACTOR;
  }
  ar & make_nvp("Fired", fired_);
  ar & make_nvp("TriggerKey", key_);
}

using CompareFloatingPoint::Compare;

bool 
I3Trigger::operator==(const I3Trigger& rhs) const
{
  return  fired_ == rhs.fired_ &&
    Compare(time_, rhs.time_) &&
    Compare(length_, rhs.length_) &&
    key_ == rhs.key_;
    ;
}

bool 
I3Trigger::operator!=(const I3Trigger& rhs) const
{
  return !( *this == rhs );
}

std::ostream& I3Trigger::Print(std::ostream& oss) const{
  oss << GetTriggerKey().GetSourceString() << " "
    << GetTriggerKey().GetTypeString() << " "
    << GetTriggerKey().GetSubtype() << " ";
  if(GetTriggerKey().CheckConfigID())
    oss << GetTriggerKey().GetConfigID() << " ";
  else
    oss<<"configID not set ";
  oss << (GetTriggerFired() ? "FIRED":"NOT_FIRED")
    << " length = " << GetTriggerLength()/I3Units::microsecond << " mus"
    << " time = " << GetTriggerTime()/I3Units::microsecond << " mus";
  return oss;
}

std::ostream& operator<<(std::ostream& oss, const I3Trigger& t){
  return(t.Print(oss));
}

I3_SERIALIZABLE(I3Trigger);
I3_SERIALIZABLE(I3VectorI3Trigger);
