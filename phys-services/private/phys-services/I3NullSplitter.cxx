/**
 *  $Id$
 *  
 *  Copyright (C) 2011
 *  Nathan Whitehorn <nwhitehorn@icecube.wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 */

#include <icetray/I3ConditionalModule.h>
#include <dataclasses/I3MapOMKeyMask.h>
#include <phys-services/I3Splitter.h>

#include <string>

class I3NullSplitter : public I3ConditionalModule, public I3Splitter
{
	public:
		I3NullSplitter(const I3Context& context);
		void Configure();
		void DAQ(I3FramePtr frame);
	private:
		std::string input_, output_;
};

I3_MODULE(I3NullSplitter);

I3NullSplitter::I3NullSplitter(const I3Context& context) :
  I3ConditionalModule(context), I3Splitter(configuration_)
{
	AddParameter("InputPulseSeries", "The name of the pulse series to mask "
	    "(optional)", "");
	AddParameter("OutputPulseSeriesMask", "The name of the output pulse "
	    "series mask (optional)", "");
	AddParameter("SubEventStreamName",
		     "The name of the SubEvent stream.",
		     configuration_.InstanceName());
	AddOutBox("OutBox");
}

void
I3NullSplitter::Configure()
{
	GetParameter("InputPulseSeries", input_);
	GetParameter("OutputPulseSeriesMask", output_);
	GetParameter("SubEventStreamName", sub_event_stream_name_);
}

void
I3NullSplitter::DAQ(I3FramePtr frame)
{
	PushFrame(frame);

	I3FramePtr subevent = GetNextSubEvent(frame);

	// If requested, make a pulse mask containing all of the pulses
	if (subevent->Has(input_) && output_ != "") {
		I3RecoPulseSeriesMapMaskPtr mask(new
		    I3RecoPulseSeriesMapMask(*subevent, input_));
		subevent->Put(output_, mask);
	}

	PushFrame(subevent);
}

