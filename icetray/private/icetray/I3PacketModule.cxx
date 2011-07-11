/**
 *  Copyright (C) 2011
 *  Nathan Whitehorn   <nwhitehorn@icecube.wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 */

#include <icetray/I3PacketModule.h>

template<class Base>
I3PacketModuleBase<Base>::I3PacketModuleBase(const I3Context& context) :
    Base(context)
{
	Base::AddParameter("PacketSentinelStream", "Frame type that marks the "
	    "beginning of a new frame packet", I3Frame::DAQ);
}

template<class Base>
I3PacketModuleBase<Base>::~I3PacketModuleBase() {}

template<class Base>
void I3PacketModuleBase<Base>::Configure_()
{
	Base::GetParameter("PacketSentinelStream", sentinel_);
	Base::Configure_();
}

template<class Base>
void I3PacketModuleBase<Base>::Finish()
{
	if (!queue_.empty())
		FramePacket(queue_);

	queue_.clear();
}

template<class Base>
void I3PacketModuleBase<Base>::Process()
{
	I3FramePtr frame = Base::PopFrame();
	if (!frame)
		log_fatal("Not a driving module!");

	if (frame->GetStop() == sentinel_ && !queue_.empty()) {
		FramePacket(queue_);
		queue_.clear();
	}

	queue_.push_back(frame);
}

