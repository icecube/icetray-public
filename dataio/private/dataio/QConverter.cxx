/**
 *  $Id$
 *  
 *  Copyright (C) 2011
 *  Nathan Whitehorn <nwhitehorn@icecube.wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 */
#include <icetray/I3Frame.h>
#include <icetray/I3Context.h>
#include <icetray/I3Configuration.h>
#include <icetray/I3Module.h>
#include <dataclasses/physics/I3EventHeader.h>

class QConverter : public I3Module
{
	public:
		QConverter(const I3Context& context);
		void Configure();
		void Physics(I3FramePtr frame);
		void DAQ(I3FramePtr frame);
	private:
		std::string subevent_stream;
		std::vector<std::string> keys_to_q;
		std::vector<std::string> types_to_q;
		bool output_p_frame;
                SET_LOGGER("QConverter");
};

I3_MODULE(QConverter);

QConverter::QConverter(const I3Context& context) :
    I3Module(context)
{

	keys_to_q.push_back("DrivingTime");
	keys_to_q.push_back("CorsikaWeightMap");
	keys_to_q.push_back("I3MCTree");
	keys_to_q.push_back("I3MCTreeGen");
	keys_to_q.push_back("I3MCWeightDict");
	keys_to_q.push_back("CalibratedATWD");
	keys_to_q.push_back("CalibratedFADC");
	keys_to_q.push_back("I3DAQData");
	keys_to_q.push_back("InIceRawData");
	keys_to_q.push_back("IceTopRawData");
	AddParameter("QKeys", "Names of keys to move to the Q frame",
	    keys_to_q);

	types_to_q.push_back("I3EventHeader");
	types_to_q.push_back("I3Map<OMKey, vector<I3Waveform> >");
	types_to_q.push_back("I3Map<OMKey, vector<I3DOMLaunch> >");
	types_to_q.push_back("I3Map<OMKey, vector<I3MCHit> >");
	types_to_q.push_back("I3Map<OMKey, vector<I3RecoPulse> >");
	types_to_q.push_back("I3Map<OMKey, I3PortiaPulse>");
	types_to_q.push_back("I3PortiaEvent");
	types_to_q.push_back("I3Tree<I3Trigger>");
	types_to_q.push_back("JEBEventInfo");
	types_to_q.push_back("I3Vector<I3MMCTrack>");
	AddParameter("QTypes", "Types of keys to move to the Q frame",
	    types_to_q);
	
	output_p_frame = true;
	AddParameter("WritePFrame", "If False, P frames are converted into Q"
	    "frames without the creation of P frames",
	    output_p_frame);
	
	AddOutBox("OutBox");
}

void
QConverter::Configure()
{
	subevent_stream = GetName();

	GetParameter("QKeys", keys_to_q);
	GetParameter("QTypes", types_to_q);
	GetParameter("WritePFrame", output_p_frame);
}

void
QConverter::Physics(I3FramePtr frame)
{
	// Purge all non-native keys, so we don't reassign GCD keys
	frame->purge();

	// Reassign special frame objects' stops from P -> Q
	for (I3Frame::typename_iterator iter = frame->typename_begin();
	    iter != frame->typename_end(); iter++) {
		for (std::vector<std::string>::const_iterator toqiter =
		    types_to_q.begin(); toqiter != types_to_q.end(); toqiter++)
			if (iter->second == *toqiter)
				frame->ChangeStream(iter->first, I3Frame::DAQ);
	}
	for (std::vector<std::string>::const_iterator iter = keys_to_q.begin();
	    iter != keys_to_q.end(); iter++) {
		if (frame->Has(*iter))
			frame->ChangeStream(*iter, I3Frame::DAQ);
	}

	// If there is no event header, reassign all keys to the Q frame,
	// since we can't make a stub P frame
	if (!output_p_frame || !frame->Has("I3EventHeader")) {
		for (I3Frame::typename_iterator iter = frame->typename_begin();
		    iter != frame->typename_end(); iter++)
			frame->ChangeStream(iter->first, I3Frame::DAQ);
	}

	// Clone Q bits to a new Q frame
	I3FramePtr daq(new I3Frame(*frame));
	daq->SetStop(I3Frame::DAQ);
	daq->purge(I3Frame::Physics);
	PushFrame(daq);
	
	// Write remnant keys, if they exist and it was requested, to a stub P
	if(output_p_frame && frame->Has("I3EventHeader")) {
		// Rewrite event header in P frame
		I3EventHeaderPtr header(new
		    I3EventHeader(frame->Get<I3EventHeader>()));
		header->SetSubEventStream(subevent_stream);
		header->SetSubEventID(0);
		frame->Delete("I3EventHeader");
		frame->Put("I3EventHeader", header);
		PushFrame(frame);
	}
}

void
QConverter::DAQ(I3FramePtr frame)
{
	log_fatal("Trying to QConvert data already containing Q frames!");
}

