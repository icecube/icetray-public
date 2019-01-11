#include <I3Test.h>

TEST_GROUP(I3ConditionalModule);

#include <icetray/I3Tray.h>
#include <icetray/I3Bool.h>
#include <icetray/I3IcePick.h>
#include <icetray/I3IcePickInstaller.h>
#include <icetray/I3ConditionalModule.h>

#include <string>

using namespace std;

class TestConditionalPutModule : public I3ConditionalModule
{
public:
  TestConditionalPutModule(const I3Context& context) :
    I3ConditionalModule(context)
  {
    AddOutBox("OutBox");
  }

  void Configure() { }

  void Physics(I3FramePtr frame)
  {
    frame->Put("fired", I3FrameObjectPtr());
    PushFrame(frame,"OutBox");
  }
};
I3_MODULE(TestConditionalPutModule);

class DumbPick : public I3IcePick
{
public:
  DumbPick(const I3Context& context) :
    I3IcePick(context),
    key("")
  {
    AddParameter("CheckThisKeyInFrame",
		 "Key where there's a bool that says if the frame passes.",
		 key);
  }

  void Configure()
  {
    GetParameter("CheckThisKeyInFrame",key);
  }

  bool SelectFrame(I3Frame& frame)
  {
    return frame.Has(key);
  }

private:

  string key;
};
I3_SERVICE_FACTORY(I3IcePickInstaller<DumbPick>);

class TesterSource : public I3Module
{
public:
  TesterSource(const I3Context& context) :
    I3Module(context),
    called(0)
  {
    AddOutBox("OutBox");
  }

  void Process()
  {
    I3FramePtr frame(new I3Frame(I3Frame::Physics));

    if(called < 2) 
      frame->Put("lt_two",I3FrameObjectPtr());

    if(called >= 2) 
      frame->Put("ge_two",I3FrameObjectPtr());

    if(called % 2 == 0) 
      frame->Put("mod_two",I3FrameObjectPtr());

    if(called % 3 == 0) 
      frame->Put("mod_three",I3FrameObjectPtr());

    PushFrame(frame,"OutBox");
    ++called;
  }
private:
  int called;
};
I3_MODULE(TesterSource);

TEST(none)
{
  I3Tray tray;

  tray.AddModule("TesterSource","thesource");

  tray.AddModule("TestConditionalPutModule","dumbmodule")
    ("IcePickServiceKey","");

  tray.AddModule("CountObject", "lt_two_count")
    ("where", "lt_two")
    ("expected", 2);

  tray.AddModule("CountObject", "ge_two_count")
    ("where", "ge_two")
    ("expected", 8);

  tray.AddModule("CountObject", "mod_two_count")
    ("where", "mod_two")
    ("expected", 5);

  tray.AddModule("CountObject", "mod_three_count")
    ("where", "mod_three")
    ("expected", 4);

  tray.AddModule("TrashCan","thecan");

  tray.Execute(10);
  tray.Finish();
}

TEST(on_mod_two)
{
  I3Tray tray;

  tray.AddService("I3IcePickInstaller<DumbPick>","mod_two_pick")
    ("CacheResults", true)
    ("CheckThisKeyInFrame","mod_two");

  tray.AddModule("TesterSource","thesource");

  tray.AddModule("TestConditionalPutModule", "mod")
    ("IcePickServiceKey","mod_two_pick");

  tray.AddModule("CountObject", "co")
    ("where", "fired")
    ("expected", 5)
    ;

  tray.AddModule("CountObject", "co2")
    ("where", "mod_two")
    ("expected", 5)
    ;
				      
  tray.AddModule("CountObject", "co3")
    ("where", "mod_two_pick_cache")
    ("expected", 10)
    ;
				      
  tray.AddModule("Dump", "dump");
  tray.AddModule("TrashCan", "thecan");

  tray.Execute(10);
  tray.Finish();
}

TEST(on_mod_three)
{
  I3Tray tray;

  tray.AddService("I3IcePickInstaller<DumbPick>","mod_three_pick")
    ("CheckThisKeyInFrame","mod_three");

  tray.AddModule("TesterSource","thesource");

  tray.AddModule("TestConditionalPutModule","dumbmodule")
    ("IcePickServiceKey","mod_three_pick");

  tray.AddModule("CountObject", "co")
    ("where", "fired")
    ("expected", 4)
    ;

  tray.AddModule("Dump", "dump");
  tray.AddModule("TrashCan","thecan");

  tray.Execute(10);
  tray.Finish();
}
