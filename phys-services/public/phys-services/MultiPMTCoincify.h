// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef MULTIPMT_COINCIFY_H_INCLUDED
#define MULTIPMT_COINCIFY_H_INCLUDED

#include <string>
#include <map>
#include <vector>

#include "icetray/I3ConditionalModule.h"
#include "icetray/OMKey.h"
#include "dataclasses/ModuleKey.h"
#include "phys-services/I3RandomService.h"
#include <dataclasses/physics/I3RecoPulse.h>
#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/geometry/I3OMGeo.h"
#include "dataclasses/geometry/I3ModuleGeo.h"

/// \class: MultiPMTCoincify
///
/// \author Michael Larson <mlarson@icecube.wisc.edu>
///
/// \brief Code which produces a software local coincidence for Upgrade
///        and Gen2 mult-PMT modules.
///
class MultiPMTCoincify : public I3ConditionalModule{
public:
  MultiPMTCoincify(const I3Context& context);
  MultiPMTCoincify(unsigned int moduleSpan, double moduleTime, double pmtTime, bool reset);
  ~MultiPMTCoincify();
  void Configure();
  I3RecoPulseSeriesMapPtr Coincify(I3RecoPulseSeriesMapConstPtr inputMap);
  void DAQ(I3FramePtr frame);
  void Finish();

  std::string pulsesName_;
  unsigned int moduleSpan_;
  float moduleTime_;
  float pmtTime_;
  bool reset_;

  SET_LOGGER("MultiPMTCoincify");
};

#endif
