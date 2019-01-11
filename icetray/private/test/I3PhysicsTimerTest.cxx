#include <I3Test.h>
#include <icetray/I3Units.h>
#include <icetray/serialization.h>
#include <icetray/modules/FrameCheck.h>
#include <icetray/modules/Dump.h>

#include <icetray/I3PhysicsTimer.h>
#include <icetray/I3TrayHeaders.h>
#include <icetray/I3Tray.h>
#include <icetray/Utility.h>

TEST_GROUP(I3PhysicsTimer);

void 
do_sleep(I3FramePtr frame, const std::string name, unsigned sec, unsigned usec)
{
  I3PhysicsTimer timer(frame, name);
  sleep(sec);
  usleep(usec);
  usleep(15);
}

void timed_sleep(I3FramePtr frame, const std::string name, unsigned sec, unsigned usec)
{
  do_sleep(frame, name, sec, usec);

  const I3RUsage& rusage = frame->Get<I3RUsage>(name + "_rusage");

  double mintime = (double) sec * I3Units::second + (double) usec * I3Units::microsecond;
  ENSURE(rusage.wallclocktime >= mintime);

  // we can't be sure of the accuracy of this from host to host, it
  // depends on hardware.  Set a *really* sloppy tolerance.
  double maxtime = mintime + 0.5 * I3Units::second;
  ENSURE(rusage.wallclocktime <= maxtime);
  log_trace("%us %uus => %f", sec, usec, rusage.wallclocktime);

}


TEST(test_timer)
{
  I3FramePtr frame(new I3Frame(I3Frame::None));
  timed_sleep(frame, "7microseconds", 0, 7);
  timed_sleep(frame, "50microseconds", 0, 50);
  timed_sleep(frame, "471microseconds", 0, 471);
  timed_sleep(frame, "50000microseconds", 0, 50000);
  timed_sleep(frame, "450000microseconds", 0, 450000);
  timed_sleep(frame, "1second", 1, 0);
  timed_sleep(frame, "2.38seconds", 2, 380000);

  cout << *frame << "\n";
}


struct TimedModule : I3Module
{
  TimedModule(const I3Context& context) : I3Module(context) 
  { 
    AddOutBox("OutBox");
  }

  void Physics(I3FramePtr frame)
  {
    I3PhysicsTimer t(frame, context_.Get<I3Configuration>().InstanceName());

    // do expensie timed physics
    sleep (3);

    PushFrame(frame, "OutBox");
  }
};

I3_MODULE(TimedModule);


TEST(test_timed_module)
{

  I3Tray tray;

  tray.AddModule("BottomlessSource", "source");
  tray.AddModule<TimedModule>("timer");
  tray.AddModule("Dump", "dump");

  tray.AddModule<FrameCheck>("framecheck")
    ("ensure_physics_has", to_vector("timer_rusage"));


  tray.AddModule("TrashCan", "trash");

  tray.Execute(2);

  tray.Finish();

}


