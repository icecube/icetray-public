#include <icetray/I3Int.h>
#include <icetray/I3Tray.h>
#include <icetray/I3Frame.h>

#include <I3Test.h>

TEST_GROUP(PhysicsBuffering);

// Just checks for those sequential ints to be sure frames are in the
// correct order.
struct IntCheck : public I3Module
{
  int ctr_;
  
  IntCheck(const I3Context& context) : I3Module(context), ctr_(1) 
  { }

  void Physics(I3FramePtr frame)
  {
    const I3Int& data = frame->Get<I3Int>("myint");
    ENSURE_EQUAL(data.value, ctr_);
    if (data.value != ctr_)
      log_fatal("eh?");
    log_debug("Got %d", data.value);
    ctr_++;
    if (ctr_ % 4) log_trace("delete this.  trying to figure out what is wrong with dart.");
    PushFrame(frame);
  }
};
I3_MODULE(IntCheck);

// provoke another build
// Run checks with a 0 size buffer (a no-op) up to a buffer bigger
// than actual number of physics frames
#define DOTEST(NUMA, BATCHSIZEA, NUMB, BATCHSIZEB)	\
  TEST(Buffer_N ## NUMA ## _BATCH ## BATCHSIZEA ## _N ## NUMB ## _BATCH ## BATCHSIZEB) \
  {							\
    std::vector<std::string> physics_has;               \
    physics_has.push_back("myint");			\
    I3Tray tray;					\
    tray.AddModule("IntGenerator", "generator");	\
    tray.AddModule("IntCheck", "prebuffercheck");	\
    tray.AddModule("CountFrames", "prebuffercounter")	\
      ("physics", 1000);				\
    tray.AddModule("PhysicsBuffer","buffer1")		\
      ("buffersize", NUMA)				\
      ("batchpush", BATCHSIZEA);			\
    tray.AddModule("IntCheck", "postbufferchecka");	\
    tray.AddModule("PhysicsBuffer","buffer2")		\
      ("buffersize", NUMB)				\
      ("batchpush", BATCHSIZEB);			\
    tray.AddModule("IntCheck", "postbuffercheckb");	\
    tray.AddModule("CountFrames", "postbuffercounter")	\
      ("physics", 1000);				\
    			\
    tray.Execute(1000);					\
  }


DOTEST(0, 1, 0, 1);
DOTEST(1, 1, 10, 10);
DOTEST(10, 10, 10, 10);
DOTEST(49, 11, 57, 31);
DOTEST(57, 1, 13, 1);
DOTEST(999, 1, 1000, 1);
DOTEST(1000, 1000, 999, 999);
DOTEST(1000, 1000, 1000, 1000);
DOTEST(1001, 1000, 1001, 1000);
DOTEST(10000, 10000, 10000, 10000);



