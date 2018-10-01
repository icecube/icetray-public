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
#include "dataio/I3ReaderServiceFactory.h"

I3_SERVICE_FACTORY(I3ReaderServiceFactory);

#include "dataio/I3ReaderService.h"

#include <icetray/Utility.h>

I3ReaderServiceFactory::I3ReaderServiceFactory(const I3Context& context) 
  : I3ServiceFactory(context),
    omitGeometry_(false),
    omitCalibration_(false),
    omitStatus_(false),
    omitEvent_(false),
    skip_missingDrivingTime_(false)
{
  AddParameter("Filename", ".i3 file name", filename_);	
  AddParameter("FilenameList", "list of .i3 file name", filenames_);	
  AddParameter("SkipKeys", 
	       "Vector of regexes: if any one matches the key, don't load",
	       skip_keys_);

  AddParameter("SkipMissingDrivingTime",
	       "Skip frames from i3 files that do not contain DrivingTime"
	       " (mostly I3DST entries from Pole.)",
	       skip_missingDrivingTime_);

  AddParameter("OmitGeometry",
	       "Don't install the geometry service",
	       omitGeometry_);

  installGeometryServiceAs_ = I3DefaultName<I3GeometryService>::value();
  AddParameter("InstallGeometryServiceAs",
	       "Install the geometry service at the following location"
	       "(Default value is the value according to I3_DEFAULT_NAME)",
	       installGeometryServiceAs_);

  AddParameter("OmitCalibration",
	       "Don't install the calib service",
	       omitCalibration_);

  installCalibrationServiceAs_ = I3DefaultName<I3CalibrationService>::value();
  AddParameter("InstallCalibrationServiceAs",
	       "Install the calibration service at the following location"
	       "(Default value is the value according to I3_DEFAULT_NAME)",
	       installCalibrationServiceAs_);

  AddParameter("OmitStatus",
	       "Don't install the status service",
	       omitStatus_);

  installDetectorStatusServiceAs_ = I3DefaultName<I3DetectorStatusService>::value();
  AddParameter("InstallDetectorStatusServiceAs",
	       "Install the detectorstatus service at the following location"
	       "(Default value is the value according to I3_DEFAULT_NAME)",
	       installDetectorStatusServiceAs_);

  AddParameter("OmitEvent",
	       "Don't install the event service",
	       omitEvent_);

  installEventServiceAs_ = I3DefaultName<I3EventService>::value();
  AddParameter("InstallEventServiceAs",
	       "Install the event service at the following location"
	       "(Default value is the value according to I3_DEFAULT_NAME)",
	       installEventServiceAs_);

}

I3ReaderServiceFactory::~I3ReaderServiceFactory() { }

void 
I3ReaderServiceFactory::Configure()
{

  GetParameter("Filename", filename_);

  GetParameter("FilenameList", filenames_);

  if(!filename_.empty() && !filenames_.empty())
  {
    log_fatal("Ambiguous:  Both parameter FileName (string) and FilenameList (vector of string) were specified.  Use one other the other.");
  }

  if (!filename_.empty())
    filenames_.push_back(filename_);

  log_info("FilenameList includes the following files:");
  std::vector<std::string>::iterator iter;
  for(iter=filenames_.begin(); iter!=filenames_.end(); iter++)
  {
    log_info("  %s", iter->c_str());
  }

  GetParameter("SkipKeys", skip_keys_);

  if(filenames_.empty())
    log_fatal("Input file names (\"Filename\" or \"FilenameList\") weren't set.");

  GetParameter("OmitGeometry", omitGeometry_);
  GetParameter("InstallGeometryServiceAs",installGeometryServiceAs_);

  GetParameter("OmitCalibration", omitCalibration_);
  GetParameter("InstallCalibrationServiceAs",installCalibrationServiceAs_);

  GetParameter("OmitStatus", omitStatus_);
  GetParameter("InstallDetectorStatusServiceAs",installDetectorStatusServiceAs_);

  GetParameter("OmitEvent", omitEvent_);
  GetParameter("InstallEventServiceAs",installEventServiceAs_);

  GetParameter("SkipMissingDrivingTime",skip_missingDrivingTime_);
}

bool
I3ReaderServiceFactory::InstallService(I3Context& services)
{

  bool good(true);
  if (!reader_)
    reader_ = I3ReaderServicePtr(new I3ReaderService(filenames_, skip_keys_, skip_missingDrivingTime_));

  if(!omitGeometry_)    
    good = good && services.Put<I3GeometryService>(installGeometryServiceAs_, reader_);
  
  if(!omitCalibration_)
    good = good && services.Put<I3CalibrationService>(installCalibrationServiceAs_, reader_);

  if(!omitStatus_)
    good = good && services.Put<I3DetectorStatusService>(installDetectorStatusServiceAs_, reader_);
  
  if(!omitEvent_)
    good = good && services.Put<I3EventService>(installEventServiceAs_, reader_);

  good = good && services.Put<I3MetaService>(reader_);

  return good;

}
