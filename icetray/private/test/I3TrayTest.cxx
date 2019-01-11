#include <I3Test.h>

#include <icetray/I3Tray.h>
#include <icetray/I3Module.h>
#include <icetray/I3TrayHeaders.h>
#include <icetray/Utility.h>
#include "TestModule.h"
#include "TestServiceFactory.h"



TEST_GROUP(I3TrayTest);

TEST(0)
{
  ENSURE(!TestModule::module);
}

TEST(normal_interface)
{
  I3Tray tray;
  tray.AddModule<TestModule>("test");
  tray.AddModule("TrashCan", "trash");

  tray.SetParameter("test","boolParam", true);
  tray.SetParameter("test","intParam", 54);
  tray.SetParameter("test","stringParam", "foobar");
  tray.SetParameter("test","longParam", 67);
  tray.SetParameter("test","doubleParam", 3.14159);

  tray.Execute(0);

  ENSURE(TestModule::module);

  ENSURE(TestModule::module->boolParam == true);
  ENSURE(TestModule::module->intParam == 54);
  ENSURE(TestModule::module->stringParam == "foobar");
  ENSURE(TestModule::module->longParam == 67);
  ENSURE_DISTANCE(TestModule::module->doubleParam,
		  (double)3.14159,
		  0.0000001);
    
}

#if 0
//#warning put this error checking back.  For now we do without it.
T_EST(last_module_has_disconnected_outbox_0)
{
  I3Tray tray;
  tray.AddModule<TestModule>("test1");
  tray.AddModule<TestModule>("test2");

  try {
    tray.Execute(0);
    FAIL("that should have complained about a disconnected outbox");
  } catch (const std::exception& e) { }
}


T_EST(last_module_has_disconnected_outbox_1)
{
  I3Tray tray;
  tray.AddModule("TestModule","test1");
  tray.AddModule("TestModule","test2");

  try {
    tray.Execute(0);
    FAIL("that should have complained about a disconnected outbox");
  } catch (const std::exception& e) { }
}
#endif

TEST(convenience_interface_0)
{
  I3Tray tray;

  tray.AddService("TestServiceFactory", "service")
    ("boolParam", (bool)true)
    ("intParam", (int)52)
    ("doubleParam", (double)4.14159)
    ("stringParam", string("it puts the lotion in the basket"))
    ("longParam", (long)68);

  tray.AddModule("TestModule", "test")
    ("boolParam", false)
    ("intParam", (int)53)
    ("doubleParam", (double)3.14159)
    ("stringParam", "it puts the lotion in the basket")
    ("longParam", (long)67);
  tray.AddModule("TrashCan", "trash");

  tray.Execute(0);

  ENSURE_EQUAL(TestServiceFactory::boolParam, true);
  ENSURE_EQUAL(TestServiceFactory::intParam, 52);
  ENSURE(TestServiceFactory::stringParam == "it puts the lotion in the basket");
  ENSURE(TestServiceFactory::longParam == 68);
  ENSURE_DISTANCE(TestServiceFactory::doubleParam,
		  (double)4.14159,
		  0.0000001);

  ENSURE(TestModule::module);

  ENSURE(TestModule::module->boolParam == false);
  ENSURE(TestModule::module->intParam == 53);
  ENSURE(TestModule::module->stringParam == "it puts the lotion in the basket");
  ENSURE(TestModule::module->longParam == 67);
  ENSURE_DISTANCE(TestModule::module->doubleParam,
		  (double)3.14159,
		  0.0000001);
}

TEST(convenience_interface_1)
{
  I3Tray tray;

  tray.AddService<TestServiceFactory>("service")
    ("intParam", (int)41);

  tray.AddModule<TestModule>("test")
    ("boolParam", false)
    ("intParam", (int)99);
  tray.AddModule("TrashCan", "trash");

  tray.Execute(0);

  ENSURE_EQUAL(TestServiceFactory::intParam, 41);
  ENSURE(TestModule::module);
  ENSURE(TestModule::module->intParam == 99);
}

TEST(default_convenience_connectboxes)
{
  I3Tray tray;

  tray.AddService<TestServiceFactory>("service")
    ("boolParam", (bool)true)
    ("intParam", (int)52)
    ("doubleParam", (double)4.14159)
    ("stringParam", string("it puts the lotion in the basket"))
    ("longParam", (long)68);

  tray.AddModule<TestModule>("test")
    ("boolParam", false)
    ("intParam", (int)53)
    ("doubleParam", (double)3.14159)
    ("stringParam", string("it puts the lotion in the basket"))
    ("longParam", (long)67);

  tray.AddModule<TestModule>("test2")("boolParam", true);
  tray.AddModule<TestModule>("test3")("boolParam", true);
  tray.AddModule<TestModule>("test4")("boolParam", true);
  tray.AddModule<TestModule>("test5")("boolParam", true);

  tray.AddModule("TrashCan", "trash");

  tray.Execute(0);
}

TEST(missing_module_fails_correctly)
{
  I3Tray tray;

  tray.AddModule("BottomlessSource", "source");
  tray.AddModule("Fork", "fork")
    ("Outboxes", to_vector("OutBox"));

  tray.ConnectBoxes("source", "OutBox", "fork");

  // up until the icetray-idlib module, this was a fail at Execute(0)
  // time, not at Execute(1) time.  Tray sanity-checking needs
  // improvement.
  try {
    tray.Execute(5);
    FAIL("That should have thrown... attempt to connect Outbox to nothing");
  } catch(const std::exception& e) {  
    // ok
  }
}

TEST(no_such_module)
{
  I3Tray tray;

  tray.AddModule("BottomlessSource", "source");
  tray.AddModule("Fork", "fork")
    ("Outboxes", to_vector("OutBox", "BadBox"));
  tray.AddModule("Fork", "fork2")
    ("Outboxes", to_vector("OutBox"));
  tray.ConnectBoxes("source", "OutBox", "fork");
  tray.ConnectBoxes("fork", "OutBox", "fork2");
  tray.ConnectBoxes("fork", "BadBox", "NoSuchModule");

  try {
    tray.Execute(0);
    FAIL("That should have thrown... attempt to connect Outbox to nonexistant module.");
  } catch(const std::exception& e) {  
    // ok
  }
}

#if 0
T_EST(no_such_box)
{
  I3Tray tray;

  tray.AddModule("BottomlessSource", "source");
  tray.AddModule("TrashCan", "trash");
  tray.AddModule("TrashCan", "trash2");

  tray.ConnectBoxes("source", "OutBox", "trash");
  tray.ConnectBoxes("source", "nosuchbox", "trash2");

  try {
    tray.Execute(0);
    FAIL("That should have thrown.");
  } catch(const std::exception& e) {  
    // ok
  }
}
#endif
