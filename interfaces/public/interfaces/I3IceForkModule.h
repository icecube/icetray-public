#ifndef INTERFACES_I3ICEFORKMODULE_H
#define INTERFACES_I3ICEFORKMODULE_H

#include <icetray/I3Module.h>
#include <icetray/I3Bool.h>
#include <icetray/I3Frame.h>

/**
 * @brief This module is just like I3IcePickModule except that it
 * does the event selection and then dumps the rejected frame to 
 * it's TrashBox.
 *
 * This module is a template, so when you want to add this module you 
 * need to specify a subclass of I3IcePick that you want to use as the
 * template parameter.  
 */
template <class IcePick>
class I3IceForkModule : public I3Module
{
 public:
  I3IceForkModule(const I3Context& context) : 
    I3Module(context),
    decisionName_(I3::name_of<IcePick>()),
    pick_(context) 
    {
      // Synchronize the two configurations via the back door
      configuration_ = *pick_.configuration_;
      delete pick_.configuration_;
      pick_.configuration_ = &configuration_;

      AddParameter("DecisionName",
		   "Name of the filter decision in the Frame",
		   decisionName_);
      AddOutBox("TrueBox");
      AddOutBox("FalseBox");
    }

  void Configure()
    {
      GetParameter("DecisionName",
		   decisionName_);

      pick_.ConfigureInterface();
    }

  void Physics(I3FramePtr frame)
    {
      I3BoolPtr decision(new I3Bool(pick_.SelectFrameInterface(*frame)));
      if(!decisionName_.empty())
        frame->Put(decisionName_,decision);
      if(!decision->value)
	{
	  PushFrame(frame,"FalseBox");
	  return;
	}
      PushFrame(frame,"TrueBox");
    }

 private:
  std::string decisionName_;
  IcePick pick_;
};

#endif
