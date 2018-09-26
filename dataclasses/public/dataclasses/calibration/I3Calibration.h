/**
 *
 * Definition of I3Calibration class
 *
 * copyright  (C) 2004
 * the IceCube collaboration
 * @version $Id$
 * @file I3Calibration.h
 * @date $Date$
 */

#ifndef I3CALIBRATION_H_INCLUDED
#define I3CALIBRATION_H_INCLUDED

#include <map>

#include "dataclasses/Utility.h"
#include "dataclasses/calibration/I3DOMCalibration.h"
#include "dataclasses/calibration/I3VEMCalibration.h"
#include "dataclasses/I3Time.h"
#include "icetray/OMKey.h"
#include <icetray/I3FrameObject.h>
#include <icetray/I3DefaultName.h>

static const unsigned i3calibration_version_ = 4;

class I3Calibration : public I3FrameObject
{
public:
  I3Time startTime;
  I3Time endTime;

  I3Calibration();
    
  ~I3Calibration();
    
  I3DOMCalibrationMap domCal;
  I3VEMCalibrationMap vemCal;
  
  bool operator==(const I3Calibration& rhs)
  {
    return (startTime == rhs.startTime &&
            endTime == rhs.endTime &&
            domCal == rhs.domCal &&
            vemCal == rhs.vemCal);
  }
  bool operator!=(const I3Calibration& rhs)
  {
    return !operator==(rhs);
  }
  
private:
  friend class icecube::serialization::access;
  template <class Archive> void load(Archive & ar, unsigned version);
  template <class Archive> void save(Archive & ar, unsigned version) const;
  I3_SERIALIZATION_SPLIT_MEMBER();
};

I3_CLASS_VERSION(I3Calibration, i3calibration_version_);
I3_DEFAULT_NAME(I3Calibration);
I3_POINTER_TYPEDEFS(I3Calibration);

#endif // I3CALIBRATION_H_INCLUDED
    

