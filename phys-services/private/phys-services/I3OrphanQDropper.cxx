/**
 *  $Id$
 *  
 *  Copyright (C) 2011
 *  Nathan Whitehorn <nwhitehorn@icecube.wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 */

#include <icetray/I3PacketModule.h>
#include <boost/foreach.hpp>

class I3OrphanQDropper : public I3PacketModule
{
	public:
		I3OrphanQDropper(const I3Context& context);
		void FramePacket(std::vector<I3FramePtr> &packet);
};

I3_MODULE(I3OrphanQDropper);

I3OrphanQDropper::I3OrphanQDropper(const I3Context& context) :
  I3PacketModule(context, I3Frame::DAQ)
{

	AddOutBox("OutBox");
}

void
I3OrphanQDropper::FramePacket(std::vector<I3FramePtr> &packet)
{
	
	if (packet.size() == 1)
		return; // Drop packets with only a Q frame

	BOOST_FOREACH(I3FramePtr frame, packet)
		PushFrame(frame);
}

