#include "FunctionModule.h"

FunctionModule::FunctionModule(const I3Context& context):
I3ConditionalModule(context){
	AddParameter("Streams",
	             "A list of frame types which this function should fire on. "
	             "By default runs only on physics frames",
	             std::vector<I3Frame::Stream>{I3Frame::Physics});
}

FunctionModule::FunctionModule(const I3Context& context,
                               std::function<void(boost::shared_ptr<I3Frame>)> f):
FunctionModule(context){
	func=[this,f](boost::shared_ptr<I3Frame> frame){
		f(frame);
		PushFrame(frame);
	};
}

FunctionModule::FunctionModule(const I3Context& context,
                               std::function<bool(boost::shared_ptr<I3Frame>)> f):
FunctionModule(context){
	func=[this,f](boost::shared_ptr<I3Frame> frame){
		if(f(frame))
			PushFrame(frame);
	};
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
	if(!streams.count(frame->GetStop()))
		//not a stream we're supposed to process, so just hand it off
		PushFrame(frame);
	else
		func(frame); //calls PushFrame if appropriate
}
