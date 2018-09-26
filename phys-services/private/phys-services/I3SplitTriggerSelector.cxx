#include <icetray/I3ConditionalModule.h>

#include <limits>

#include <icetray/I3Units.h>
#include <dataclasses/calibration/I3Calibration.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/physics/I3EventHeader.h>
#include <dataclasses/physics/I3Trigger.h>
#include <dataclasses/physics/I3TriggerHierarchy.h>
#include <dataclasses/physics/I3RecoPulse.h>

///A counterpart to I3TriggerSplitter, which generates subsets of I3TriggerHierarchies for the subevents produced by other splitters.
class I3SplitTriggerSelector : public I3ConditionalModule{
	
//At the moment this can only be used once per scope
#define AlwaysPush(frame) FramePusher guaranteePush(this,frame)

public:
	I3SplitTriggerSelector(const I3Context& context):
	I3ConditionalModule(context),
	inputHierarchyName("I3TriggerHierarchy"),
	physicsTriggersOnly(true){
		AddParameter("SubEventStream","The name of the subevent stream for which to create minimal trigger hierarchies. "
		             "If this parameter is left empty this module will act on all P frames.",subeventStream);
		AddParameter("InputHierarchy","The name of the full trigger hierarchy from which subsets should be constructed",inputHierarchyName);
		AddParameter("SubsetPulses","The set of pulses which should define the time window for constructing the subset of triggers",subsetPulsesName);
		AddParameter("OutputHierarchy","The name of the subset trigger hierarchy to generate",outputHierarchyName);
		AddParameter("PhysicsTriggersOnly","If True, output only physics triggers, not throughput or merged triggers",physicsTriggersOnly);
	}
	
	void Configure(){
		GetParameter("SubEventStream", subeventStream);
		GetParameter("InputHierarchy", inputHierarchyName);
		GetParameter("SubsetPulses", subsetPulsesName);
		GetParameter("OutputHierarchy", outputHierarchyName);
		GetParameter("PhysicsTriggersOnly",physicsTriggersOnly);
		if(subeventStream.empty())
			log_info("%s: No specific subevent stream specified; will run on all P frames",GetName().c_str());
		if(inputHierarchyName.empty())
			log_fatal("%s: No input trigger hierarchy specified. Please set the 'InputHierarchy' parameter.",GetName().c_str());
		if(subsetPulsesName.empty())
			log_fatal("%s: No pulses specified to define the trigger subset. Please set the 'SubsetPulses' parameter.",GetName().c_str());
		if(outputHierarchyName.empty()){
			outputHierarchyName=inputHierarchyName+"Subset";
			log_info("%s: Defaulting ouput hierarchy name to '%s'",GetName().c_str(),outputHierarchyName.c_str());
		}
	}
	
	void Physics(boost::shared_ptr<I3Frame> frame){
		AlwaysPush(frame);
		
		//check whether this is a frame we want to process at all
		if(!subeventStream.empty()){ //we are looking at only a specific stream
			boost::shared_ptr<const I3EventHeader> header=frame->Get<boost::shared_ptr<const I3EventHeader> >("I3EventHeader");
			if(!header){
				log_warn("%s: No event header; ignoring frame",GetName().c_str());
				return;
			}
			if(header->GetSubEventStream()!=subeventStream)
				return;
		}
		
		//locate inputs
		boost::shared_ptr<const I3TriggerHierarchy> triggers=frame->Get<boost::shared_ptr<const I3TriggerHierarchy> >(inputHierarchyName);
		if(!triggers){
			log_warn("%s: No input triggers, so no output will be generated",GetName().c_str());
			return;
		}
		boost::shared_ptr<const I3RecoPulseSeriesMap> pulses=frame->Get<boost::shared_ptr<const I3RecoPulseSeriesMap> >(subsetPulsesName);
		if(!pulses){
			log_warn("%s: No input pulses, so no output will be generated",GetName().c_str());
			return;
		}
		boost::shared_ptr<const I3Calibration> calibration=frame->Get<boost::shared_ptr<const I3Calibration> >();
		if(!calibration)
			log_fatal("%s: No calibration data present",GetName().c_str());
		boost::shared_ptr<const I3DetectorStatus> status=frame->Get<boost::shared_ptr<const I3DetectorStatus> >();
		if(!status)
			log_fatal("%s: No detector status data present",GetName().c_str());
		
		//determine the time range of pulses in this subevent
		double minTime=std::numeric_limits<double>::max(), maxTime=-std::numeric_limits<double>::max();
		for(I3RecoPulseSeriesMap::const_iterator dom=pulses->begin(), domEnd=pulses->end(); dom!=domEnd; dom++){
			if(dom->second.empty())
				continue;
			std::map<OMKey,I3DOMCalibration>::const_iterator domCalib=calibration->domCal.find(dom->first);
			std::map<OMKey,I3DOMStatus>::const_iterator domStatus=status->domStatus.find(dom->first);
			if(domCalib==calibration->domCal.end()){
				log_error("Can't find calibration for DOM (%d,%u)!", dom->first.GetString(), dom->first.GetOM());
				continue;
			}
			if(domStatus==status->domStatus.end()) {
				log_error("Can't find status for DOM (%d,%u)!", dom->first.GetString(), dom->first.GetOM());
				continue;
			}
			//need to add correction because pulses have photocathode times and triggers have mainboard times
			double correction=LaunchTimeCorrection(domCalib->second,domStatus->second);
			minTime=std::min(minTime,dom->second.front().GetTime()+correction);
			maxTime=std::max(maxTime,dom->second.back().GetTime()+correction);
		}
		if(minTime>maxTime){ //no sane splitter should ever generate this case, but. . .
			log_warn("%s: Apparently there were no pulses. No triggers will be output",GetName().c_str());
			return;
		}
		
		//actually collect the trigger subset
		boost::shared_ptr<I3TriggerHierarchy> triggerSubset(new I3TriggerHierarchy);
		
		//collect all relevant subsidiary triggers
		//If we want to keep non-physics triggers and preserve the tree structure
		//of the hierarchy we'll need parent iterators for each insertion,
		//so we'll set up some stacks to keep track of the parent nodes we've seen
		std::vector<I3TriggerHierarchy::iterator> oldParentStack;
		std::vector<I3TriggerHierarchy::iterator> parentStack;
		I3TriggerHierarchy::iterator trigIter=triggers->begin();
		if(trigIter==triggers->end()){
			log_warn("%s: Apparently there were no triggers. No triggers will be output",GetName().c_str());
			return;
		}
		oldParentStack.push_back(trigIter);
		
		//copy the root trigger unconditionally
		I3TriggerHierarchy::iterator subIter=triggerSubset->end();
		if(!physicsTriggersOnly || trigIter->GetTriggerKey().GetSource()!=TriggerKey::GLOBAL
		   || (trigIter->GetTriggerKey().GetType()!=TriggerKey::THROUGHPUT && trigIter->GetTriggerKey().GetType()!=TriggerKey::MERGED))
			subIter=triggerSubset->insert(subIter,*trigIter);
		parentStack.push_back(subIter);
		
		trigIter++;
		for(I3TriggerHierarchy::iterator trigEnd=triggers->end(); trigIter!=trigEnd; trigIter++){
			//only keep the stacks up-to-date if we're preserving structure
			if(!physicsTriggersOnly && triggers->parent(trigIter)!=oldParentStack.back()){
				//at this point we know we need to update the parent stacks,
				//but we don't yet know whether we've just moved down a level,
				//or up some number of levels and then sideways to a sibling
				//we can find out by checking whether the current node's parent is on the stack
				std::vector<I3TriggerHierarchy::iterator>::iterator oldP=std::find(oldParentStack.begin(),oldParentStack.end(),triggers->parent(trigIter));
				if(oldP==oldParentStack.end()){
					//the current node's parent isn't on the stack, so we have moved down
					oldParentStack.push_back(triggers->parent(trigIter));
					parentStack.push_back(subIter);
				}
				else{
					//we've moved up and sideways, so we need to remove all of the parents from the subtree we just left
					//we can determine how many to remove from the old tree's stack, and then remove the same number from the new tree's stack
					unsigned int nRemove=std::distance(oldP,oldParentStack.end())-1;
					oldParentStack.resize(oldParentStack.size()-nRemove);
					parentStack.resize(parentStack.size()-nRemove);
				}
				assert(parentStack.size()==oldParentStack.size() && "parent stacks must stay synchronized!");
			}
			
			if(rangesOverlap(minTime,maxTime,
			                 trigIter->GetTriggerTime(),trigIter->GetTriggerTime()+trigIter->GetTriggerLength())){
				if(physicsTriggersOnly){
					//throughput and merged triggers are not physics triggers
					if(trigIter->GetTriggerKey().GetSource()==TriggerKey::GLOBAL &&
					   (trigIter->GetTriggerKey().GetType()==TriggerKey::THROUGHPUT || trigIter->GetTriggerKey().GetType()==TriggerKey::MERGED))
						continue;
					triggerSubset->insert(triggerSubset->end(),*trigIter);
				}
				else
					subIter=triggerSubset->append_child(parentStack.back(),*trigIter);
			}
		}
		
		frame->Put(outputHierarchyName,triggerSubset);
	}
private:
	std::string subeventStream;
	std::string inputHierarchyName;
	std::string subsetPulsesName;
	std::string outputHierarchyName;
	bool physicsTriggersOnly;
	
	//helper functions
	
	//Borrowed from I3TriggerSplitter (trunk@84833)
	/* *
	 * estimate the delay between launches (trigger) times and pulse times
	 * This is an un-doing of what is done in I3WaveCalibrator::GetStartTime() to shift the times
	 * In no way is this a general physical estimate of the various delays, and therefore,
	 * this calculates a wavecalibrator setting dependent parameter.
	 * fadc delay is included to account for the longest DT situation of having a pulse
	 * start in the first FADC bin
	 * */
	double LaunchTimeCorrection(const I3DOMCalibration& calib, const I3DOMStatus& status){
		const LinearFit& fit = calib.GetTransitTime();
		const double pmtHV = status.pmtHV/I3Units::V;
		const double transit_time = fit.slope/sqrt(pmtHV) + fit.intercept;
		return(transit_time - calib.GetFADCDeltaT()); //fadc dt larger than atwd
	}
	
	bool rangesOverlap(double start1, double stop1, double start2, double stop2){
		return((start1<start2 && start2<stop1) || (start2<start1 && start1<stop2));
	}
	
	void modifyTriggerSpan(I3Trigger& trig, double minTime, double maxTime){
		//if necessary move the start of the trigger forward to the start of the subevent
		if(trig.GetTriggerTime()<minTime){
			trig.SetTriggerLength(trig.GetTriggerLength()-(minTime-trig.GetTriggerTime()));
			trig.SetTriggerTime(minTime);
		}
		//if necessary move the end of the trigger back to the end of the subevent
		if((trig.GetTriggerTime()+trig.GetTriggerLength())>maxTime)
			trig.SetTriggerLength(maxTime-trig.GetTriggerTime());
	}
	
	struct FramePusher{
		I3SplitTriggerSelector* module;
		boost::shared_ptr<I3Frame> frame;
		FramePusher(I3SplitTriggerSelector* m, boost::shared_ptr<I3Frame> f):module(m),frame(f){}
		~FramePusher(){ module->PushFrame(frame); }
	};
};

I3_MODULE(I3SplitTriggerSelector);
