/**
 *
 * @version $Id$
 * @file I3IceTopSLCCalibration.cxx
 * @date $Date$
 */


#include <icetray/serialization.h>
#include "dataclasses/calibration/I3IceTopSLCCalibration.h"

// ---------- THE THINGS (CALIB CONSTANTS) THAT GO IN THE MAP ------------
I3IceTopSLCCalibration::~I3IceTopSLCCalibration() {}

template <class Archive>
void I3IceTopSLCCalibration::serialize(Archive& ar, unsigned version)
{
    ar & make_nvp("intercept_C0A0",      intercept_C0A0);
    ar & make_nvp("intercept_C0A1",      intercept_C0A1);
    ar & make_nvp("intercept_C0A2",      intercept_C0A2);
    ar & make_nvp("intercept_C1A0",      intercept_C1A0);
    ar & make_nvp("intercept_C1A1",      intercept_C1A1);
    ar & make_nvp("intercept_C1A1",      intercept_C1A2);
    ar & make_nvp("slope_C0A0",      slope_C0A0);
    ar & make_nvp("slope_C0A1",      slope_C0A1);
    ar & make_nvp("slope_C0A2",      slope_C0A2);
    ar & make_nvp("slope_C1A0",      slope_C1A0);
    ar & make_nvp("slope_C1A1",      slope_C1A1);
    ar & make_nvp("slope_C1A1",      slope_C1A2);
    ar & make_nvp("intercept_CunkA0",      intercept_CunkA0);
    ar & make_nvp("intercept_CunkA1",      intercept_CunkA1);
    ar & make_nvp("intercept_CunkA2",      intercept_CunkA2);
    ar & make_nvp("slope_CunkA0",      slope_CunkA0);
    ar & make_nvp("slope_CunkA1",      slope_CunkA1);
    ar & make_nvp("slope_CunkA2",      slope_CunkA2);
    ar & make_nvp("A0_A1_crossover",      A0_A1_crossover);
    ar & make_nvp("A1_A2_crossover",      A1_A2_crossover);
    
}

std::ostream& operator<<(std::ostream& oss, const I3IceTopSLCCalibration& sc)
{
    oss << "[ I3IceTopSLCCalibration :: " << std::endl
    << "        intercept/slope Chip0 ATWD0  : " << sc.intercept_C0A0 <<" " << sc.slope_C0A0 << std::endl
    << "        intercept/slope Chip0 ATWD1  : " << sc.intercept_C0A1 <<" " << sc.slope_C0A1 << std::endl
    << "        intercept/slope Chip0 ATWD2  : " << sc.intercept_C0A2 <<" " << sc.slope_C0A2 << std::endl
    << "        intercept/slope Chip1 ATWD0  : " << sc.intercept_C1A0 <<" " << sc.slope_C1A0 << std::endl
    << "        intercept/slope Chip1 ATWD1  : " << sc.intercept_C1A1 <<" " << sc.slope_C1A1 << std::endl
    << "        intercept/slope Chip1 ATWD2  : " << sc.intercept_C1A2 <<" " << sc.slope_C1A2 << std::endl
    << "        intercept/slope UnknownChip ATWD0  : " << sc.intercept_CunkA0 <<" " << sc.slope_CunkA0 << std::endl
    << "        intercept/slope UnknownChip ATWD1  : " << sc.intercept_CunkA1 <<" " << sc.slope_CunkA1 << std::endl
    << "        intercept/slope UnknownChip ATWD2  : " << sc.intercept_CunkA2 <<" " << sc.slope_CunkA2 << std::endl
    << "        ATWD0->1 crossover  : " << sc.A0_A1_crossover << std::endl
    << "        ATWD1->2 crossover  : " << sc.A1_A2_crossover << std::endl
    << "]" ;
    return oss;
}

// ---------- THE FRAME OBJECT "CONTAINER" ------------
I3IceTopSLCCalibrationCollection::I3IceTopSLCCalibrationCollection(){}
I3IceTopSLCCalibrationCollection::~I3IceTopSLCCalibrationCollection() {}

template <class Archive>
void I3IceTopSLCCalibrationCollection::save(Archive& ar, unsigned version) const
{
    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    ar & make_nvp("itslccal", ITslcCal);
    ar & make_nvp("StartTime", startTime);
    ar & make_nvp("EndTime", endTime);
}

template <class Archive>
void I3IceTopSLCCalibrationCollection::load(Archive& ar, unsigned version)
{
    if (version>i3itslccalibration_version_){
        log_fatal("Attempting to read version %u from file but running version %u of I3IceTopSLCCalibrationCollection class.",version,i3itslccalibration_version_);
    }
    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    ar & make_nvp("itslccal", ITslcCal);
    ar & make_nvp("StartTime", startTime);
    ar & make_nvp("EndTime", endTime);
}


I3_SERIALIZABLE(I3IceTopSLCCalibration);
I3_SERIALIZABLE(I3IceTopSLCCalibrationMap);
I3_SPLIT_SERIALIZABLE(I3IceTopSLCCalibrationCollection);

