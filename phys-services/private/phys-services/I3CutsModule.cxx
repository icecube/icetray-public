/**
 * $Id$
 * (c) 2005 IceCube Collaboration
 */


#include "icetray/I3TrayHeaders.h"
#include "dataclasses/physics/I3RecoPulse.h"
#include "dataclasses/geometry/I3Geometry.h"
#include "I3CutsModule.h"
#include "phys-services/I3CutValues.h"
#include "phys-services/I3CascadeCutValues.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/foreach.hpp>

using boost::algorithm::split;
using boost::algorithm::is_any_of;

using namespace std;
using namespace I3Units;

I3_MODULE(I3CutsModule);

//--------------------------------------------------------------
I3CutsModule::I3CutsModule(const I3Context& ctx) : I3ConditionalModule(ctx)
{
  AddOutBox("OutBox");

  particleName_ = "";
  AddParameter("ParticleNames", "Name of I3Particle to be used for cut calc"
               ,particleName_);

  AddParameter("ParticleNameList", "List of I3Particle names to be used for cut calc"
               ,particleNameList_);

  hitsName_ = "";
  AddParameter("HitsName", "Name of the hit series to be used for cut calc"
               ,hitsName_);

  pulsesName_ = "";
  AddParameter("PulsesName", "Name of the pulse series to be used for cut calc"
               ,pulsesName_);

  nameTag_ = "";
  AddParameter("NameTag", "Tag to add to name of output object"
               ,nameTag_);

  timeRange_.resize(2);
  timeRange_[0] = I3Constants::dt_window_l;
  timeRange_[1] = I3Constants::dt_window_h;
  AddParameter("DirectHitsTimeRange","Time range for calculating direct hits"
               ,timeRange_);

}


//--------------------------------------------------------------
void I3CutsModule::Configure()
{
  log_warn("This module is deprecated.");
  GetParameter("ParticleNames",particleName_);
  GetParameter("ParticleNameList",particleNameList_);
  GetParameter("HitsName",hitsName_);
  GetParameter("PulsesName",pulsesName_);
  GetParameter("NameTag",nameTag_);
  GetParameter("DirectHitsTimeRange",timeRange_);

  if (hitsName_.empty() && pulsesName_.empty())
    log_fatal("Either 'HitsName' or 'PulsesName' parameter HAS to be set!");
  if (!hitsName_.empty() && !pulsesName_.empty())
    log_fatal("Can't set both 'HitsName' and 'PulsesName' parameters!");

  if (!particleName_.empty() && !particleNameList_.empty()){
    log_warn("Parameters passed to both 'ParticleNames' and 'ParticleNameList'");
    log_warn("Please update your script to use 'ParticleNameList' as 'ParticleNames' is deprecated.");
    log_warn("For now the lists will be merged.");
  }

  if (!particleName_.empty()){
    log_warn("This parameter is deprecated. From now on please pass a "
             "list of string names to 'ParticleNameList'.");
    std::vector<std::string> particleNames;
    split(particleNames, particleName_, is_any_of(":,;"));

    BOOST_FOREACH(std::string& name, particleNames)
      particleNameList_.push_back(name);
  }
}


//--------------------------------------------------------------
void I3CutsModule::Physics(I3FramePtr frame)
{
  //---Get geometry......
  const I3Geometry& geometry = frame->Get<I3Geometry>();

  //---Get hit map......
  I3RecoPulseSeriesMapConstPtr pulsemap =
    frame->Get<I3RecoPulseSeriesMapConstPtr>(pulsesName_);

  if (!pulsemap) {
    log_error("The chosen reaodut '%s' is not present.",pulsesName_.c_str());
    PushFrame(frame,"OutBox");
    return;
  }

  //if the user did not specify then process all particles in the frame.
  std::vector<std::string> particleNames(particleNameList_);
  //if the user has specified the I3Particles to process, process them...
  //otherwise particleName_ is empty, so process all I3Particles in the frame...
  if(particleNames.empty()){
    for (I3Frame::typename_iterator iter = frame->typename_begin();
         iter != frame->typename_end();
         iter++) {

      std::string name = iter->first;
      std::string type = iter->second;

      if(type=="I3Particle") particleNames.push_back(name);
    }
  }

  //iterate over the set of strings to see if the frame has an object by that name...
  for (std::vector<std::string>::const_iterator sit = particleNameList_.begin(); 
       sit!=particleNameList_.end(); ++sit) {
    std::string name = *sit;

    I3ParticleConstPtr particle = frame->Get<I3ParticleConstPtr>(name);
    if (!particle) continue;

    log_debug("Found I3Particle: '%s'",name.c_str());
    log_trace("%s",AsXML(particle).c_str());
    log_debug(" ---> calculating cuts for I3Particle '%s'...", name.c_str());

    I3CutValuesBasePtr cuts;
    if(particle->IsTrack()){
      log_debug(" ---> I3Particle '%s' is a track, so proceeding accordingly...", 
                name.c_str());
      cuts = I3CutValuesPtr(new I3CutValues());
    }else if(particle->IsCascade()){
      log_debug(" ---> I3Particle '%s' is a cascade, so proceeding accordingly...", 
                name.c_str());
      cuts = I3CascadeCutValuesPtr(new I3CascadeCutValues());
    } else {
      //this is probably just a failed fit; if not, something is really screwy...
      if(particle->GetFitStatusString()=="OK"){
        log_debug("This I3Particle is neither a track nor a cascade!!");
      } else {
        log_debug("Looks like the fit for this I3Particle failed...");
      }
    }

    if(cuts){
      cuts->Calculate(*particle,geometry,*pulsemap,timeRange_[0],timeRange_[1]);

      frame->Put(name+"Cuts"+nameTag_, cuts);
      log_debug("%s",AsXML(cuts).c_str());
    }
  }

  PushFrame(frame,"OutBox");
}

