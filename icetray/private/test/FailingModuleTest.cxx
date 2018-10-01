#include <I3Test.h>
#include "icetray/I3TrayHeaders.h"
#include "icetray/I3Tray.h"

TEST_GROUP(FailingModule);

namespace FailingModuleTest
{
  class FailingModule : public I3Module
  {
  public:
    FailingModule(const I3Context& context) : I3Module(context) 
    {}
    
    void Process(){
      log_fatal("Don't be fooled by this 'fatal' message.  This module is supposed to fail");
      // and so this function should never be called:
      FAIL("that should have thrown");
    }
  };
  

  TEST(verify_failure)
  {
    I3Tray tray;
    tray.AddModule("FailingModule");
    try
      {
	tray.Execute(1);
	FAIL("Failing module should have thrown");
      }
    catch(std::exception& e)
      {
	// good.  it threw. 
      }
  }
  I3_MODULE(FailingModule);
}

