#include <icetray/serialization.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include "deprecated/Deprecated.hpp"

I3DetectorStatus::~I3DetectorStatus() {}

template <class Archive>
void
I3DetectorStatus::save(Archive& ar, unsigned version) const
{
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("DOMStatus", domStatus);
  ar & make_nvp("TriggerStatus", triggerStatus);
  ar & make_nvp("StartTime", startTime);
  ar & make_nvp("EndTime", endTime);
  ar & make_nvp("DaqConfigurationName", daqConfigurationName);
}

template <class Archive>
void
I3DetectorStatus::load(Archive& ar, unsigned version)
{
  if (version>i3detectorstatus_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3DetectorStatus class.",
              version,i3detectorstatus_version_);

  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("DOMStatus", domStatus);
  if((version > 0) && (version < 4)){
    std::map<OMKey, TWRAOMStatus> garbage;
    ar & make_nvp("AOMStatus", garbage);
  }
  ar & make_nvp("TriggerStatus", triggerStatus);
  if((version > 1) && (version < 4)){
    I3TriggerStatusMap garbage;
    ar & make_nvp("AmandaTriggerStatus", garbage);
  }
  ar & make_nvp("StartTime", startTime);
  ar & make_nvp("EndTime", endTime);
  if(version > 2)
  {
    ar & make_nvp("DaqConfigurationName", daqConfigurationName);
  }
}

I3_SPLIT_SERIALIZABLE(I3DetectorStatus);
