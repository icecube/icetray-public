#ifndef INTERFACES_I3ICEPICKMODULE_H
#define INTERFACES_I3ICEPICKMODULE_H

#include <dataclasses/physics/I3EventHeader.h>
#include <icetray/I3ConditionalModule.h>
#include <icetray/I3Bool.h>
#include <icetray/I3Frame.h>

#include <boost/foreach.hpp>
#include <boost/python.hpp>

/**
 * @brief This is module that does the event selection.  You put it your 
 * module chain and then frames which don't pass the cut don't make it
 * past this module.
 *
 * This module is a template, so when you want to add this module you 
 * need to specify a subclass of I3IcePick that you want to use as the
 * template parameter.  Don't forget to generate a dictionary for the 
 * new template instantiation.
 */
template <class IcePick>
class I3IcePickModule : public I3ConditionalModule
{
 public:
  I3IcePickModule(const I3Context& context) :
    I3ConditionalModule(context),
    decisionName_(I3::name_of<IcePick>()),
    discardEvents_(false),
    invertoutput_(false),
    pick_(context),
    nEventsToPick_ (-1)
    {
      // Synchronize the two configurations via the back door
      BOOST_FOREACH(const std::string &key, pick_.configuration_->keys()) {
         std::string description;
         boost::python::object def;
         description = pick_.configuration_->GetDescription(key);
         def = pick_.configuration_->Get(key);
         AddParameter(key, description, def);
      }
      delete pick_.configuration_;
      pick_.configuration_ = &configuration_;

      AddParameter("DecisionName",
		   "Name of the filter decision in the Frame",
		   decisionName_);
      AddParameter("DiscardEvents",
		   "Set to 'true' if we're supposed to "
		   "actually discard events",
		   discardEvents_);
      AddParameter("NEventsToPick",
		   "Number of events to pick.  If set to a value other than -1,"
		   "IceTray will stop processing once after this many"
		   "selected events by requesting suspension",
		   nEventsToPick_);
      AddParameter("InvertFrameOutput",
		   "Set to 'true' if you want the I3Bool that is written to the "
		   "frame to be 'true' when the event fails the IcePick and 'false' "
		   "when the event passes the IcePick (as in the old reverse-Pretzian "
		   "logic).  This ONLY affects the frame object output.",
		   invertoutput_);

      streams_.push_back(I3Frame::Physics);
      AddParameter("Streams",
                   "Frame stops on which to operate. All other stop types are "
                   "ignored.",
                   streams_);

      AddOutBox("OutBox");
    }

  void Configure()
    {
      GetParameter("DecisionName",
		   decisionName_);
      GetParameter("DiscardEvents",
		   discardEvents_);
      GetParameter("NEventsToPick",
		   nEventsToPick_);
      GetParameter("InvertFrameOutput",
		   invertoutput_);
      GetParameter("Streams", streams_);
      if (streams_.size() == 0)
          log_fatal("IcePick set to run on no frame types!");
      
      pick_.ConfigureInterface();
      number_Events_Picked = 0;
      number_Events_Tossed = 0;
    }

  void Process()
    {
      I3FramePtr frame = PopFrame();
      if (!frame)
          log_fatal("Icepicks are not driving modules.");

      if (find(streams_.begin(), streams_.end(), frame->GetStop()) ==
        streams_.end()) {
          PushFrame(frame);
          return;
      }

      I3BoolPtr decision(new I3Bool(pick_.SelectFrameInterface(*frame)));
      if (decisionName_!="")
	{
	  if(invertoutput_)
	    {
	      I3BoolPtr inverted(new I3Bool(!decision->value));
	      frame->Put(decisionName_,inverted);
	    }
	  else frame->Put(decisionName_,decision);
	}
      if(!decision->value) number_Events_Tossed++;
      else number_Events_Picked++;
      log_trace("IcePick counts: count:%i goal:%i",
	       number_Events_Picked,nEventsToPick_);      
      if(nEventsToPick_>0){
	if(number_Events_Picked>=nEventsToPick_){
	  log_info("IcePick %s reached requested number of events.  Stopping",
		   decisionName_.c_str());
	  I3Module::RequestSuspension();
	}
      }
      if(discardEvents_)
	{
	  if(!decision->value)
	    {
	      return;
	    }
	}

      PushFrame(frame,"OutBox");
    }

  void Finish(){
      log_info( "(%s) Kept %d events, rejected %d.",
                GetName().c_str(),
                number_Events_Picked,
                number_Events_Tossed );
  }

 private:
  std::string decisionName_;
  bool discardEvents_;
  bool invertoutput_;
  IcePick pick_;
  int nEventsToPick_;
  int number_Events_Picked;
  int number_Events_Tossed;
  std::vector<I3Frame::Stream> streams_;
  SET_LOGGER("I3IcePickModule");
};

#endif
