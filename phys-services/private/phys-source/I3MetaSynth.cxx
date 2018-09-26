/**
 *  $Id$
 *  
 *  Copyright (C) 2011
 *  The IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 */

#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>
#include <dataclasses/I3Time.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/calibration/I3Calibration.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/physics/I3EventHeader.h>
#include <interfaces/I3GeometryService.h>
#include <interfaces/I3CalibrationService.h>
#include <interfaces/I3DetectorStatusService.h>

class I3MetaSynth : public I3Module
{
  public:
	I3MetaSynth(const I3Context& context);
	void Configure();
	void Process();

  private:
	void CheckGCDUpdate(I3Time event_time);

	I3GeometryConstPtr geometry_;
	I3CalibrationConstPtr calibration_;
	I3DetectorStatusConstPtr detectorstatus_;

	I3GeometryServicePtr geometryService_;
	I3CalibrationServicePtr calibrationService_;
	I3DetectorStatusServicePtr detectorStatusService_;
};

I3_MODULE(I3MetaSynth);

I3MetaSynth::I3MetaSynth(const I3Context& context)
    : I3Module(context)
{
	AddParameter("GeometryService",
	    "Name of the geometry service to use",
	    I3DefaultName<I3GeometryService>::value());
	AddParameter("CalibrationService",
	    "Name of the calibration service to use",
	    I3DefaultName<I3CalibrationService>::value());
	AddParameter("DetectorStatusService",
	    "Name of the detector status service to use",
	    I3DefaultName<I3DetectorStatusService>::value());

	AddOutBox("OutBox");
}

void I3MetaSynth::Configure()
{

	GetParameter("GeometryService", geometryService_);
	GetParameter("CalibrationService", calibrationService_);
	GetParameter("DetectorStatusService", detectorStatusService_);
}

void I3MetaSynth::Process()
{
	log_trace("%s: %s", GetName().c_str(), __PRETTY_FUNCTION__);

	I3FramePtr frame = PopFrame();
	if (!frame)
		log_fatal("I3MetaSynth is not a driving module. Please add "
		    "one (e.g. I3Reader or I3InfiniteSource)");

	// Allow G/C/D frames in the stream to set our GCD info
	if (frame->GetStop() == I3Frame::Geometry)
		geometry_ = frame->Get<I3GeometryConstPtr>();
	else if (frame->GetStop() == I3Frame::Calibration)
		calibration_ = frame->Get<I3CalibrationConstPtr>();
	else if (frame->GetStop() == I3Frame::DetectorStatus)
		detectorstatus_ = frame->Get<I3DetectorStatusConstPtr>();

	if (frame->Has("I3EventHeader")) {
		I3Time event_time = frame->Get<I3EventHeaderConstPtr>()->
		    GetStartTime();
		CheckGCDUpdate(event_time);
	} else if (frame->Has("DrivingTime")) {
		CheckGCDUpdate(*frame->Get<I3TimeConstPtr>("DrivingTime"));
	}

	PushFrame(frame);
}

void I3MetaSynth::CheckGCDUpdate(I3Time event_time)
{
	if (geometryService_ &&
	    (!geometry_ || (geometry_->startTime > event_time ||
	    geometry_->endTime < event_time))) {
		geometry_ = geometryService_->GetGeometry(event_time);
		I3FramePtr gframe(new I3Frame(I3Frame::Geometry));
		gframe->Put("I3Geometry", geometry_);
		PushFrame(gframe);
	}
	if (calibrationService_ &&
	    (!calibration_ || (calibration_->startTime > event_time ||
	    calibration_->endTime < event_time))) {
		calibration_ = calibrationService_->GetCalibration(event_time);
		I3FramePtr cframe(new I3Frame(I3Frame::Calibration));
		cframe->Put("I3Calibration", calibration_);
		PushFrame(cframe);
	}
	if (detectorStatusService_ &&
	    (!detectorstatus_ || (detectorstatus_->startTime > event_time ||
	    detectorstatus_->endTime < event_time))) {
		detectorstatus_ =
		    detectorStatusService_->GetDetectorStatus(event_time);
		I3FramePtr dframe(new I3Frame(I3Frame::DetectorStatus));
		dframe->Put("I3DetectorStatus", detectorstatus_);
		PushFrame(dframe);
	}
}

