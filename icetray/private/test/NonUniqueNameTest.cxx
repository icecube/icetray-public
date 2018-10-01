#include <I3Test.h>
#include "icetray/I3TrayHeaders.h"
#include "icetray/I3Tray.h"

TEST_GROUP(NonUniqueNames)

namespace NonUniqueNames
{
  class NonUniqueNamesSource : public I3Module
  {
  public:
    NonUniqueNamesSource(I3Context& context) : I3Module(context) 
    {
      AddOutBox("OutBox");
    }
    
    void Configure(){}
    
    void Process(){
      // and so this function should never be called:
      FAIL("module should never have been run");
    }
  };

  class NonUniqueNameModule : public I3Module
  {
  public:
    NonUniqueNameModule(I3Context& context) : I3Module(context)
    {}
  };
  
  TEST(duplicate_module)
  {
    I3Tray tray;
    try {
      tray.AddModule<NonUniqueNamesSource>("test");
      tray.AddModule<NonUniqueNameModule>("module");
      tray.AddModule<NonUniqueNameModule>("module");
      tray.Execute(0);
      FAIL("tray didn't throw exception like it should have!");
    } catch(std::runtime_error& e) {
      // good. it threw.
    }
  }
}

