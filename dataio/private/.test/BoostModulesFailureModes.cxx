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

TEST_GROUP(ModulesFailureModes);

TEST(NoFileFailurePhysics)
{
  RootI3Tray tray;
        
  tray.AddModule("I3BoostPhysicsReader","physreader")
    ("Infile","ThisFileDNE.xml");
    
  try{
    tray.Execute();
    tray.Finish();
    FAIL("This test was supposed to throw an exception.");
  }catch(const std::exception&){
    //If an exception was thrown this test passed
  }
}

TEST(NoFileFailureCalibration)
{
  RootI3Tray tray;
        
  tray.AddModule("I3DefaultPhysicsSource","physsource");
  tray.AddModule("I3BoostCalibrationReader","calreader")
    ("Infile","ThisFileDNE.xml");

  try{
    tray.Execute();
    tray.Finish();
    FAIL("This test was supposed to throw an exception.");
  }catch(const std::exception&){
    //If an exception was thrown this test passed
  }
}

TEST(NoFileFailureGeometry)
{
  RootI3Tray tray;
        
  tray.AddModule("I3DefaultPhysicsSource","physsource");
  tray.AddModule("I3BoostGeometryReader","georeader")
    ("Infile","ThisFileDNE.xml");

  try{
    tray.Execute();
    tray.Finish();
    FAIL("This test was supposed to throw an exception.");
  }catch(const std::exception&){
    //If an exception was thrown this test passed
  }
}

TEST(NoFileFailureDetectorStatus)
{
  RootI3Tray tray;
        
  tray.AddModule("I3DefaultPhysicsSource","physsource");
  tray.AddModule("I3BoostDetectorStatusReader","detstatreader")
    ("Infile","ThisFileDNE.xml");

  try{
    tray.Execute();
    tray.Finish();
    FAIL("This test was supposed to throw an exception.");
  }catch(const std::exception&){
    //If an exception was thrown this test passed
  }
}

