/**
 *  Copyright (C) 2011
 *  Nathan Whitehorn   <nwhitehorn@icecube.wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 */

#include <icetray/I3PacketModule.h>

#include <boost/foreach.hpp>

I3PacketModule::I3PacketModule(const I3Context& context,
    I3Frame::Stream _sentinel) : I3Module(context), sentinel(_sentinel)
{
	AddParameter("If", "A python function... if this returns something "
	    "that evaluates to True, Module runs, else it doesn't", if_);

	// Set packet_types for the most common use case
	if (sentinel == I3Frame::DAQ) {
		packet_types.push_back(I3Frame::DAQ);
		packet_types.push_back(I3Frame::Physics);
	}
}

I3PacketModule::~I3PacketModule() {}

void I3PacketModule::Configure_()
{
	boost::python::object configured_if_;
	GetParameter("If", configured_if_);
	if (if_.ptr() != configured_if_.ptr()) {
		if (!PyFunction_Check(configured_if_.ptr()))
			log_fatal("'If' parameter to module %s must be a "
			    "python function", GetName().c_str());

		if_ = configured_if_;
	}

	I3Module::Configure_();
}

void I3PacketModule::Finish()
{
	FlushQueue();
}

void I3PacketModule::FlushQueue()
{
	if (!queue_.empty()) {
		if (!if_ || boost::python::extract<bool>(if_(queue_)))
			FramePacket(queue_);
		else
			BOOST_FOREACH(I3FramePtr frame, queue_)
				PushFrame(frame);
		queue_.clear();
	}
}

void I3PacketModule::Process()
{
	I3FramePtr frame = PeekFrame();
	if (!frame)
		log_fatal("Not a driving module!");

	if (!packet_types.empty() && find(packet_types.begin(),
	    packet_types.end(), frame->GetStop()) == packet_types.end()) {
		FlushQueue();
		I3Module::Process();
		return;
	}

	if (frame->GetStop() == sentinel)
		FlushQueue();
	queue_.push_back(PopFrame());
}

void I3PacketModule::FramePacket(std::vector<I3FramePtr> &frames)
{
	BOOST_FOREACH(I3FramePtr frame, frames)
		PushFrame(frame);
}

