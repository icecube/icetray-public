#include "phys-services/source/I3GCDFileService.h"
#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/calibration/I3Calibration.h"
#include "dataclasses/status/I3DetectorStatus.h"
#include "dataclasses/I3Time.h"
#include "icetray/I3Frame.h"
#include <iostream>
#include <fstream>
#include <icetray/open.h>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/gzip.hpp>

template <class T>
T
GetFromFile(I3Frame::Stream stop, std::string filename){
  T null_ptr;

  boost::iostreams::filtering_istream ifs;
  I3::dataio::open(ifs,filename);
  if(ifs.peek() == EOF) return null_ptr;

  while(ifs.peek() != EOF){
    I3FramePtr frame(new I3Frame);
    try { frame->load(ifs); }
    catch (const std::exception &e) {
      log_fatal("Error reading %s : %s",
		filename.c_str(), e.what());
      return null_ptr;
    }
    if( frame && frame->GetStop() == stop){
      return frame->Get<T>();
    }
  }
  return null_ptr;
}

I3GeometryConstPtr
I3GCDFileGeometryService::GetGeometry(I3Time t){
  if(!geo_) 
    geo_ = GetFromFile<I3GeometryConstPtr>(I3Frame::Geometry,filename_);
  return geo_;
}


I3CalibrationConstPtr
I3GCDFileCalibrationService::GetCalibration(I3Time t){
  if(!cal_) 
    cal_ = GetFromFile<I3CalibrationConstPtr>(I3Frame::Calibration, filename_);
  return cal_;
}

I3DetectorStatusConstPtr
I3GCDFileDetectorStatusService::GetDetectorStatus(I3Time t){
  if(!stat_) 
    stat_ = GetFromFile<I3DetectorStatusConstPtr>(I3Frame::DetectorStatus, filename_);
  return stat_;
}

