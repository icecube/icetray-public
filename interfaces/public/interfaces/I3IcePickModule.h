#ifndef INTERFACES_I3ICEPICKMODULE_H
#define INTERFACES_I3ICEPICKMODULE_H

#include <dataclasses/physics/I3EventHeader.h>
#include <icetray/I3Module.h>
#include <dataclasses/I3Bool.h>
#include <icetray/I3Frame.h>

/**
 * @brief This is module that does the event selection.  You put it your 
 * module chain and then modules which don't pass the cut don't make it
 * past this module.
 *
 * This module is a template, so when you want to add this module you 
 * need to specify a subclass of I3IcePick that you want to use as the
 * template parameter.  Don't forget to generate a dicitionary for the 
 * new template instantiation.
 */
template <class IcePick>
class I3IcePickModule : public I3Module
{
 public:
  I3IcePickModule(const I3Context& context) :
    I3Module(context),
    decisionName_(I3::name_of<IcePick>()),
    discardEvents_(false),
    invertoutput_(false),
    pick_(context),
    nEventsToPick_ (-1)
    {
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
      
      pick_.ConfigureInterface();
      number_Events_Picked = 0;
      number_Events_Tossed = 0;
    }

  void Physics(I3FramePtr frame)
    {
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
  SET_LOGGER("I3IcePickModule");
};

#endif
