/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */
#include <icetray/open.h>
#include <dataio/I3ReaderService.h>
#include <dataclasses/physics/I3EventHeader.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/calibration/I3Calibration.h>
#include <dataclasses/status/I3DetectorStatus.h>

#include <icetray/I3TrayInfo.h>
#include <icetray/I3TrayInfoService.h>

#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <ostream>
#include <boost/iostreams/stream.hpp>

#include <fstream>

namespace io = boost::iostreams;
namespace dataio = I3::dataio;

I3ReaderService::I3ReaderService(const vector<string>& filenames,
				 const vector<string>& skip,
				 bool skip_missingDrivingTime) 
  : nframes_(0), 
    filenames_(filenames), 
    skip_(skip), 
    popmeta_done_(false), 
    skip_missingDrivingTime_(skip_missingDrivingTime),
    filenames_iter_(filenames_.begin())
{
  OpenNextFile();
}

I3ReaderService::~I3ReaderService() 
{ 
}

bool
I3ReaderService::MoreFiles()
{
  return filenames_iter_ != filenames_.end();
}

void
I3ReaderService::OpenNextFile()
{
  if (!ifs_.empty())
    ifs_.pop();
  ifs_.reset();
  assert(ifs_.empty());

  string filename = *filenames_iter_;
  filenames_iter_++;

  dataio::open(ifs_, filename);
  log_trace("Constructing with filename %s, %zu regexes", 
	    filename.c_str(), skip_.size());

  log_info("Opened file %s", filename.c_str());
}

I3FramePtr
I3ReaderService::PopFrame()
{
  while (true)
    {
      if (ifs_.eof())
	return I3FramePtr();

      I3FramePtr tmpframe(new I3Frame);

      bool r = tmpframe->load(ifs_, skip_);

      if (!r)
	if (ifs_.eof())
	  if (MoreFiles())
	    OpenNextFile();
	  else
	    return I3FramePtr();
	else
	  log_fatal("eh?  how did we get a null frame from load without either an eof or throw?");
      else
	return tmpframe;
    }
}	

//
//  if it is metadata stream at the front, return it.  Otherwise cache
//  up whatever is there.  geo/cal/status is just an assignment, the
//  event is a little copy shuck-and-jive.
//
I3FramePtr 
I3ReaderService::PopMeta()
{
  log_trace("%s", __PRETTY_FUNCTION__);
  popmeta_done_ = true;
  //
  // when we find a next_event_ we drop out of this loop.
  //
  while (! next_event_)
    {
      log_trace("! next_event_");
      I3FramePtr tmpframe = PopFrame();
      if (!tmpframe)
	return tmpframe;

      // if it's a slow stream, cache it
      if (tmpframe->GetStop() == I3Frame::Geometry)
	{
	  log_trace("MoreEvents updating I3Geometry");
	  geometry_ = tmpframe->Get<I3GeometryConstPtr>();
	}
      else if (tmpframe->GetStop() == I3Frame::Calibration)
	{
	  log_trace("MoreEvents updating calibration");
	  calibration_ = tmpframe->Get<I3CalibrationConstPtr>();
	}
      else if (tmpframe->GetStop() == I3Frame::DetectorStatus)
	{
	  log_trace("MoreEvents updating detector status");
	  status_ = tmpframe->Get<I3DetectorStatusConstPtr>();
	}      
      //x if it's physics, cache it in next_event_, which triggers
      // exit from this loop and return of "no more meta"
      else if (tmpframe->GetStop() == I3Frame::Physics)
	{
	  bool read_this_event = true;
	  if(skip_missingDrivingTime_)
	    {
	      if( ! tmpframe->Has("DrivingTime")) 
		read_this_event = false;
	    }
	  if(read_this_event)
	    {
	      next_event_ = I3FramePtr(new I3Frame(I3Frame::Physics));
	      for(I3Frame::typename_iterator iter = tmpframe->typename_begin();
		  iter != tmpframe->typename_end();
		  iter++)
		{
		  next_event_->take(*tmpframe, iter->first);
		}
	      log_trace("Set next event with %zu items", next_event_->size());
	    }
	}
      else // well, it's something else.
	{
	  return tmpframe;
	}
    }
  // there were no more metas and an event is queued (or not)
  return I3FramePtr();
}

bool
I3ReaderService::MoreEvents()
{
  // if that's null, we're at EOF or other run-stopping archive error.
  return next_event_;
}

// PopEvent is responsible for keeping the cached other streams 
// current
I3Time 
I3ReaderService::PopEvent(I3Frame& frame)
{
  if (!popmeta_done_)
    log_fatal("You're trying to pop an event before doing PopMeta");
  popmeta_done_ = false;

  for(I3Frame::typename_iterator iter = next_event_->typename_begin();
      iter != next_event_->typename_end();
      iter++)
    {
      // DrivingTime needs cleanup
      if (iter->first != "DrivingTime")
	frame.take(*next_event_, iter->first);
    }
  log_trace("Popping event with %zu items", frame.size());
  I3Time thetime = next_event_->Get<I3Time>("DrivingTime");
  // this has to be cleared out so PopMeta will move us to the next event.
  next_event_.reset();
  return thetime;
}

I3DetectorStatusConstPtr
I3ReaderService::GetDetectorStatus (I3Time t)
{
  return status_;
}

I3CalibrationConstPtr
I3ReaderService::GetCalibration (I3Time t)
{ 
  return calibration_;
}

I3GeometryConstPtr
I3ReaderService::GetGeometry (I3Time t)
{
  return geometry_;
}
