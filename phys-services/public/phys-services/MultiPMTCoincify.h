// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef MULTIPMT_COINCIFY_H_INCLUDED
#define MULTIPMT_COINCIFY_H_INCLUDED

#include "icetray/I3Logging.h"
#include <algorithm>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <variant>

#include "icetray/I3ConditionalModule.h"
#include "icetray/OMKey.h"
#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/geometry/I3OMGeo.h"
#include "dataclasses/physics/I3DEggLaunch.h"
#include "dataclasses/physics/I3mDOMLaunch.h"
#include "dataclasses/physics/I3XDOMHit.h"
#include "dataclasses/physics/UpgradeReadout.h"
#include "dataclasses/physics/detail/UpgradeLCFlags.h"
#include "dataclasses/physics/detail/I3XDOMLaunch.h"

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
  void Coincify(UpgradeReadoutSeriesMap& launchMap);
  void DAQ(I3FramePtr frame);
  void Finish();

  std::string mDOMLaunchMapName_;
  std::string DEggLaunchMapName_;
  std::string xDOMHitMapName_;
  unsigned int moduleSpan_;
  float moduleTime_;
  float pmtTime_;
  UpgradeLCFlags lcTypes_;
  bool reset_;

  SET_LOGGER("MultiPMTCoincify");
};

#endif
