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

#include "icetray/I3ServiceFactory.h"
#include "dataio/I3MuxingReaderService.h"

class I3MuxingReaderServiceFactory
: public I3ServiceFactory
{
 public:

  I3MuxingReaderServiceFactory(const I3Context& context);

  virtual ~I3MuxingReaderServiceFactory();

  bool InstallService(I3Context& services);

  void Configure();

 private:

  I3MuxingReaderServiceFactory (const I3MuxingReaderServiceFactory& rhs);
  I3MuxingReaderServiceFactory operator=(const I3MuxingReaderServiceFactory& rhs);

  std::string filename_;
  std::vector<std::vector<std::string> > filenames_;

  std::vector<std::string> skip_keys_;

  I3MuxingReaderServicePtr reader_;

  bool omitGeometry_;
  std::string installGeometryServiceAs_;

  bool omitCalibration_;
  std::string installCalibrationServiceAs_;

  bool omitStatus_;
  std::string installDetectorStatusServiceAs_;

  bool omitEvent_;
  std::string installEventServiceAs_;

  bool skip_missingDrivingTime_;

  SET_LOGGER("I3MuxingReaderServiceFactory");
};

I3_SERVICE_FACTORY(I3MuxingReaderServiceFactory);

#include <icetray/Utility.h>

I3MuxingReaderServiceFactory::I3MuxingReaderServiceFactory(const I3Context& context) 
  : I3ServiceFactory(context),
    omitGeometry_(false),
    omitCalibration_(false),
    omitStatus_(false),
    omitEvent_(false),
    skip_missingDrivingTime_(true)
{
  AddParameter("FilenameList", "list of lists of .i3 file name", filenames_);	
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

I3MuxingReaderServiceFactory::~I3MuxingReaderServiceFactory() { }

void 
I3MuxingReaderServiceFactory::Configure()
{

  GetParameter("FilenameList", filenames_);

#if 0
  log_info("FilenameList includes the following files:");
  std::vector<std::string>::iterator iter;
  for(iter=filenames_.begin(); iter!=filenames_.end(); iter++)
  {
    log_info("  %s", iter->c_str());
  }
#endif

  GetParameter("SkipKeys", skip_keys_);

  if(filenames_.empty())
    log_fatal("Input file names (\"FilenameList\") weren't set.");

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
I3MuxingReaderServiceFactory::InstallService(I3Context& services)
{

  bool good(true);
  if (!reader_)
    reader_ = I3MuxingReaderServicePtr(new I3MuxingReaderService(filenames_, skip_keys_, skip_missingDrivingTime_));

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
