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

#ifndef DATAIO_I3MUXINGREADERSERVICE_H_INCLUDED
#define DATAIO_I3MUXINGREADERSERVICE_H_INCLUDED

#include <icetray/IcetrayFwd.h>
#include <icetray/I3DefaultName.h>

#include <interfaces/I3EventService.h>
#include <interfaces/I3DetectorStatusService.h>
#include <interfaces/I3CalibrationService.h>
#include <interfaces/I3GeometryService.h>
#include <interfaces/I3MetaService.h>

#include "dataio/I3ReaderService.h"

#include <dataclasses/I3Time.h>

#include <boost/iostreams/filtering_stream.hpp>

class I3MuxingReaderService : public I3EventService,
    public I3DetectorStatusService,
    public I3CalibrationService,
    public I3GeometryService,
    public I3MetaService
{
	unsigned nframes_;
	
	I3FramePtr next_event_;
	I3Time next_time_;
	
	/* A copy of the frame injected into the module chain */
	I3FramePtr injected_event_;
	
	I3DetectorStatusConstPtr status_;
	I3GeometryConstPtr       geometry_;
	I3CalibrationConstPtr    calibration_;
	
	I3ReaderServicePtr status_source_, geometry_source_, calibration_source_;
	
	void QueueEvent();
	/* Merge keys without overwriting. */
	void MergeFrames(I3FramePtr master, I3FramePtr subframe);
		
	struct SourceBundle {
		I3ReaderServicePtr reader; /* A reader service attached to a file list. */
		I3FramePtr frame; /* The last physics frame read (if we overshot the canonical time) */
		I3Time time; /* The DrivingTime of the overshot frame */
	};
	
	std::vector<SourceBundle> sources_;
	
	SET_LOGGER("I3MuxingReaderService");

public:
	I3MuxingReaderService(const std::vector<std::vector<std::string> >& filenames, 
		  const std::vector<std::string>& keys_to_skip, bool no_driving_time);

	~I3MuxingReaderService();
	
	// I3MetaService
	I3FramePtr PopMeta();
	
	// I3EventService
	bool MoreEvents();
	
	I3Time PopEvent(I3Frame&);
	
	void DiffEvent(I3Frame&);
	bool DiffEventImplemented() { return true; }
	
	// I3DetectorStatusService
	I3DetectorStatusConstPtr GetDetectorStatus(I3Time);
	
	// I3CalibrationService
	I3CalibrationConstPtr    GetCalibration(I3Time);
	
	// I3GeometryService
	I3GeometryConstPtr       GetGeometry(I3Time);
};

I3_DEFAULT_NAME(I3MuxingReaderService);
I3_POINTER_TYPEDEFS(I3MuxingReaderService);

#endif /* end of include guard: DATAIO_I3MUXINGREADERSERVICE_H_INCLUDED */
