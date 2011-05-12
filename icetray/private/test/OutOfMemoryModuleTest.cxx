#include <I3Test.h>
#include "icetray/I3TrayHeaders.h"
#include "icetray/I3Tray.h"

TEST_GROUP(OutOfMemoryModuleTest);

namespace OutOfMemoryModuleTest
{
  class OutOfMemoryModule : public I3Module
  {
  public:

    OutOfMemoryModule(const I3Context& context) : I3Module(context) 
    {
      AddOutBox("OutBox");
    }
    
    void Configure(){}
    
    void Process()
    {
      uint64_t size = std::numeric_limits<uint64_t>::max();
      char* array = new char[size];
      for(uint64_t i = 0 ; i< size ; i++)
	array[i] = i;
    }
  };
  
  TEST(malloc_too_much)
  {
    I3Tray tray;
    tray.AddModule("OutOfMemoryModule", "test");
    try
      {
	tray.Execute(1);
	FAIL("OutOfMemoryModule didn't throw.");
      }
    catch(std::exception& e)
      {
      }
  }
}

I3_MODULE(OutOfMemoryModuleTest::OutOfMemoryModule)

