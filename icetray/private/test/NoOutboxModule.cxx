#include <I3Test.h>
#include "icetray/I3TrayHeaders.h"
#include "icetray/I3Tray.h"

TEST_GROUP(ModuleMustAddOutbox);

class NoOutboxModule : public I3Module
{
public:
  NoOutboxModule(const I3Context& context) : I3Module(context) 
  {
  }
    
  void Configure(){}
    
  void Process() {
    I3FramePtr f(new I3Frame(I3Frame::Physics));
    PushFrame(f, "OutBox");
  }
};
  
TEST(verify_failure)
{
  I3Tray tray;
  tray.AddModule("NoOutboxModule", "test");
  tray.AddModule("NoOutboxModule", "test2");
  tray.AddModule("TrashCan", "trash");
  try
    {
      tray.Execute(1);
      FAIL("Should have thrown");
    }
  catch(std::exception& e)
    {
      // good.  it threw. 
    }
}


I3_MODULE(NoOutboxModule);
