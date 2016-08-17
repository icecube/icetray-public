#include <I3Test.h>

#include <icetray/I3Tray.h>
#include <icetray/I3Module.h>
#include <icetray/I3TrayHeaders.h>
#include <icetray/Utility.h>
#include "TestModule.h"
#include "TestServiceFactory.h"

#include <boost/assign/list_of.hpp>

using boost::assign::list_of;

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

TEST(convenience_interface_0)
{
  I3Tray tray;

  tray.AddService("TestServiceFactory", "service")
    ("boolParam", (bool)true)
    ("intParam", (int)52)
    ("doubleParam", (double)4.14159)
    ("stringParam", std::string("it puts the lotion in the basket"))
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
    ("stringParam", std::string("it puts the lotion in the basket"))
    ("longParam", (long)68);

  tray.AddModule<TestModule>("test")
    ("boolParam", false)
    ("intParam", (int)53)
    ("doubleParam", (double)3.14159)
    ("stringParam", std::string("it puts the lotion in the basket"))
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

  std::vector<std::string> params;
  params.push_back("OutBox");
  tray.AddModule("Fork", "fork")
    ("Outboxes", params);

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

  std::vector<std::string> params;

  tray.AddModule("BottomlessSource", "source");

  params.push_back("OutBox");
  params.push_back("BadBox");
  tray.AddModule("Fork", "fork")
    ("Outboxes", params);

  params.clear();
  params.push_back("OutBox");
  tray.AddModule("Fork", "fork2")
    ("Outboxes", params);
  tray.ConnectBoxes("source", "OutBox", "fork");
  tray.ConnectBoxes("fork", "OutBox", "fork2");

  try {
    tray.ConnectBoxes("fork", "BadBox", "NoSuchModule");
    FAIL("That should have thrown... attempt to connect Outbox to nonexistant module.");
  } catch(const std::exception& e) {  
    // ok
  }
}

TEST(multiple_tray_create_destroy)
{
  {
    I3Tray tray;
    tray.AddModule("BottomlessSource", "source");
    tray.AddModule("TrashCan", "trash");
    tray.Execute(1);
  }

  {
    I3Tray tray2;
    tray2.AddModule("BottomlessSource", "source");
    tray2.AddModule("TrashCan", "trash");
    tray2.Execute(1);
  }

}

//A module with a simple side effect (incrementing a counter)
//so that we can easily verify that it has been run.
//It also stops the containing tray by calling RequestSuspension. 
class SideEffectModule : public I3Module {
public:
  SideEffectModule(const I3Context& context) : I3Module(context) {
    AddOutBox("OutBox");
    AddParameter("Life", "Number of iterations this module should survive", 0);
  }
  
  void Configure() {
    GetParameter("Life",life);
  }
  
  void Process() {
    counter++;
    if(life){
      life--;
      if(!life)
        RequestSuspension();
    }
  }
  static unsigned int counter;
  unsigned int life;
};

I3_MODULE(SideEffectModule);
unsigned int SideEffectModule::counter=0;

TEST(simultaneous_trays)
{
  SideEffectModule::counter=0;
  I3Tray tray1, tray2;
  tray1.AddModule("SideEffectModule", "counter1")("Life", 5);
  tray2.AddModule("SideEffectModule", "counter2")("Life", 5);
  
  tray1.Execute();
  ENSURE_EQUAL(SideEffectModule::counter,5u);
  //after one tray has stopped due to a module requesting suspension
  //another should still be able to run
  tray2.Execute();
  ENSURE_EQUAL(SideEffectModule::counter,10u);
}

TEST(anonymous_module)
{
  I3Tray tray;
  tray.AddModule("BottomlessSource");
  tray.AddService("TestServiceFactory");
  tray.Execute(1);
}

namespace{
  void simple_void_function(boost::shared_ptr<I3Frame>){}
  bool simple_bool_function(boost::shared_ptr<I3Frame>){ return(true); }
}

TEST(functions_as_modules){
  I3Tray tray;
  tray.AddModule("BottomlessSource");
  //test using function pointers
  tray.AddModule(&simple_void_function);
  tray.AddModule(&simple_bool_function);
#if __cplusplus >= 201103L && BOOST_VERSION > 105500
  //test using lambdas if they exist
  tray.AddModule([](boost::shared_ptr<I3Frame>){});
  tray.AddModule([](boost::shared_ptr<I3Frame>){ return(true); });
#endif
  tray.Execute(1);
}
