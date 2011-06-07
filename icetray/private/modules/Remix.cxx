/**
 *  $Id$
 *  
 *  Copyright (C) 2011
 *  The IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 */

#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>

class Remix : public I3Module
{
  public:
	Remix(const I3Context& context);
	void Process();

  private:
	I3FramePtr cache_;
};

I3_MODULE(Remix);

Remix::Remix(const I3Context& context)
    : I3Module(context), cache_(new I3Frame)
{
	AddOutBox("OutBox");
}

void Remix::Process()
{
	log_trace("%s: %s", GetName().c_str(), __PRETTY_FUNCTION__);

	I3FramePtr frame = PopFrame();
	if (!frame)
		return;

	cache_->purge(frame->GetStop());
	frame->purge();
	cache_->merge(*frame);
	frame->merge(*cache_);

	PushFrame(frame);
}

