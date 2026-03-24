/**
 * class: MultiPMTCoincify
 *
 * Version $Id: $
 *
 * Copyright (c) 2022 IceCube Collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 * @file MultiPMTCoincify.cxx
 * @date $Date: $
 * @author mlarson
 **/

#include <bitset>
#include <phys-services/MultiPMTCoincify.h>
#include <icetray/I3Units.h>

MultiPMTCoincify::MultiPMTCoincify(const I3Context& ctx)
  : I3ConditionalModule(ctx),
    mDOMLaunchMapName_("I3mDOMLaunchSeriesMap"),
    DEggLaunchMapName_("I3DEggLaunchSeriesMap"),
    xDOMHitMapName_("I3DEggLaunchSeriesMap"),
    moduleSpan_(2),
    moduleTime_(250 * I3Units::ns),
    pmtTime_(10 * I3Units::ns),
    lcTypes_(),
    reset_(true)
{
  AddParameter("mDOMLaunchMapName",
               "The name of the I3mDOMLaunchSeriesMap to apply coincidence conditions to.",
               mDOMLaunchMapName_);
  AddParameter("DEggLaunchMapName",
               "The name of the I3DEggLaunchSeriesMap to apply coincidence conditions to.",
               DEggLaunchMapName_);
  AddParameter("XDOMHitMapName",
               "The name of the I3XDOMHitSeriesMap to apply coincidence conditions to.",
               xDOMHitMapName_);
  AddParameter("Span",
               "The number of modules up and down a string to check for coincidences.",
               moduleSpan_);
  AddParameter("TimeWindowBetweenModules",
               "How long of a time window between two separate modules before"
               " they are no longer in coincidence.",
               moduleTime_);
  AddParameter("TimeWindowBetweenPMTs",
               "How long of a time window between two PMTs on the same module"
               " before they are no longer in coincidence.",
               pmtTime_);

  lcTypes_ = (UpgradeLCFlags::SingleModuleLC | UpgradeLCFlags::MultiModuleLC
              | UpgradeLCFlags::DEggInclusiveLC | UpgradeLCFlags::DEggExclusiveLC);
  AddParameter("LCType",
               "Which of the various types of I3XDOMLaunch::UpgradeLCFlags to process with"
               " these settings. Options are SingleModuleLC, MultiModuleLC, DEggInclusiveLC,"
               " and DEggExclusiveLC. Multiple flags can be used at once, but note that all"
               " specified LC type will use the same window and span settings. Existing flags"
               " matching these types will be reset before processing.",
               lcTypes_);
  AddParameter("ResetFlags",
               "If set to True, the module will reset any existing flags matching those specified"
               " in LCType. For example, if LCType = UpgradeLCFlags.MultiModuleLC, then the module"
               " will set all launches' MultiModuleLC bits to 0 before beginning.", reset_);
  AddOutBox("OutBox");
}


MultiPMTCoincify::~MultiPMTCoincify()
{
}

void MultiPMTCoincify::Configure()
{
  log_debug("Configuring the MultiPMTCoincify");

  GetParameter("mDOMLaunchMapName", mDOMLaunchMapName_);
  GetParameter("DEggLaunchMapName", DEggLaunchMapName_);
  GetParameter("XDOMHitMapName", xDOMHitMapName_);
  GetParameter("Span", moduleSpan_);
  GetParameter("TimeWindowBetweenModules", moduleTime_);
  GetParameter("TimeWindowBetweenPMTs", pmtTime_);
  GetParameter("LCType", lcTypes_);
  GetParameter("ResetFlags", reset_);

  // Check whether there's an unsupported flag.
  UpgradeLCFlags supported = (UpgradeLCFlags::SingleModuleLC | UpgradeLCFlags::MultiModuleLC
                              | UpgradeLCFlags::DEggInclusiveLC | UpgradeLCFlags::DEggExclusiveLC
                              | UpgradeLCFlags::NonUniformLC);
  UpgradeLCFlags selectedUnsupported = (lcTypes_ & ~supported);
  if(bool(selectedUnsupported)){
    log_fatal_stream("Unsupported LC type requested. MultiPMTCoincify handles"
                     " SingleModuleLC (" << UpgradeLCFlags::SingleModuleLC << ")"
                     ", MultiModuleLC (" << UpgradeLCFlags::MultiModuleLC << ")"
                     ", DEggInclusiveLC (" << UpgradeLCFlags::DEggInclusiveLC << ")"
                     ", DEggExclusiveLC (" << UpgradeLCFlags::DEggExclusiveLC << ")"
                     ", and NonUniformLC (" << UpgradeLCFlags::NonUniformLC << ")"
                     " but you have requested LC types " << lcTypes_ << ".");
  }
}

void MultiPMTCoincify::Coincify(UpgradeReadoutSeriesMap& readoutMap){
  log_debug("Entering MultiPMTCoincify::Coincify()");

  //---------------------------
  // Run the coincidence processing
  // The plan here is to try to cleverly use four pieces of information:
  //  (1) I3Map is a map, so we know we read out in order of OMKeys
  //  (2) We can compare OMKeys to set the bound for a second loop
  //  (3) Readouts are time ordered now, so we can use std::lower_bound for LC calculations
  //  (4) LC conditions are symmetric, so we only need to look forward, not back.
  //---------------------------
  for(auto firstPmtIter=readoutMap.begin(); firstPmtIter!=readoutMap.end(); ++firstPmtIter){
    OMKey firstOMKey = firstPmtIter->first;
    UpgradeReadoutSeries& firstReadouts = firstPmtIter->second;

    // Keep track of when we've gone too far...
    const OMKey tooFar = OMKey(firstOMKey.GetString(), firstOMKey.GetOM()+moduleSpan_+1, 0);
    for(auto secondPmtIter=std::next(firstPmtIter);
	(secondPmtIter->first<tooFar) && (secondPmtIter!=readoutMap.end());
	++secondPmtIter){
      OMKey secondOMKey = secondPmtIter->first;
      UpgradeReadoutSeries& secondReadouts = secondPmtIter->second;

      // Choose the time window to use for checks
      bool sameModule = ((firstOMKey.GetString() == secondOMKey.GetString())
                         && (firstOMKey.GetOM() == secondOMKey.GetOM()));
      double window = sameModule ? pmtTime_ : moduleTime_;

      // If we're not doing same-module LC here, we can move on.
      if(sameModule && !bool(lcTypes_ & UpgradeLCFlags::SingleModuleLC)){
        continue;
      }

      //---------------------------
      // Start going through the launches, using std::lower_bound to
      // find the first hits worth checking.
      //---------------------------
      for(auto& firstHit : firstReadouts){
        auto secondHitIter = std::lower_bound(secondReadouts.begin(), secondReadouts.end(), firstHit,
                                              [](const UpgradeReadout& A, const UpgradeReadout& B){return A.GetTime() < B.GetTime();});
        for(; secondHitIter != secondReadouts.end(); ++secondHitIter){
          double dt = secondHitIter->GetTime() - firstHit.GetTime();
          if(dt > window){
            break;
          }
          if(std::abs(dt) <= window){
            if(bool(lcTypes_ & UpgradeLCFlags::SingleModuleLC) && sameModule){
              firstHit.AddLCFlags(UpgradeLCFlags::SingleModuleLC);
              secondHitIter->AddLCFlags(UpgradeLCFlags::SingleModuleLC);
            }
            if (bool(lcTypes_ & UpgradeLCFlags::MultiModuleLC) && !sameModule){
              firstHit.AddLCFlags(UpgradeLCFlags::MultiModuleLC);
              secondHitIter->AddLCFlags(UpgradeLCFlags::MultiModuleLC);
            }
            if(bool(lcTypes_ & UpgradeLCFlags::DEggInclusiveLC) &&
               ((firstHit.type == I3OMGeo::DEgg) || secondHitIter->type == I3OMGeo::DEgg)){
              firstHit.AddLCFlags(UpgradeLCFlags::DEggInclusiveLC);
              secondHitIter->AddLCFlags(UpgradeLCFlags::DEggInclusiveLC);
            }
            if(bool(lcTypes_ & UpgradeLCFlags::DEggExclusiveLC) &&
               ((firstHit.type == I3OMGeo::DEgg) && secondHitIter->type == I3OMGeo::DEgg)){
              firstHit.AddLCFlags(UpgradeLCFlags::DEggExclusiveLC);
              secondHitIter->AddLCFlags(UpgradeLCFlags::DEggExclusiveLC);
            }

            // The non-uniform LC is a catch-all, so just accept it here without other conditions.
            // We're going to explicitly assume that the user knows what they're doing with this one.
            if(bool(lcTypes_ & UpgradeLCFlags::NonUniformLC)){
              firstHit.AddLCFlags(UpgradeLCFlags::NonUniformLC);
              secondHitIter->AddLCFlags(UpgradeLCFlags::NonUniformLC);
            }
          }
        }
      }
    }
  }
}

void MultiPMTCoincify::DAQ(I3FramePtr frame){
  // Need a copy of the geometry to figure out the type of the xDOMHit objects
  I3GeometryConstPtr geo = frame->Get<I3GeometryConstPtr>();
  if(!geo){
    log_fatal("No geometry found in frame!");
  }
  const I3OMGeoMap& omgeoMap = geo->omgeo;

  //---------------------------
  // Copy into a non-const std::variant map
  //---------------------------
  UpgradeReadoutSeriesMap readoutMap;

  //---------------------------
  // Read the pulses from the frame
  //---------------------------
  I3mDOMLaunchSeriesMapConstPtr mdomConstMap = frame->Get<I3mDOMLaunchSeriesMapConstPtr>(mDOMLaunchMapName_);
  if(mdomConstMap){
    readoutMap = UpgradeReadout::Merge(readoutMap, UpgradeReadout::From<I3mDOMLaunchSeriesMap>(*mdomConstMap, omgeoMap));
  }

  I3DEggLaunchSeriesMapConstPtr deggConstMap = frame->Get<I3DEggLaunchSeriesMapConstPtr>(DEggLaunchMapName_);
  if(deggConstMap){
    readoutMap = UpgradeReadout::Merge(readoutMap, UpgradeReadout::From<I3DEggLaunchSeriesMap>(*deggConstMap, omgeoMap));
  }

  I3XDOMHitSeriesMapConstPtr xdomConstMap = frame->Get<I3XDOMHitSeriesMapConstPtr>(xDOMHitMapName_);
  if(xdomConstMap){
    readoutMap = UpgradeReadout::Merge(readoutMap, UpgradeReadout::From<I3XDOMHitSeriesMap>(*xdomConstMap, omgeoMap));
  }

  //---------------------------
  // Run the processing
  //---------------------------
  Coincify(readoutMap);

  //---------------------------
  // Write the newly coincified pulses back out.
  //---------------------------
  I3mDOMLaunchSeriesMap updated_mdomMap = UpgradeReadout::To<I3mDOMLaunchSeriesMap>(readoutMap);
  I3DEggLaunchSeriesMap updated_deggMap = UpgradeReadout::To<I3DEggLaunchSeriesMap>(readoutMap);
  I3XDOMHitSeriesMap    updated_xdomMap = UpgradeReadout::To<I3XDOMHitSeriesMap>(readoutMap);

  if(mdomConstMap){
    frame->Delete(mDOMLaunchMapName_);
    frame->Put(mDOMLaunchMapName_, I3mDOMLaunchSeriesMapPtr(new I3mDOMLaunchSeriesMap(updated_mdomMap)));
  }
  if(deggConstMap){
    frame->Delete(DEggLaunchMapName_);
    frame->Put(DEggLaunchMapName_, I3DEggLaunchSeriesMapPtr(new I3DEggLaunchSeriesMap(updated_deggMap)));
  }
  if(xdomConstMap){
    frame->Delete(xDOMHitMapName_);
    frame->Put(xDOMHitMapName_, I3XDOMHitSeriesMapPtr(new I3XDOMHitSeriesMap(updated_xdomMap)));
  }
  PushFrame(frame);
}

void MultiPMTCoincify::Finish()
{
   log_debug("Entering MultiPMTCoincify::Finish()");
}

I3_MODULE(MultiPMTCoincify);
