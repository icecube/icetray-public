#include <I3Test.h>
#include "icetray/I3TrayHeaders.h"
#include "icetray/I3Tray.h"
#include "TestServiceFactory.h"
#include "TestServiceFactoryModule.h"

TEST_GROUP(ConfigureService);

  
TEST(one)
{
  I3Tray tray;
  tray.AddService("TestServiceFactory", "serv1");

  tray.SetParameter("serv1","intParam",(int)42);
  tray.SetParameter("serv1","boolParam",(bool)true);
  tray.SetParameter("serv1","stringParam","foobar");
  tray.SetParameter("serv1","floatParam",(float)3.14159);
  tray.SetParameter("serv1","doubleParam",(double)0.10181978);
  tray.SetParameter("serv1","longParam",(long)12345678);

  tray.AddModule("TestServiceFactoryModule","mod1");

  ENSURE(TestServiceFactory::instance);

  tray.Execute(1);

  ENSURE(TestServiceFactory::instance);
  ENSURE(TestServiceFactory::instance->configured);

  ENSURE_EQUAL(TestServiceFactory::instance->intParam, 42);
  ENSURE_EQUAL(TestServiceFactory::instance->boolParam, true);
  ENSURE_EQUAL(TestServiceFactory::instance->stringParam, "foobar");
  ENSURE_DISTANCE(TestServiceFactory::instance->floatParam,
		  3.14159f,
		  0.001f);
  ENSURE_DISTANCE(TestServiceFactory::instance->doubleParam,
		  0.10181978,
		  0.001);
  ENSURE_EQUAL(TestServiceFactory::instance->longParam, 12345678);
}

TEST(configure_service_after_module_added)
{
  // this is OK.  Nothing actually gets created until Execute is called.
  I3Tray tray;
  tray.AddService<TestServiceFactory>("serv1");
  tray.AddModule<TestServiceFactoryModule>("mod1");

  tray.SetParameter("serv1","intParam",(int)42);
  tray.Execute(1);
}

TEST(finish_called)
{
  I3Tray tray;
  tray.AddService<TestServiceFactory>("serv1");
  tray.AddModule("BottomlessSource", "mod1");

  ENSURE(TestServiceFactory::instance, "TestServiceFactory shoud exist now");

  tray.Execute(1);

  ENSURE(TestServiceFactory::instance->finishCalled, "Finish should have been called");    
}


