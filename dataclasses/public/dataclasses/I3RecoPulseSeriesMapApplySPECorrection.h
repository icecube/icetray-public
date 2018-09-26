/**
 *  $Id$
 *
 *  Copyright (C) 2016
 *  Claudio Kopper <ckopper@icecube.wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 */

#ifndef DATACLASSES_I3RECOPULSESERIESMAPAPPLYSPECORRECTION_H_INCLUDED
#define DATACLASSES_I3RECOPULSESERIESMAPAPPLYSPECORRECTION_H_INCLUDED

#include <functional>
#include <string>
#include "icetray/I3FrameObject.h"
#include "icetray/OMKey.h"
#include "icetray/I3Frame.h"
#include "icetray/serialization.h"
#include "dataclasses/physics/I3RecoPulse.h"

static const unsigned i3recopulseseriesmapapplyspecorrection_version_ = 0;

class I3RecoPulseSeriesMapApplySPECorrection : public I3FrameObject {
public:
  /*
   * Use the SPE correction from I3Calibration to shift the pulse amplitudes.
   * Assumes the input pulses are not shifted yet.
   */
  I3RecoPulseSeriesMapApplySPECorrection(
    const std::string &pulses_key, 
    const std::string &calibration_key);
  I3RecoPulseSeriesMapApplySPECorrection();

  std::ostream& Print(std::ostream&) const override;
  
  I3RecoPulseSeriesMapConstPtr Apply(const I3Frame&) const;
  std::string GetPulsesSource() const { return pulses_key_; }
  std::string GetCalibrationSource() const { return calibration_key_; }

  bool operator==(const I3RecoPulseSeriesMapApplySPECorrection&) const;
  bool operator!=(const I3RecoPulseSeriesMapApplySPECorrection&) const;
private:
  std::string pulses_key_;
  std::string calibration_key_;
  mutable I3RecoPulseSeriesMapPtr shifted_;

  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive& ar, unsigned version);

  SET_LOGGER("I3RecoPulseSeriesMapApplySPECorrection");
};

std::ostream& operator<<(std::ostream&, const I3RecoPulseSeriesMapApplySPECorrection&);

I3_CLASS_VERSION(I3RecoPulseSeriesMapApplySPECorrection,
  i3recopulseseriesmapapplyspecorrection_version_);
I3_POINTER_TYPEDEFS(I3RecoPulseSeriesMapApplySPECorrection);

#endif // DATACLASSES_I3RECOPULSESERIESMAPAPPLYSPECORRECTION_H_INCLUDED
