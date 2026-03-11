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
#include "dataclasses/physics/detail/UpgradeLCFlags.h"
#include "dataclasses/physics/detail/I3XDOMLaunch.h"

/// \class: MultiPMTCoincify
///
/// \author Michael Larson <mlarson@icecube.wisc.edu>
///
/// \brief Code which produces a software local coincidence for Upgrade
///        and Gen2 mult-PMT modules.
///
using UpgradeLaunch = std::variant<I3DEggLaunch, I3mDOMLaunch, I3XDOMHit>;

//---------------------------
// The I3DEggLaunch and I3mDOMLaunch have similar interfaces
// and both contain an I3XDOMLaunch, but they don't share any
// inheritance. To work around this, we need to set up a generic
// interface that can be used for both.
//---------------------------
struct Launch{
  Launch(I3OMGeo::OMType type_, UpgradeLaunch launch_){
    type = type_;
    launch = launch_;
  }
  
  double GetTime() const{
    return std::visit([](auto const& x){return x.GetTime();}, launch);
  }

  void ResetLCFlags(UpgradeLCFlags flags){
    SetLCFlags(GetLCFlags() & ~flags);
  }
  
  UpgradeLCFlags GetLCFlags(){
    return std::visit([](auto const& x){return x.GetLCFlags();}, launch);
  }
  
  void SetLCFlags(UpgradeLCFlags flags){
    std::visit([&](auto& x){x.SetLCFlags(flags);}, launch);
  }

  void AddLCFlags(UpgradeLCFlags flags){
    std::visit([&](auto& x){x.AddLCFlags(flags);}, launch);
  }

  bool operator<(const Launch& rhs) const{
    return GetTime() < rhs.GetTime();
  }
  
  I3OMGeo::OMType type;
  UpgradeLaunch launch;
};
typedef std::vector<Launch> LaunchSeries;
typedef std::map<OMKey, LaunchSeries> LaunchSeriesMap;

//---------------------------
// And then we can finally do the standard I3ConditionalModule
//---------------------------
class MultiPMTCoincify : public I3ConditionalModule{
public:
  MultiPMTCoincify(const I3Context& context);
  MultiPMTCoincify(unsigned int moduleSpan, double moduleTime, double pmtTime, bool reset);
  ~MultiPMTCoincify();
  void Configure();
  void Coincify(LaunchSeriesMap& launchMap);
  void GetMaps(LaunchSeriesMap& launchMap,
               I3mDOMLaunchSeriesMap& mdomMap,
               I3DEggLaunchSeriesMap& deggMap,
               I3XDOMHitSeriesMap& xdomMap);
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
