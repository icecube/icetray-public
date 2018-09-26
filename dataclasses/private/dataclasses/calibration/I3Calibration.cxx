#include <map>
#include <icetray/serialization.h>
#include <dataclasses/calibration/I3Calibration.h>
#include "dataclasses/TankKey.h"

#include "deprecated/Deprecated.hpp"

I3Calibration::I3Calibration(){}

I3Calibration::~I3Calibration(){}

template <class Archive>
void 
I3Calibration::save(Archive& ar, unsigned version) const
{
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("domcal",domCal);
  ar & make_nvp("vemcal",vemCal);
  ar & make_nvp("StartTime",startTime);
  ar & make_nvp("EndTime",endTime);
}

template <class Archive>
void 
I3Calibration::load(Archive& ar, unsigned version) {
  if (version>i3calibration_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3Calibration class.",
              version,i3calibration_version_);
  
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("domcal",domCal);
  if(version < 4){
    std::map<OMKey,I3AMANDAOMCalibration> garbage;
    ar & make_nvp("amandacal", garbage);
  }
  if(version == 1){
    std::map<TankKey, I3TankCalibrationBackwardsCompat> garbage;
    ar & make_nvp("tankcal",garbage);
  }
  if(version > 1){
    ar & make_nvp("vemcal",vemCal);
  }
  if(version == 3){
    std::map<OMKey, TWRCalibration> garbage;
    ar & make_nvp("twrcal", garbage);
    double trash;
    ar & make_nvp("TWRGlobalT0", trash);
    double detritus;
    ar & make_nvp("TWRI3TimeOffset", detritus);
  }
  ar & make_nvp("StartTime",startTime);
  ar & make_nvp("EndTime",endTime);
}

I3_SPLIT_SERIALIZABLE(I3Calibration);

