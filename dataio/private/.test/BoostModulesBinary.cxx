#include <I3Test.h>
#include "dataio/BoostHeaders.h"
#include "dataio/I3BoostIOFunctions.h"
#include "dataio/modules/I3BoostPhysicsReader.h"
#include "dataio/modules/I3BoostGeometryReader.h"
#include "dataio/modules/I3BoostCalibrationReader.h"
#include "dataio/modules/I3BoostDetectorStatusReader.h"
#include "dataio/modules/I3BoostPhysicsWriter.h"
#include "dataio/modules/I3BoostGeometryWriter.h"
#include "dataio/modules/I3BoostCalibrationWriter.h"
#include "dataio/modules/I3BoostDetectorStatusWriter.h"
#include "phys-services/source/I3DefaultPhysicsSource.h"
#include "phys-services/source/I3DefaultGeometrySource.h"
#include "phys-services/source/I3DefaultCalibrationSource.h"
#include "phys-services/source/I3DefaultDetectorStatusSource.h"
#include "icetray/root/RootI3Tray.h"
#include <fstream>

using boost::filesystem::path;
using boost::filesystem::directory_iterator;
using boost::filesystem::remove_all;

TEST_GROUP(BoostModulesBinary);

TEST(ReadAndWriteDefault)
{

  path p("boost_physics_files");
  if(exists(p)) remove_all(p);
  unlink("Geometry.binary");
  unlink("DetectorStatus.binary");
  unlink("Calibration.binary");
  {
    RootI3Tray tray;
    
    tray.AddModule("I3DefaultPhysicsSource","physsource")
      ("EventsToReturn",100);
    
    tray.AddModule("I3DefaultDetectorStatusSource","detstatsource");
    tray.AddModule("I3DefaultCalibrationSource","calsource");
    tray.AddModule("I3DefaultGeometrySource","geosource");
    
    tray.AddModule("I3BoostPhysicsWriter","physwriter")
      ("Directory",p.string().c_str())
      ("Outfile","Physics.binary");
    tray.AddModule("I3BoostGeometryWriter","geowriter");
    tray.AddModule("I3BoostCalibrationWriter","calwriter");
    tray.AddModule("I3BoostDetectorStatusWriter","detstatwriter");

    tray.Execute();
    tray.Finish();
  }
  {
    RootI3Tray tray;
        
    tray.AddModule("I3BoostPhysicsReader","physreader")
      ("Infile","boost_physics_files/Physics.binary");
    tray.AddModule("I3BoostDetectorStatusReader","detstatreader");
    tray.AddModule("I3BoostCalibrationReader","calreader");
    tray.AddModule("I3BoostGeometryReader","georeader");

    tray.Execute();
    tray.Finish();

  }
  if(exists(p)) remove_all(p);
  unlink("Geometry.binary");
  unlink("DetectorStatus.binary");
  unlink("Calibration.binary");
}

TEST(ReadAndWriteToDir)
{
  path p("boost_physics_files");
  if(exists(p)) remove_all(p);
  unlink("Geometry.binary");
  unlink("DetectorStatus.binary");
  unlink("Calibration.binary");
  {
    RootI3Tray tray;
    
    tray.AddModule("I3DefaultPhysicsSource","physsource")
      ("EventsToReturn",100);
    
    tray.AddModule("I3DefaultDetectorStatusSource","detstatsource");
    tray.AddModule("I3DefaultCalibrationSource","calsource");
    tray.AddModule("I3DefaultGeometrySource","geosource");
    
    tray.AddModule("I3BoostPhysicsWriter","physwriter")
      ("Directory",p.string().c_str());
    tray.AddModule("I3BoostGeometryWriter","geowriter");
    tray.AddModule("I3BoostCalibrationWriter","calwriter");
    tray.AddModule("I3BoostDetectorStatusWriter","detstatwriter");

    tray.Execute();
    tray.Finish();
  }
  {
    RootI3Tray tray;
        
    tray.AddModule("I3BoostPhysicsReader","physreader")
      ("Infile",p.string().c_str());
    tray.AddModule("I3BoostDetectorStatusReader","detstatreader");
    tray.AddModule("I3BoostCalibrationReader","calreader");
    tray.AddModule("I3BoostGeometryReader","georeader");

    tray.Execute();
    tray.Finish();

  }
  if(exists(p)) remove_all(p);
  unlink("Geometry.binary");
  unlink("DetectorStatus.binary");
  unlink("Calibration.binary");
}

TEST(ReadAndWriteMultiple)
{
  path p("boost_physics_files");
  if(exists(p)) remove_all(p);
  {
    RootI3Tray tray;
    
    tray.AddModule("I3DefaultPhysicsSource","physsource")
      ("EventsToReturn",100);
    tray.AddModule("I3BoostPhysicsWriter","physwriter")
      ("Outfile","Physics.binary")
      ("Directory",p.string().c_str())
      ("NEventsPerFile",10);
    tray.Execute();
    tray.Finish();
  }
  {
    RootI3Tray tray;
        
    tray.AddModule("I3BoostPhysicsReader","physreader")
      ("Infile",p.string().c_str());

    tray.Execute();
    tray.Finish();
  }
  if(exists(p)) remove_all(p);
  unlink("Geometry.binary");
  unlink("DetectorStatus.binary");
  unlink("Calibration.binary");
}

TEST(MaxFileSize)
{
  path p("boost_physics_files");
  if(exists(p)) remove_all(p);
  {
    RootI3Tray tray;
    
    tray.AddModule("I3DefaultPhysicsSource","physsource")
      ("EventsToReturn",100);
    tray.AddModule("I3BoostPhysicsWriter","physwriter")
      ("Directory",p.string().c_str())
      ("Outfile","Physics.binary")
      ("MaxFileSize",15000);
    tray.Execute();
    tray.Finish();
  }
  {
    RootI3Tray tray;
        
    tray.AddModule("I3BoostPhysicsReader","physreader")
      ("Infile",p.string().c_str());

    tray.Execute();
    tray.Finish();
  }
  if(exists(p)) remove_all(p);
  unlink("Geometry.binary");
  unlink("DetectorStatus.binary");
  unlink("Calibration.binary");
}

TEST(Filelist)
{
  const string filelist_dir("filelist_test");
  path file_list("files_to_read.list");

  path p(filelist_dir);
  if(exists(p)) remove_all(p);
  {
    RootI3Tray tray;
    
    tray.AddModule("I3DefaultPhysicsSource","physsource")
      ("EventsToReturn",100);
    tray.AddModule("I3BoostPhysicsWriter","physwriter")
      ("Directory",filelist_dir.c_str())
      ("Outfile","Physics.binary")
      ("MaxFileSize",15000);
    tray.Execute();
    tray.Finish();
  }
  {
    RootI3Tray tray;

    //const string filename("files_to_read.list");
    directory_iterator dir_end;
    ofstream filelist(file_list.string().c_str());
    for(directory_iterator iter(p); iter != dir_end; iter++){
      boost_io::archive ar = boost_io_functions::filename_to_archive(iter->string());
      if(ar != boost_io::invalid)
	filelist<<iter->string()<<std::endl;
    }
    filelist.close();
    tray.AddModule("I3BoostPhysicsReader","physreader")
      ("Infile",file_list.string().c_str());

    tray.Execute();
    tray.Finish();
  }
  if(exists(p)) remove_all(p);
  if(exists(file_list)) remove(file_list);
  unlink("Geometry.binary");
  unlink("DetectorStatus.binary");
  unlink("Calibration.binary");
}
