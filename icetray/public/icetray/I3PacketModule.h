/**
 *  Copyright (C) 2011
 *  Nathan Whitehorn   <nwhitehorn@icecube.wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 */
#ifndef ICETRAY_I3PACKETMODULE_H_INCLUDED
#define ICETRAY_I3PACKETMODULE_H_INCLUDED

#include <icetray/I3Module.h>
#include <vector>

/**
 * @brief This class is meant to be a wedge between an I3Module and any module
 * that wants to process a "packet" of frames related to the same detector
 * configuration, in some generalized sense. The usual use is for a module
 * that wants to act on a DAQ trigger (DAQ frame) followed by its split
 * subevents (Physics frames) as a single unit.
 *
 */

class I3PacketModule : public I3Module
{
	public:
		I3PacketModule(const I3Context& context);
		~I3PacketModule();

		void Configure_();
		void Process();
		void Finish();

		virtual void FramePacket(std::vector<I3FramePtr> &packet);

	private:
		I3Frame::Stream sentinel_;
		std::vector<I3FramePtr> queue_;
		boost::python::object if_;
};

#endif

