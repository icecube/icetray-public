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
 * Packet processing will send a packet starting with a frame of type sentinel
 * (or stream start) and ending with either another frame of type sentinel, 
 * or a frame type not found in the packet_types vector. Receipt of a frame
 * not in packet_types will cause FramePacket() to be called with the current
 * buffer and the frame to be handled by I3Module::Process() (i.e.
 * Geometry()/Calibration()/etc.)
 *
 * NB: If packet_types is empty, packets will only be terminated by a new
 * sentinel frame. If sentinel is set to I3Frame::DAQ in the constructor,
 * packet_types will be initialized to [I3Frame::DAQ, I3Frame::Physics].
 * Otherwise it will be left blank by default.
 */

class I3PacketModule : public I3Module
{
	public:
		I3PacketModule(const I3Context& context,
		    I3Frame::Stream sentinel = I3Frame::DAQ);
		~I3PacketModule();

		void Configure_();
		void Process();
		void FlushQueue();
		void Finish();

		virtual void FramePacket(std::vector<I3FramePtr> &packet);

	protected:
		I3Frame::Stream sentinel;
		std::vector<I3Frame::Stream> packet_types;

	private:
		std::vector<I3FramePtr> queue_;
		boost::python::object if_;
};

#endif

