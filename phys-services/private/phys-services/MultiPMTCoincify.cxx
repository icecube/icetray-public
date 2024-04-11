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
#include <boost/foreach.hpp>
#include <phys-services/MultiPMTCoincify.h>
#include <icetray/I3Units.h>

MultiPMTCoincify::MultiPMTCoincify(unsigned int moduleSpan, double moduleTime, double pmtTime, bool reset=true):
  I3ConditionalModule(I3Context()),
  pulsesName_(""),
  moduleSpan_(moduleSpan),
  moduleTime_(moduleTime),
  pmtTime_(pmtTime),
  reset_(reset)
{
}

MultiPMTCoincify::MultiPMTCoincify(const I3Context& ctx)
  : I3ConditionalModule(ctx),
    pulsesName_("I3RecoPulseSeriesMapGen2"),
    moduleSpan_(2),
    moduleTime_(250 * I3Units::ns),
    pmtTime_(10 * I3Units::ns),
    reset_(true)
{
  AddParameter("PulseMapName",
               "The name of the I3RecoPulseSeriesMap to apply coincidence conditions to.",
               pulsesName_);
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
  AddParameter("ResetLC",
               "Should we force all LC flags to 0 before we run?",
               reset_);
  AddOutBox("OutBox");
}


MultiPMTCoincify::~MultiPMTCoincify()
{
}

void MultiPMTCoincify::Configure()
{
  log_debug("Configuring the MultiPMTCoincify");

  GetParameter("PulseMapName", pulsesName_);
  GetParameter("Span", moduleSpan_);
  GetParameter("TimeWindowBetweenModules", moduleTime_);
  GetParameter("TimeWindowBetweenPMTs", pmtTime_);
  GetParameter("ResetLC", reset_);
}

I3RecoPulseSeriesMapPtr MultiPMTCoincify::Coincify(I3RecoPulseSeriesMapConstPtr inputMap){
  log_debug("Entering MultiPMTCoincify::Coincify()");

  //---------------------------
  // Copy them to a non-const container
  // sorting them to guarantee time-ordering.
  //---------------------------
  auto pulseComp = [](I3RecoPulse a, I3RecoPulse b){return a.GetTime() < b.GetTime();};
  I3RecoPulseSeriesMapPtr outputMap(new I3RecoPulseSeriesMap());
  BOOST_FOREACH(auto pair, *inputMap){
    OMKey omk = pair.first;
    I3RecoPulseSeries pulses = pair.second;
    std::sort(pulses.begin(), pulses.end(), pulseComp);

    if(reset_){
      BOOST_FOREACH(auto& pulse, pulses){
	pulse.SetFlags(pulse.GetFlags() & !I3RecoPulse::PulseFlags::LC);
      }
    }

    outputMap->insert(std::make_pair(omk, pulses));
  }

  //---------------------------
  // Run the coincidence processing
  // The plan here is to try to cleverly use four pieces of information:
  //  (1) I3Map is a map, so we know we read out in order of OMKeys
  //  (2) We can compare OMKeys to set the bound for a second loop
  //  (3) Pulses are time ordered now, so we can use std::lower_bound for LC calculations
  //  (4) LC conditions are symmetric, so we only need to look forward, not back.
  //---------------------------
  for(auto firstPmtIter=outputMap->begin(); firstPmtIter!=outputMap->end(); ++firstPmtIter){
    OMKey firstOMKey = firstPmtIter->first;
    I3RecoPulseSeries& firstPulses = firstPmtIter->second;

    // Keep track of when we've gone too far...
    const OMKey tooFar = OMKey(firstOMKey.GetString(), firstOMKey.GetOM()+moduleSpan_+1, 0);

    for(auto secondPmtIter=std::next(firstPmtIter);
	(secondPmtIter->first<tooFar) && (secondPmtIter!=outputMap->end());
	++secondPmtIter){
      OMKey secondOMKey = secondPmtIter->first;
      I3RecoPulseSeries& secondPulses = secondPmtIter->second;

      double window = moduleTime_;
      if ((firstOMKey.GetString()==secondOMKey.GetString()) && (firstOMKey.GetOM()==secondOMKey.GetOM()))
	window = pmtTime_;

      //---------------------------
      // Start going through the pulses, using std::lower_bound to
      // find the first pulse worth checking.
      //---------------------------
      for(auto firstPulseIter=firstPulses.begin(); firstPulseIter!=firstPulses.end(); ++firstPulseIter){
	auto secondPulseIter = std::lower_bound(secondPulses.begin(), secondPulses.end(),
						*firstPulseIter,
						[&](const I3RecoPulse& a, const I3RecoPulse& b){return a.GetTime() < b.GetTime()-window;});
	for(; secondPulseIter!=secondPulses.end(); ++secondPulseIter){
	  double dt = secondPulseIter->GetTime() - firstPulseIter->GetTime();
	  if(dt > window){
            break;
          }
	  if(std::abs(dt) <= window){
	    firstPulseIter->SetFlags(firstPulseIter->GetFlags() | I3RecoPulse::PulseFlags::LC);
	    secondPulseIter->SetFlags(secondPulseIter->GetFlags() | I3RecoPulse::PulseFlags::LC);
	  }
	}
      }
    }
  }
  return outputMap;
}

void MultiPMTCoincify::DAQ(I3FramePtr frame){
  //---------------------------
  // Read the pulses from the frame
  //---------------------------
  if(!frame->Has(pulsesName_)){
    PushFrame(frame);
  }
  I3RecoPulseSeriesMapConstPtr inputMap = frame->Get<I3RecoPulseSeriesMapConstPtr>(pulsesName_);

  //---------------------------
  // Run the processing
  //---------------------------
  I3RecoPulseSeriesMapPtr outputMap = Coincify(inputMap);

  //---------------------------
  // Write the newly coincified pulses back out.
  //---------------------------
  frame->Delete(pulsesName_);
  frame->Put(pulsesName_, outputMap);
  PushFrame(frame);
}


void MultiPMTCoincify::Finish()
{
   log_debug("Entering MultiPMTCoincify::Finish()");
}

I3_MODULE(MultiPMTCoincify);
