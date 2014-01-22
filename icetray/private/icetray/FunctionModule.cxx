#include "FunctionModule.h"

FunctionModule::FunctionModule(const I3Context& context,
                               boost::function<void(boost::shared_ptr<I3Frame>)> f):
I3ConditionalModule(context),funcReturns(false),
vf(f){
	AddParameter("Streams",
	             "A list of frame types which this function should fire on. "
	             "By default runs only on physics frames",
	             std::vector<I3Frame::Stream>(1,I3Frame::Physics));
}

FunctionModule::FunctionModule(const I3Context& context,
                               boost::function<bool(boost::shared_ptr<I3Frame>)> f):
I3ConditionalModule(context),funcReturns(true),
bf(f){
	AddParameter("Streams",
	             "A list of frame types which this function should fire on. "
	             "By default runs only on physics frames",
	             std::vector<I3Frame::Stream>(1,I3Frame::Physics));
}

void FunctionModule::Configure(){
	std::vector<I3Frame::Stream> svec;
	GetParameter("Streams", svec);
	streams = std::set<I3Frame::Stream>(svec.begin(), svec.end());
}

void FunctionModule::Process(){
	boost::shared_ptr<I3Frame> frame = PopFrame();
	if(!frame)
		log_fatal("No frame in inbox:  function modules cannot be driving modules.");
	if(streams.find(frame->GetStop()) == streams.end()){
		//not a stream we're supposed to process, so just hand it off
		PushFrame(frame);
		return;
	}
	if(!funcReturns){ //wrapped function is void
		vf(frame);
		PushFrame(frame);
	}
	else{ //wrapped function returns bool
		bool flag=bf(frame);
		if(flag)
			PushFrame(frame);
	}
}