/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy$
 */

#include "dataio/I3MuxingReaderService.h"

#include <dataclasses/physics/I3EventHeader.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/calibration/I3Calibration.h>
#include <dataclasses/status/I3DetectorStatus.h>

I3MuxingReaderService::I3MuxingReaderService(
    const std::vector<std::vector<std::string > > &filenames,
    const std::vector<std::string > &skip, bool no_driving_time) : 
    nframes_(0)
{
	std::vector<std::vector<std::string > >::const_iterator iter = filenames.begin();
	for ( ; iter != filenames.end(); iter++) {
		SourceBundle bundle;
		bundle.reader = I3ReaderServicePtr(
		    new I3ReaderService(*iter, skip, no_driving_time));
		bundle.frame = I3FramePtr();
		sources_.push_back(bundle);
	}
}

I3MuxingReaderService::~I3MuxingReaderService() 
{ 
}


I3FramePtr
I3MuxingReaderService::PopMeta()
{	
	I3FramePtr frame;
	
	std::vector<SourceBundle>::const_iterator source_iter = sources_.begin();
	
	/* 
	 * Merge the keys of any non-GCD (i.e. TrayInfo) frames.
	 * These should be unique.
	 */
	for ( ; source_iter != sources_.end(); source_iter++) {
		
		/* We read too far in the last iteration. Do nothing. */
		if (source_iter->frame)
			continue;
		
		I3FramePtr tmpframe = source_iter->reader->PopMeta();
		
		if (!tmpframe)
			continue;
		
		if (!frame)
			frame = tmpframe;
		else
			for(I3Frame::typename_iterator iter = tmpframe->typename_begin();
			    iter != tmpframe->typename_end(); iter++)
				  frame->take(*tmpframe, iter->first);
	}
	
	return frame;
}


/*
 * XXX FIXME: this would be significantly more efficient if we had a 
 * way to index into an arbitrary frame rather than reading the entire
 * stream. For now, we bite the bullet and just repeatedly call
 * into I3ReaderService until we get what we want.
 */

void
I3MuxingReaderService::QueueEvent()
{
	next_event_.reset();
	
	I3FramePtr frame(new I3Frame(I3Frame::Physics));
	std::vector<SourceBundle>::reverse_iterator source_iter = sources_.rbegin();
	
	/* XXX FIXME: for now, the last source provides the canonical driving time */
	assert( source_iter->reader->MoreEvents() );

	/* 
	 * XXX FIXME: always pop for now.
	 * (the canonical stream can never be out of sync)
	 */
	next_time_ = source_iter->reader->PopEvent(*frame);
	source_iter++;
	
	for ( ; source_iter != sources_.rend(); source_iter++) {
		/* We have an appropriate frame waiting for us. Consume it. */
		if (source_iter->frame && source_iter->time == next_time_) {
			log_trace("Consuming overshot frame.");
			MergeFrames(frame, source_iter->frame);
			source_iter->frame.reset();
			continue;
		}
		
		/* Nothing left to read. */
		if (!source_iter->reader->MoreEvents())
			continue;
		
		I3FramePtr tmpframe(new I3Frame(I3Frame::Physics));
		I3Time dtime = source_iter->reader->PopEvent(*tmpframe);
		
		/* Search forward for a frame with the canonical driving time. */
		while (dtime < next_time_) {
			tmpframe = I3FramePtr(new I3Frame(I3Frame::Physics));
			source_iter->reader->PopMeta();
			if (!source_iter->reader->MoreEvents())
				break;
			dtime = source_iter->reader->PopEvent(*tmpframe);
		}
		
		/* 
		 * We've overshot the desired time. Save the popped
		 * frame and time for later.
		 */
		if (dtime != next_time_) {
			source_iter->frame = tmpframe;
			source_iter->time = dtime;
			continue;
		}
		
		MergeFrames(frame, tmpframe);
	}
	
	next_event_ = frame;
	injected_event_ = I3FramePtr(new I3Frame(*frame)); /* Save a copy for comparison. */
	log_trace("Injected event has %zu keys.", injected_event_->size());
}

void
I3MuxingReaderService::DiffEvent(I3Frame &processed)
{
	if (!injected_event_)
		return;
	
	std::vector<std::string> deleteme;
	I3FrameObjectConstPtr old_obj, new_obj;
	
	/* Drop any keys that were in the original frame and never overwritten. */
	I3Frame::typename_iterator iter = processed.typename_begin();
	for ( ; iter != processed.typename_end(); iter++) {
		if (!injected_event_->Has(iter->first))
			continue; /* A new key */
		
		try {
			old_obj = injected_event_->Get<I3FrameObjectConstPtr>(iter->first, true);
			new_obj = processed.Get<I3FrameObjectConstPtr>(iter->first, true);
		} catch (const boost::archive::archive_exception& e) {
			switch (e.code) {
				case boost::archive::archive_exception::unregistered_class:
				case boost::archive::archive_exception::unsupported_version:
					deleteme.push_back(iter->first); /* Couldn't have been modified. */
					continue;
				default:
					throw;
			}
		}
		
		if (old_obj == new_obj)
			deleteme.push_back(iter->first);
	}
	
	std::vector<std::string>::const_iterator key_it = deleteme.begin();
	for ( ; key_it != deleteme.end(); key_it++)
		processed.Delete(*key_it);
	
	processed.purge();
}

void
I3MuxingReaderService::MergeFrames(I3FramePtr master, I3FramePtr subframe)
{
	log_trace("Merging %zu keys into existing %zu", subframe->size(), master->size());
	I3Frame::typename_iterator iter = subframe->typename_begin();
	for ( ; iter != subframe->typename_end(); iter++)
		if (!master->Has(iter->first)) {
			master->take(*subframe, iter->first);
		}
}

bool 
I3MuxingReaderService::MoreEvents()
{
	bool more = sources_.back().reader->MoreEvents();
	log_trace("MoreEvents(): %d", more);
	return more;
}

I3Time
I3MuxingReaderService::PopEvent(I3Frame &frame)
{
	QueueEvent(); /* Align the streams and build a combined frame. */
	
	I3Frame::typename_iterator iter = next_event_->typename_begin();
	for ( ; iter != next_event_->typename_end(); iter++)
		frame.take(*next_event_, iter->first);
	next_event_.reset();
	return next_time_;
}

I3DetectorStatusConstPtr
I3MuxingReaderService::GetDetectorStatus(I3Time time)
{
	/* Return the cached status if current */
	if (status_source_ && status_ == status_source_->GetDetectorStatus(time))
		return status_;
	
	/* 
	 * Query the sources in reverse order, picking the first one that returns
	 * a non-NULL status.
	 */
	std::vector<SourceBundle>::reverse_iterator source_iter = sources_.rbegin();
	for ( ; source_iter != sources_.rend(); source_iter++) {
		status_ = source_iter->reader->GetDetectorStatus(time);
		if (status_) {
			status_source_ = source_iter->reader;
			break;
		}
	}
	
	return status_;
}

I3CalibrationConstPtr
I3MuxingReaderService::GetCalibration(I3Time time)
{
	/* Return the cached calibration if current */
	if (calibration_source_ && calibration_ == calibration_source_->GetCalibration(time))
		return calibration_;
	
	/* 
	 * Query the sources in reverse order, picking the first one that returns
	 * a non-NULL calibration.
	 */
	std::vector<SourceBundle>::reverse_iterator source_iter = sources_.rbegin();
	for ( ; source_iter != sources_.rend(); source_iter++) {
		calibration_ = source_iter->reader->GetCalibration(time);
		if (calibration_) {
			calibration_source_ = source_iter->reader;
			break;
		}
	}
	
	return calibration_;
}

I3GeometryConstPtr
I3MuxingReaderService::GetGeometry(I3Time time)
{
	/* Return the cached geometry if current */
	if (geometry_source_ && geometry_ == geometry_source_->GetGeometry(time))
		return geometry_;
	
	/* 
	 * Query the sources in reverse order, picking the first one that returns
	 * a non-NULL geometry.
	 */
	std::vector<SourceBundle>::reverse_iterator source_iter = sources_.rbegin();
	for ( ; source_iter != sources_.rend(); source_iter++) {
		geometry_ = source_iter->reader->GetGeometry(time);
		if (geometry_) {
			geometry_source_ = source_iter->reader;
			break;
		}
	}
	
	return geometry_;
}