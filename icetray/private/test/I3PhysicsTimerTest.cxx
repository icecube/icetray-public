#include <I3Test.h>
#include <icetray/I3Units.h>
#include <icetray/serialization.h>
#include <icetray/modules/FrameCheck.h>

#include <icetray/I3PhysicsTimer.h>
#include <icetray/I3TrayHeaders.h>
#include <icetray/I3Tray.h>
#include <icetray/Utility.h>

#include <sstream>
#include <iomanip>
#include <boost/assign/list_of.hpp>
using boost::assign::list_of;

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
  // on the mac, under load, wallclocktime can actually come up short.
  // so we build in a small tolerance
#ifdef __APPLE_CC__
  if (sec >= 2) {
    mintime = mintime - 50000;
  }
#endif
  std::stringstream s;
  s << "rusage.wallclocktime: " << std::setiosflags(std::ios::fixed) << rusage.wallclocktime << std::endl << "\t\t\t\t" << "mintime: " << mintime ;
  ENSURE(rusage.wallclocktime >= mintime, (const std::string) s.str());

  // we can't be sure of the accuracy of this from host to host, it
  // depends on hardware.  Set a *really* sloppy tolerance.
  double maxtime = mintime + 0.5 * I3Units::second;
  s.str(std::string());
  s << "rusage.wallclocktime: " << std::setiosflags(std::ios::fixed) << rusage.wallclocktime << std::endl << "\t\t\t\t" << "maxtime: " << maxtime ;
  ENSURE(rusage.wallclocktime <= maxtime, (const std::string) s.str());
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

  std::cout << *frame << "\n";
}


struct TimedModule : I3Module
{
  TimedModule(const I3Context& context) : I3Module(context) 
  { }

  void Physics(I3FramePtr frame)
  {
    I3PhysicsTimer t(frame, GetName());

    // do expensie timed physics
    sleep (3);

    PushFrame(frame);
  }
};

I3_MODULE(TimedModule);


TEST(test_timed_module)
{

  I3Tray tray;

  tray.AddModule("BottomlessSource");
  tray.AddModule<TimedModule>("timer");
  tray.AddModule("Dump");

  std::vector<std::string> params = list_of("timer_rusage");
  tray.AddModule<FrameCheck>("framecheck")
    ("ensure_physics_has", params);

  tray.Execute(2);
}


