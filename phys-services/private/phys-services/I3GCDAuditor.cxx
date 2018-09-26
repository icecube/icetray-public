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
#include <dataclasses/calibration/I3Calibration.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/I3DOMFunctions.h>

class I3GCDAuditor : public I3Module
{
	public:
		I3GCDAuditor(const I3Context& context);
		void Configure();
		void DetectorStatus(I3FramePtr frame);
	private:
		bool CheckDOM(OMKey om, const I3OMGeo &omgeo,
		    const I3DOMCalibration &cal, const I3DOMStatus &status);
		std::string bad_dom_list_;
		bool AMANDA_is_error_;
		bool max_paranoia_;
};

I3_MODULE(I3GCDAuditor);

I3GCDAuditor::I3GCDAuditor(const I3Context& context) : I3Module(context)
{
	AddParameter("BadDOMList", "List of DOMs not to check", "BadDomsList");
	AddParameter("AMANDAIsAnError", "Fail if geometry contains TWR OMs",
	    true);
	AddParameter("MaximumParanoia", "Fail for nonsense quantities that "
	    "are mostly only relevant for simulation (e.g. PMT discriminator "
	    "thresholds) as well as things that are unreasonable but not "
	    "universally fatal", true);
	AddOutBox("OutBox");
}

void
I3GCDAuditor::Configure()
{
	GetParameter("BadDOMList", bad_dom_list_);
	GetParameter("AMANDAIsAnError", AMANDA_is_error_);
	GetParameter("MaximumParanoia", max_paranoia_);
}

void
I3GCDAuditor::DetectorStatus(I3FramePtr frame)
{
	const I3Vector<OMKey> &bdl = frame->Get<const I3Vector<OMKey> >(
	    bad_dom_list_);
	const I3Geometry &geo = frame->Get<I3Geometry>();
	const I3Calibration &calib = frame->Get<I3Calibration>();
	const I3DetectorStatus &status = frame->Get<I3DetectorStatus>();

	bool err = false;
	
	// checks for acausal G/C/D frames
	if (geo.startTime >= geo.endTime) {
		log_error("Geometry start time %d,%ju is after end time %d,%ju",
		    geo.startTime.GetUTCYear(),
		    (uintmax_t)geo.startTime.GetUTCDaqTime(),
		    geo.endTime.GetUTCYear(),
		    (uintmax_t)geo.endTime.GetUTCDaqTime());
		err = true;
	}
	if (calib.startTime >= calib.endTime) {
		log_error("Calibration start time %d,%ju is after "
		    "end time %d,%ju", calib.startTime.GetUTCYear(),
		    (uintmax_t)calib.startTime.GetUTCDaqTime(),
		    calib.endTime.GetUTCYear(),
		    (uintmax_t)calib.endTime.GetUTCDaqTime());
		err = true;
	}
	if (status.startTime >= status.endTime) {
		log_error("Detector status start time %d,%ju is after "
		    "end time: %d,%ju", status.startTime.GetUTCYear(),
		    (uintmax_t)status.startTime.GetUTCDaqTime(),
		    status.endTime.GetUTCYear(),
		    (uintmax_t)status.endTime.GetUTCDaqTime());
		err = true;
	}

	#define paranoia(...) if (max_paranoia_) { \
			log_error(__VA_ARGS__); err = true; \
		} else { \
			log_warn(__VA_ARGS__); err = true; \
		}
	
	// Check that various times are reasonable
	if (status.endTime.GetUTCYear() == 2038 /* 32-bit UNIX end of days */) {
		paranoia("Detector status validity ends in 2038, the UNIX "
		    "end of days, which is usually wrong");
	}

	if ((status.endTime - status.startTime)/I3Units::gregorianyear >= 1) {
		paranoia("difference between D end time %d,%ju and "
		    "start time: %d,%ju is (%f yrs) greater than 1yr, which "
		    "is likely wrong", status.endTime.GetUTCYear(),
		    (uintmax_t)status.endTime.GetUTCDaqTime(),
		    status.startTime.GetUTCYear(),
		    (uintmax_t)status.startTime.GetUTCDaqTime(),
		    (status.endTime - status.startTime)/I3Units::gregorianyear);
	}

	#undef paranoia

	for (I3OMGeoMap::const_iterator i = geo.omgeo.begin();
	   i != geo.omgeo.end(); i++) {
		if (std::find(bdl.begin(), bdl.end(), i->first) != bdl.end())
			continue;

		#define bad_dom(...) { log_error(__VA_ARGS__); err = true; \
		    continue; }

		// Check for AMANDA OMs, skipping if they are not an error
		if (i->second.omtype == I3OMGeo::AMANDA && AMANDA_is_error_)
			bad_dom("Geometry contains AMANDA OM%s",
			    i->first.str().c_str());
		if (i->second.omtype == I3OMGeo::AMANDA)
			continue;

		if (calib.domCal.find(i->first) == calib.domCal.end())
			bad_dom("OM%s has no calibration",
			    i->first.str().c_str());
		if (status.domStatus.find(i->first) == status.domStatus.end())
			bad_dom("OM%s has no detector status",
			    i->first.str().c_str());

		#undef bad_dom

		if (!CheckDOM(i->first, i->second,
		    calib.domCal.find(i->first)->second,
		    status.domStatus.find(i->first)->second))
			err = true;
	}

	if (err)
		log_fatal("Errors in GCD information. Check above for "
		    "details.");

	PushFrame(frame);
}

bool I3GCDAuditor::CheckDOM(OMKey om, const I3OMGeo &omgeo,
    const I3DOMCalibration &cal, const I3DOMStatus &status)
{
	#define bad_dom(...) { log_error(__VA_ARGS__); return false; }
	#define sim_bad_dom(...) { log_error(__VA_ARGS__); \
	    if (max_paranoia_) return false; \
	    else \
	    log_error("WARNING! The above error indicates a serious problem, " \
	     "which has been ignored due to the setting of MaximumParanoia. " \
	     "It will likely cause simulation to fail!");}

	// Check geometry
	if (!std::isfinite(omgeo.position.GetX()) ||
	    !std::isfinite(omgeo.position.GetY()) ||
	    !std::isfinite(omgeo.position.GetZ()))
		bad_dom("OM%s has invalid position", om.str().c_str());

	// Check that at least one digitizer is on
	if (!(status.statusATWDa == I3DOMStatus::On ||
	    status.statusATWDb == I3DOMStatus::On ||
	    status.statusFADC == I3DOMStatus::On))
		bad_dom("OM%s has all digitizers disabled but is marked good",
		    om.str().c_str());


	// Check calibration
	for (int chip = 0; chip < 2; chip++) {
	    if (chip == 0 && status.statusATWDa == I3DOMStatus::Off)
		continue;
	    if (chip == 1 && status.statusATWDb == I3DOMStatus::Off)
		continue;
	    for (int chan = 0; chan < 3; chan++)
		if (!std::isfinite(cal.GetATWDBeaconBaseline(chip, chan)))
			bad_dom("Invalid baselines for OM%s ATWD chip %d, "
			    "channel %d", om.str().c_str(), chip, chan);
	}
	if (!std::isfinite(cal.GetFADCBeaconBaseline()))
		bad_dom("Invalid baselines for OM%s FADC", om.str().c_str());

	for (int chan = 0; chan < 3; chan++)
		if (!std::isfinite(cal.GetATWDGain(chan)) ||
		    cal.GetATWDGain(chan) >= 0)
			bad_dom("Invalid ATWD gain for OM%s channel %d (%e)",
			    om.str().c_str(), chan, cal.GetATWDGain(chan));
	if (!std::isfinite(cal.GetFADCGain()) || cal.GetFADCGain() <= 0)
		bad_dom("Invalid FADC gain for OM%s (%e)", om.str().c_str(),
		    cal.GetFADCGain());
	for (int chip = 0; chip < 2; chip++) {
	    if (chip == 0 && status.statusATWDa == I3DOMStatus::Off)
		continue;
	    if (chip == 1 && status.statusATWDb == I3DOMStatus::Off)
		continue;
	    for (int chan = 0; chan < 3; chan++)
		for (int bin = 0; bin < 128; bin++) {
			if (!std::isfinite(cal.GetATWDBinCalibSlope(chip, chan,
			    bin)) ||
			    cal.GetATWDBinCalibSlope(chip, chan, bin) >= 0)
				bad_dom("Invalid bin calibration for OM%s "
				    "ATWD chip %d, channel %d, bin %d (%e)",
				    om.str().c_str(), chip, chan, bin,
				    cal.GetATWDBinCalibSlope(chip, chan,
				    bin));
		}
	}

	if (!std::isfinite(cal.GetRelativeDomEff()))
		bad_dom("Invalid DOM efficiency for OM%s", om.str().c_str());
	
	if (!std::isfinite(cal.GetDomNoiseRate()) || cal.GetDomNoiseRate() <= 0
	    || cal.GetDomNoiseRate() > 10000*I3Units::hertz)
		bad_dom("Invalid noise rate for OM%s (%f Hz)",
		    om.str().c_str(), cal.GetDomNoiseRate()/I3Units::hertz);

	if (!std::isfinite(cal.GetFrontEndImpedance()) ||
	    cal.GetFrontEndImpedance() <= 0 ||
	    cal.GetFrontEndImpedance() > 100*I3Units::ohm)
		bad_dom("Invalid front end impedance for OM%s (%f Ohm)",
		    om.str().c_str(), cal.GetFrontEndImpedance()/I3Units::ohm);

	// Check Detector Status
	if (!std::isfinite(status.pmtHV) || status.pmtHV < 100.*I3Units::V ||
	    status.pmtHV > 2000.*I3Units::V)
		bad_dom("Invalid PMT high voltage for OM%s (%f V)",
		    om.str().c_str(), status.pmtHV/I3Units::V);
	
	// Check composite quantities
	if (!std::isfinite(SPEMean(status, cal)) || SPEMean(status, cal) <= 0)
		bad_dom("Invalid mean SPE charge for OM%s (%f pC)",
		    om.str().c_str(), SPEMean(status, cal)/I3Units::pC);

	if (omgeo.omtype!=I3OMGeo::Scintillator) {
	    if (!std::isfinite(TransitTime(status, cal)) ||
	        TransitTime(status, cal) <= 0 ||
	        TransitTime(status, cal) > 200*I3Units::ns)
		    bad_dom("Invalid transit time for OM%s (%f ns)",
    		    om.str().c_str(), TransitTime(status, cal)/I3Units::ns);
	} else {
	  log_info("Transit time check skipped for scintillator OM: %s", om.str().c_str());
	}

	if (!std::isfinite(PMTGain(status, cal)) || PMTGain(status, cal) <= 0 ||
	    PMTGain(status, cal) > 5e7)
		bad_dom("Invalid PMT gain for OM%s (%e)",
		    om.str().c_str(), PMTGain(status, cal));
	for (int chip = 0; chip < 2; chip++) {
		if (chip == 0 && status.statusATWDa == I3DOMStatus::Off)
			continue;
		if (chip == 1 && status.statusATWDb == I3DOMStatus::Off)
			continue;
		if (!std::isfinite(ATWDSamplingRate(chip, status, cal)) || 
		    ATWDSamplingRate(chip, status, cal) <= 0 ||
		    ATWDSamplingRate(chip, status, cal) >
		    400*I3Units::megahertz) {
			bad_dom("Invalid ATWD sampling rate for OM%s chip %d "
			    "(%f MHz)", om.str().c_str(), chip,
			    ATWDSamplingRate(chip, status, cal)/
			    I3Units::megahertz);
		}
	}
	if (!std::isfinite(SPEPMTThreshold(status, cal)) ||
	    SPEPMTThreshold(status, cal) <= 0)
		sim_bad_dom("Invalid SPE threshold for OM%s (%f mV)",
		    om.str().c_str(), SPEPMTThreshold(status, cal)/I3Units::mV);

	return true;

	#undef bad_dom
	#undef sim_bad_dom
}

