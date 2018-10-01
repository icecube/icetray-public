#include <I3Test.h>

#include <icetray/I3Module.h>

TEST_GROUP(I3ModulePlumbing);

class ConnectionTestModule : public I3Module{
public:
  ConnectionTestModule(const I3Context& ctx):I3Module(ctx){
    AddParameter("ExtraOutBoxes","names of OutBoxes to add when configuring",
                 std::vector<std::string>());
  }
  
  ConnectionTestModule(const I3Context& ctx,
                       std::vector<std::string> outboxNames):
  I3Module(ctx){
    for(std::vector<std::string>::const_iterator nameIt=outboxNames.begin();
        nameIt!=outboxNames.end(); nameIt++)
      AddOutBox(*nameIt);
    AddParameter("ExtraOutBoxes","names of OutBoxes to add when configuring",
                 std::vector<std::string>());
  }
  
  virtual void Configure(){
    std::vector<std::string> outboxNames;
    GetParameter("ExtraOutBoxes",outboxNames);
    for(std::vector<std::string>::const_iterator nameIt=outboxNames.begin();
        nameIt!=outboxNames.end(); nameIt++){
      AddOutBox(*nameIt);
    }
  }
};

TEST(HasOutBox){
  I3Context ctx;
  std::vector<std::string> expectedOutBoxes;
  expectedOutBoxes.push_back("Box1");
  expectedOutBoxes.push_back("Box2");
  expectedOutBoxes.push_back("Box3");
  ConnectionTestModule ctm(ctx,expectedOutBoxes);
  
  for(std::vector<std::string>::const_iterator nameIt=expectedOutBoxes.begin();
      nameIt!=expectedOutBoxes.end(); nameIt++)
    ENSURE(ctm.HasOutBox(*nameIt));
  ENSURE(!ctm.HasOutBox("OutBox"));
  ENSURE(!ctm.HasOutBox("OtherBox"));
}

TEST(ConnectOutBox){
  I3Context ctx;
  std::vector<std::string> initialOutBoxes, dynamicOutBoxes;
  initialOutBoxes.push_back("Box1");
  dynamicOutBoxes.push_back("Box2");
  ConnectionTestModule ctm(ctx,initialOutBoxes);
  
  boost::shared_ptr<I3Module> dummy1(new I3Module(ctx));
  boost::shared_ptr<I3Module> dummy2(new I3Module(ctx));
  //connect an outbox which definietly exists
  ctm.ConnectOutBox("Box1",dummy1);
  //we have to permit this sort of out-of-order madness, apparently
  ctm.ConnectOutBox("Box2",dummy2);
  ctm.GetConfiguration().Set("ExtraOutBoxes",boost::python::object(dynamicOutBoxes));
  ctm.Configure();
}

TEST(OutBoxesConnected){
  I3Context ctx;
  std::vector<std::string> expectedOutBoxes;
  expectedOutBoxes.push_back("Box1");
  expectedOutBoxes.push_back("Box2");
  expectedOutBoxes.push_back("Box3");
  ConnectionTestModule ctm(ctx,expectedOutBoxes);
  
  //we haven't connected anything yet. . .
  ENSURE(!ctm.AllOutBoxesConnected());
  
  boost::shared_ptr<I3Module> dummy1(new I3Module(ctx));
  boost::shared_ptr<I3Module> dummy2(new I3Module(ctx));
  boost::shared_ptr<I3Module> dummy3(new I3Module(ctx));
  
  ctm.ConnectOutBox("Box1",dummy1);
  //still two boxes not connected
  ENSURE(!ctm.AllOutBoxesConnected());
  
  ctm.ConnectOutBox("Box2",dummy2);
  //still one box not connected
  ENSURE(!ctm.AllOutBoxesConnected());
  
  ctm.ConnectOutBox("Box3",dummy3);
  //now they're all connected
  ENSURE(ctm.AllOutBoxesConnected());
}

TEST(HasInBox){
  I3Context ctx;
  boost::shared_ptr<ConnectionTestModule> m1(new ConnectionTestModule(ctx,std::vector<std::string>(1,"OutBox")));
  boost::shared_ptr<I3Module> m2(new I3Module(ctx));
  
  ENSURE(!m1->HasInBox());
  ENSURE(!m2->HasInBox());
  m1->ConnectOutBox("OutBox",m2);
  ENSURE(!m1->HasInBox());
  ENSURE(m2->HasInBox()); //should now be true
}
