// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

/**
 *
 * @version $Id$
 * @file I3IceTopSLCCalibration.cxx
 * @date $Date$
 */


#include <icetray/serialization.h>
#include "dataclasses/calibration/I3IceTopSLCCalibration.h"

#include <boost/foreach.hpp>

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/lexical_cast.hpp>

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
    ar & make_nvp("StartRun", startRun);
    ar & make_nvp("EndRun", endRun);
    ar & make_nvp("Provenance", itSlcCalProvenance);
}

template <class Archive>
void I3IceTopSLCCalibrationCollection::load(Archive& ar, unsigned version)
{
    if (version>i3itslccalibration_version_){
        log_fatal("Attempting to read version %u from file but running version %u of I3IceTopSLCCalibrationCollection class.",version,i3itslccalibration_version_);
    }
    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    ar & make_nvp("itslccal", ITslcCal);
    if (version < 1) {
      I3Time garbage1;
      I3Time garbage2;
      ar & make_nvp("StartTime", garbage1);
      ar & make_nvp("EndTime", garbage2);
    }
    if (version >= 1) {
      ar & make_nvp("StartRun", startRun);
      ar & make_nvp("EndRun", endRun);
      ar & make_nvp("Provenance", itSlcCalProvenance);
    }
}


// For converting ITSLCCalProvenance enum's to strings and back again:
// I stole/adapted this from I3Geometry, which was stolen/adapted from I3Particle.
#define MAKE_ENUM_TO_STRING_CASE_LINE(r, data, t) case I3IceTopSLCCalibrationCollection::t : return BOOST_PP_STRINGIZE(t);
std::string I3IceTopSLCCalibrationCollection::GetITSLCCalProvenanceString() const
{
  switch (itSlcCalProvenance) {
    BOOST_PP_SEQ_FOR_EACH(MAKE_ENUM_TO_STRING_CASE_LINE, ~, I3ICETOPSLCCALIBRATION_H_I3IceTopSLCCalibrationCollection_ITSLCCalProvenance)
      }
  return(boost::lexical_cast<std::string>( itSlcCalProvenance ));
}

#define MAKE_STRING_TO_ENUM_IF_LINE2(r, data, t) else if ( str == BOOST_PP_STRINGIZE(t) ) { data = t; }
void I3IceTopSLCCalibrationCollection::SetITSLCCalProvenanceString(const std::string &str)
{
  if (false) { }
  BOOST_PP_SEQ_FOR_EACH(MAKE_STRING_TO_ENUM_IF_LINE2, itSlcCalProvenance, I3ICETOPSLCCALIBRATION_H_I3IceTopSLCCalibrationCollection_ITSLCCalProvenance)
  else {
    try {
      itSlcCalProvenance = static_cast<ITSLCCalProvenance>( boost::lexical_cast<int>(str) );
    } catch(boost::bad_lexical_cast &bad) {
      log_fatal("\"%s\" is not a valid ITSLCCalProvenance.", str.c_str());
    }
  }
}


I3_SERIALIZABLE(I3IceTopSLCCalibration);
I3_SERIALIZABLE(I3IceTopSLCCalibrationMap);
I3_SPLIT_SERIALIZABLE(I3IceTopSLCCalibrationCollection);

