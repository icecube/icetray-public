#include <I3Test.h>
#include "dataio/BoostHeaders.h"
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

const int EVENTS_TO_RETURN = 1000;

using boost::filesystem::path;
using boost::filesystem::directory_iterator;
using boost::filesystem::remove_all;

TEST_GROUP(BoostModulesXML);

TEST(ReadAndWrite)
{

  path p("xml_boost_files");
  if(exists(p)) remove_all(p);
  unlink("Geometry.xml");
  unlink("DetectorStatus.xml");
  unlink("Calibration.xml");
  {
    RootI3Tray tray;
    
    tray.AddModule("I3DefaultPhysicsSource","physsource")
      ("EventsToReturn",EVENTS_TO_RETURN);

    tray.AddModule("I3DefaultDetectorStatusSource","detstatsource");
    tray.AddModule("I3DefaultCalibrationSource","calsource");
    tray.AddModule("I3DefaultGeometrySource","geosource");
    
    tray.AddModule("I3BoostPhysicsWriter","physwriter")
      ("Directory",p.string().c_str())
      ("Outfile","Physics.xml");

    tray.AddModule("I3BoostGeometryWriter","geowriter")
      ("Outfile","Geometry.xml");

    tray.AddModule("I3BoostCalibrationWriter","calwriter")
      ("Outfile","Calibration.xml");

    tray.AddModule("I3BoostDetectorStatusWriter","detstatwriter")
      ("Outfile","DetectorStatus.xml");

    tray.Execute();
    tray.Finish();
  }
  {
    RootI3Tray tray;

    string filename = p.string() + "/Physics.xml";
        
    tray.AddModule("I3BoostPhysicsReader","physreader")
      ("Infile",filename.c_str());

    tray.AddModule("I3BoostDetectorStatusReader","detstatreader")
      ("Infile","DetectorStatus.xml");

    tray.AddModule("I3BoostCalibrationReader","calreader")
      ("Infile","Calibration.xml");

    tray.AddModule("I3BoostGeometryReader","georeader")
      ("Infile","Geometry.xml");

    tray.Execute();
    tray.Finish();
  }
  if(exists(p)) remove_all(p);
  unlink("Geometry.xml");
  unlink("DetectorStatus.xml");
  unlink("Calibration.xml");
}

TEST(ReadAndWriteMultipleFiles)
{
  path p("xml_boost_files");
  if(exists(p)) remove_all(p);
  {
    RootI3Tray tray;
    
    tray.AddModule("I3DefaultPhysicsSource","physsource")
      ("EventsToReturn",EVENTS_TO_RETURN);
    tray.AddModule("I3BoostPhysicsWriter","physwriter")
      ("Outfile","Physics.xml")
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
  unlink("Geometry.xml");
  unlink("DetectorStatus.xml");
  unlink("Calibration.xml");
}

TEST(MaxFileSize)
{
  path p("xml_boost_files");
  if(exists(p)) remove_all(p);
  {
    RootI3Tray tray;
    
    tray.AddModule("I3DefaultPhysicsSource","physsource")
      ("EventsToReturn",EVENTS_TO_RETURN);
    tray.AddModule("I3BoostPhysicsWriter","physwriter")
      ("Outfile","Physics.xml")
      ("Directory",p.string().c_str())
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
  unlink("Geometry.xml");
  unlink("DetectorStatus.xml");
  unlink("Calibration.xml");
}
