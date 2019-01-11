#include <I3Test.h>

#include "TestClientModule.h"
#include <icetray/I3Tray.h>

class SourceOrderTester{};

typedef TestClientModule<SourceOrderTester> Client;

I3_MODULE(Client);

TEST_GROUP(SourceOrderTest);

TEST(correctConfiguration_1)
{
  // just testing that this doesn't give an error
  I3Tray tray;
    
  tray.AddModule("BottomlessSource", "source");
  tray.AddModule("Client","client");
  tray.AddModule("TrashCan","trash");
    
  tray.ConnectBoxes("source","OutBox","client");
  tray.ConnectBoxes("client","OutBox","trash");
    
  tray.Execute(10);
  tray.Finish();
}

// everything is done in reverse order, but the connections are indeed OK
TEST(correctConfiguration_2)
{
  I3Tray tray;
    
  tray.AddModule("TrashCan","trash");
  tray.AddModule("Client", "client");
  tray.AddModule("BottomlessSource", "source");
    
  tray.ConnectBoxes("client","OutBox","trash");
  tray.ConnectBoxes("source","OutBox","client");
    
  tray.Execute(10);
  tray.Finish();
}

TEST(wrongConfiguration)
{
  try 
  {
    I3Tray tray;
    tray.AddModule("BottomlessSource", "source");
    tray.AddModule<Client>("client");
      
    tray.ConnectBoxes("client","OutBox","source");
      
//#warning need more robust checking for cycles and pathological tray configurations.
    // FAIL("This is supposed to generate an error");
  }
  catch(const std::exception& e)
    {
      // everything's fine
    }
}
