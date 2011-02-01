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
#ifndef DATAIO_I3READERSERVICE_H_INCLUDED
#define DATAIO_I3READERSERVICE_H_INCLUDED

#include <icetray/IcetrayFwd.h>
#include <icetray/I3DefaultName.h>

#include <interfaces/I3EventService.h>
#include <interfaces/I3DetectorStatusService.h>
#include <interfaces/I3CalibrationService.h>
#include <interfaces/I3GeometryService.h>
#include <interfaces/I3MetaService.h>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/scoped_ptr.hpp>

#include <fstream>
#include <set>

class I3ReaderService : public I3EventService,
			public I3DetectorStatusService,
			public I3CalibrationService,
			public I3GeometryService,
			public I3MetaService
{

  unsigned nframes_;
  std::vector<std::string> filenames_;
  std::vector<std::string> skip_;

  I3FramePtr next_event_;
  bool popmeta_done_;
  bool skip_missingDrivingTime_;

  boost::iostreams::filtering_istream ifs_;

  I3DetectorStatusConstPtr status_;
  I3GeometryConstPtr       geometry_;
  I3CalibrationConstPtr    calibration_;
  I3FramePtr daqframe_;

  std::vector<std::string>::iterator filenames_iter_;

  bool MoreFiles();
  void OpenNextFile();
  I3FramePtr PopFrame();

  SET_LOGGER("I3ReaderService");

 public:

  I3ReaderService(const std::vector<std::string>& filenames, 
		  const std::vector<std::string>& keys_to_skip,
		  bool no_driving_time);

  ~I3ReaderService();

  // I3MetaService
  I3FramePtr PopMeta();

  // I3EventService
  bool MoreEvents();

  I3Time PopEvent(I3Frame&);

  // I3DetectorStatusService
  I3DetectorStatusConstPtr GetDetectorStatus(I3Time);

  // I3CalibrationService
  I3CalibrationConstPtr    GetCalibration(I3Time);

  // I3GeometryService
  I3GeometryConstPtr       GetGeometry(I3Time);

};

I3_DEFAULT_NAME(I3ReaderService);
I3_POINTER_TYPEDEFS(I3ReaderService);

#endif
