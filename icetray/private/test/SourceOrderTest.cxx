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
    
  tray.ConnectBoxes("source","OutBox","client");
    
  tray.Execute(10);
}

// everything is done in reverse order, but the connections are indeed OK
TEST(correctConfiguration_2)
{
  I3Tray tray;
    
  tray.AddModule("Client", "client");
  tray.AddModule("BottomlessSource", "source");
    
  tray.ConnectBoxes("source","OutBox","client");
    
  tray.Execute(10);
}

