#include <I3Test.h>
#include "icetray/I3TrayHeaders.h"
#include "icetray/I3Tray.h"
#include "icetray/I3ServiceFactory.h"

TEST_GROUP(NonUniqueNames)

namespace NonUniqueNames
{
  class NonUniqueNamesSource : public I3Module
  {
  public:
    NonUniqueNamesSource(I3Context& context) : I3Module(context) 
    {}
    
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
      FAIL("tray didn't throw exception when two modules were added with the same name!");
    } catch(std::runtime_error& e) {
      // good. it threw.
    }
  }
  
  struct NonUniqueNameService : public I3ServiceFactory
  {
    NonUniqueNameService(const I3Context& context) 
    : I3ServiceFactory(context) { }
    
    bool InstallService(I3Context& s)
    {
      FAIL("Service should never have been installed");
      return false;
    }    
  };
  
  I3_SERVICE_FACTORY(NonUniqueNameService);
  
  TEST(duplicate_service)
  {
    I3Tray tray;
    try{
      tray.AddService<NonUniqueNameService>("service");
      tray.AddService<NonUniqueNameService>("service");
      FAIL("tray didn't throw exception when two services were added with the same name!");
    } catch(std::runtime_error& e) {
      // good. it threw.
    }
  }
}

