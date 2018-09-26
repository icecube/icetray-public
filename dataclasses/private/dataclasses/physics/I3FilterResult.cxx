/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3FilterResult.cxx
 * @version $Revision$
 * @date $Date$
 */

// class header file

#include <icetray/serialization.h>
#include <dataclasses/physics/I3FilterResult.h>

// definitions


// namespace declarations


// implementation

I3FilterResult::~I3FilterResult()
{
}


template <class Archive>
void I3FilterResult::serialize(Archive& ar, unsigned version)
{
  ar & make_nvp("I3FrameObject",base_object<I3FrameObject>(*this));
  ar & make_nvp("conditionPassed",conditionPassed);
  ar & make_nvp("prescalePassed",prescalePassed);
}

std::ostream& I3FilterResult::Print(std::ostream& os) const
{
  os << "Condition: " << (conditionPassed ? "Passed" : "Failed")
  << ", Prescale: " << (prescalePassed ? "Passed" : "Failed");
  return os;
}

bool I3FilterResult::operator==(const I3FilterResult& rhs) const {
  return (this->conditionPassed == rhs.conditionPassed &&
             this->prescalePassed == rhs.prescalePassed);
}

std::ostream& operator<<(std::ostream& os, const I3FilterResult& fr)
{
  return(fr.Print(os));
}

I3_SERIALIZABLE(I3FilterResult);

I3_SERIALIZABLE(I3FilterResultMap);
