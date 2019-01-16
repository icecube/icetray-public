/**
 *  $Id$
 *  
 *  Copyright (C) 2011
 *  Nathan Whitehorn <nwhitehorn@icecube.wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 */

#include <icetray/I3Module.h>
#include <icetray/I3Units.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <dataclasses/physics/I3EventHeader.h>
#include <dataclasses/I3Time.h>

#include <boost/foreach.hpp>

class I3BadDOMAuditor : public I3Module
{
	public:
		I3BadDOMAuditor(const I3Context& context);
		void Configure();
		void Finish();
		void DetectorStatus(I3FramePtr frame);
		void DAQ(I3FramePtr frame);
	private:
		void CheckResults();
		boost::shared_ptr<const I3Vector<OMKey> > bad_doms_;
		I3GeometryConstPtr geo_;
		std::vector<std::string> pulses_;
		std::vector<OMKey> ignore_oms_;
		std::string bad_dom_list_;
		std::map<OMKey, int> pulse_counts_;
        boost::shared_ptr<const I3Time> good_run_start_time_;
        boost::shared_ptr<const I3Time> good_run_end_time_;
        bool use_good_run_times_;
        bool out_of_good_time_range_;
        bool skip_check;
		bool bad_doms_dont_trigger_, icetop_always_triggers_;
};

I3_MODULE(I3BadDOMAuditor);

I3BadDOMAuditor::I3BadDOMAuditor(const I3Context& context) : I3Module(context)
{
	AddParameter("BadDOMList", "List of DOMs not to check", "BadDomsList");
	AddParameter("Pulses", "Name[s] of pulse series to check",
	    "OfflinePulses");
	AddParameter("BadDOMsDontTrigger", "Fail if any DOM on the bad DOM "
	    "produces any pulses", true);
	AddParameter("IceTopAlwaysTriggers", "Fail if any good IceTop DOM "
	    "fails to produce any pulses", true);
	AddParameter("IgnoreOMs", "A list of OMs that are excluded from the audit", std::vector<OMKey>());
	AddParameter("UseGoodRunTimes", "Use GoodRunStartTime and GoodRunEndTime from D frame if available", false);
	AddOutBox("OutBox");
}

void
I3BadDOMAuditor::Configure()
{
	GetParameter("BadDOMList", bad_dom_list_);
	GetParameter("BadDOMsDontTrigger", bad_doms_dont_trigger_);
	GetParameter("IceTopAlwaysTriggers", icetop_always_triggers_);
	GetParameter("IgnoreOMs", ignore_oms_);
	GetParameter("UseGoodRunTimes", use_good_run_times_);

	try {
		std::string simple_name;
		GetParameter("Pulses", simple_name);
		pulses_.push_back(simple_name);
	} catch (...) {
		PyErr_Clear();
		GetParameter("Pulses", pulses_);
	}

    out_of_good_time_range_ = false;
}

void
I3BadDOMAuditor::Finish()
{
	if (!bad_doms_)
		log_fatal("Finish() called but no bad DOM list so far");

	CheckResults();
}

void
I3BadDOMAuditor::CheckResults()
{
    if(skip_check) {
        return;
    }

	bool err = false;

	#define bad_dom(...) { log_error(__VA_ARGS__); err = true; continue; }

	for (I3OMGeoMap::const_iterator i = geo_->omgeo.begin();
	    i != geo_->omgeo.end(); i++) {
		if (std::find(ignore_oms_.begin(), ignore_oms_.end(), i->first) != ignore_oms_.end()) {
			log_info("Skipped bad dom audit for OM: %s", i->first.str().c_str());
		} else {
			if (std::find(bad_doms_->begin(), bad_doms_->end(), i->first)
			    == bad_doms_->end()) {
				// Don't check for unhit IceTop DOMs if asked not to
				if (i->second.omtype == I3OMGeo::IceTop &&
				    !icetop_always_triggers_)
					continue;

				// DOM is not marked bad -- make sure it triggered
				if (pulse_counts_.find(i->first) == pulse_counts_.end())
					bad_dom("OM%s not marked bad but not present "
					    "in pulse map.", i->first.str().c_str());
				if (pulse_counts_.find(i->first)->second == 0)
					bad_dom("OM%s present in pulse map but never "
					    "produced any pulses. Check calibration "
					    "constants.", i->first.str().c_str());
			} else if (bad_doms_dont_trigger_) {
				// DOM is marked bad -- make sure it did *not* trigger
				if (pulse_counts_.find(i->first) != pulse_counts_.end())
					bad_dom("OM%s marked bad but present in "
					    "pulse map.", i->first.str().c_str());
			}
		}
	}

	// Not strictly bad-DOM-list-related, but we have the data...
	for (std::map<OMKey, int>::const_iterator i = pulse_counts_.begin();
	    i != pulse_counts_.end(); i++) {
		if (geo_->omgeo.find(i->first) == geo_->omgeo.end())
			bad_dom("OM%s triggered but not present in "
			    "geometry.", i->first.str().c_str());
	}

	#undef bad_dom

	if (err)
		log_fatal("Bad DOM list inconsistent with data. Check above.");
}
void
I3BadDOMAuditor::DetectorStatus(I3FramePtr frame)
{
	boost::shared_ptr<const I3Vector<OMKey> > new_bad_doms =
	    frame->Get<boost::shared_ptr<const I3Vector<OMKey> > >(
	    bad_dom_list_);

	if (!new_bad_doms)
		log_fatal("Bad DOMs list named %s does not exist",
		    bad_dom_list_.c_str());

	if (bad_doms_ && *new_bad_doms != *bad_doms_) {
		CheckResults();
		pulse_counts_.clear();
	}

	bad_doms_ = new_bad_doms;
	geo_ = frame->Get<I3GeometryConstPtr>();

    if(use_good_run_times_) {
        good_run_start_time_ = frame->Get<boost::shared_ptr<const I3Time> >("GoodRunStartTime");
        good_run_end_time_ = frame->Get<boost::shared_ptr<const I3Time> >("GoodRunEndTime");
    }

	PushFrame(frame);
}

void
I3BadDOMAuditor::DAQ(I3FramePtr frame)
{
	if (!bad_doms_)
		log_fatal("DAQ frame but no bad DOM list so far");

    I3Time start_time;

    // Skip check if we're already after the good run end time
    skip_check = out_of_good_time_range_;

    // If the good run times should be considered, try to get the start time of the event
    // from the eader
    if(use_good_run_times_ && !skip_check && good_run_start_time_ && good_run_end_time_) {
        boost::shared_ptr<const I3EventHeader> header = frame->Get<boost::shared_ptr<const I3EventHeader> >("I3EventHeader");

        if(header) {
            start_time = header->GetStartTime();

            if(start_time != I3Time()) {
                if(start_time <= *good_run_start_time_) {
                    // If the event time is earlier than the good start time,
                    // it can still happen that we reach an event within the
                    // good times.
                    skip_check = true;
                } else if(start_time >= *good_run_end_time_) {
                    // Since we're now after the good run end time, there should
                    // not come an event that is within the good run times again.
                    // Therefore, skip all other events
                    out_of_good_time_range_ = true;
                    skip_check = true;
                }
            }
        }
    }

    if(!skip_check) {
    	BOOST_FOREACH(const std::string &pulses_name, pulses_) {
    		I3RecoPulseSeriesMapConstPtr pulses =
    		    frame->Get<I3RecoPulseSeriesMapConstPtr>(pulses_name);
    		if (!pulses)
    			continue;
    
    		for (I3RecoPulseSeriesMap::const_iterator i = pulses->begin();
    		    i != pulses->end(); i++)
    			pulse_counts_[i->first] += i->second.size();
    	}
    }

	PushFrame(frame);
}
