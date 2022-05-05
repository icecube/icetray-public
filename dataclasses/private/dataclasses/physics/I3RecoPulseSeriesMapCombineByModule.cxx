/**
 *  $Id$
 *
 *  Copyright (C) 2018
 *  Jakob van Santen <jakob.van.santen@desy.de>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 */

#include "dataclasses/physics/I3RecoPulseSeriesMapCombineByModule.h"
#include "dataclasses/physics/I3RecoPulse.h"
#include "dataclasses/calibration/I3Calibration.h"
#include "boost/make_shared.hpp"
#include "boost/foreach.hpp"

I3RecoPulseSeriesMapCombineByModule::I3RecoPulseSeriesMapCombineByModule() 
  : pulses_key_() {}

I3RecoPulseSeriesMapCombineByModule::I3RecoPulseSeriesMapCombineByModule(
  const std::string &pulses_key) 
  : pulses_key_(pulses_key) {}

I3RecoPulseSeriesMapConstPtr
I3RecoPulseSeriesMapCombineByModule::Apply(const I3Frame &frame) const
{
  typedef I3RecoPulseSeriesMap Map;
  typedef boost::shared_ptr<const Map> MapConstPtr;
  typedef Map::value_type Pair;
  typedef Pair::second_type Series;
  typedef Series::value_type Element;
 
  if (combined_)
    return combined_;

  MapConstPtr in_pulses = frame.Get<MapConstPtr>(pulses_key_);
  if (!in_pulses)
    log_fatal("Couldn't find '%s' in the frame!",
        pulses_key_.c_str());

  I3RecoPulseSeriesMapPtr combined = boost::make_shared<Map>();
  
  for (const auto &pair : *in_pulses) {
    auto &target = (*combined)[OMKey(pair.first.GetString(), pair.first.GetOM(), 0)];
    std::copy(pair.second.begin(), pair.second.end(), std::back_inserter(target));
  }
  for (auto &pair : *combined) {
    std::sort(pair.second.begin(), pair.second.end(),
      [](const I3RecoPulse &left, const I3RecoPulse &right)
        { return left.GetTime() < right.GetTime(); });
  }
  
  // save in cache
  combined_ = combined;

  return combined;
}

bool
I3RecoPulseSeriesMapCombineByModule::operator==(
  const I3RecoPulseSeriesMapCombineByModule& other) const
{
  return (pulses_key_ == other.pulses_key_);
}

bool
I3RecoPulseSeriesMapCombineByModule::operator!=(
  const I3RecoPulseSeriesMapCombineByModule& other) const
{
  return (pulses_key_ != other.pulses_key_);
}

std::ostream& I3RecoPulseSeriesMapCombineByModule::Print(std::ostream& os) const
{
  os << "Pulses: " << pulses_key_;
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3RecoPulseSeriesMapCombineByModule& corr)
{
  return(corr.Print(os));
}

template <class Archive>
void
I3RecoPulseSeriesMapCombineByModule::serialize(Archive& ar, unsigned version)
{
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("PulsesKey", pulses_key_);
}

I3_SERIALIZABLE(I3RecoPulseSeriesMapCombineByModule);
