/**
 *  Copyright (C) 2011
 *  Nathan Whitehorn   <nwhitehorn@icecube.wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 */

#include <icetray/I3PacketModule.h>

I3PacketModule::I3PacketModule(const I3Context& context,
    I3Frame::Stream sentinel) : I3Module(context), sentinel_(sentinel)
{
	AddParameter("If", "A python function... if this returns something "
	    "that evaluates to True, Module runs, else it doesn't", if_);
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
	if (!queue_.empty()) {
		if (!if_ || boost::python::extract<bool>(if_(queue_)))
			FramePacket(queue_);
	}

	queue_.clear();
}

void I3PacketModule::Process()
{
	I3FramePtr frame = PopFrame();
	if (!frame)
		log_fatal("Not a driving module!");

	if (frame->GetStop() == sentinel_ && !queue_.empty()) {
		if (!if_ || boost::python::extract<bool>(if_(queue_)))
			FramePacket(queue_);
		queue_.clear();
	}

	queue_.push_back(frame);
}

void I3PacketModule::FramePacket(std::vector<I3FramePtr> &frames)
{
}

