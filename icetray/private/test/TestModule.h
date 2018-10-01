#ifndef TESTMODULE_H
#define TESTMODULE_H

#include <icetray/I3Module.h>

class TestModule : public I3Module
{
 public: 
  static TestModule* module;
  int intParam;
  double doubleParam;
  long longParam;
  std::string stringParam;
  bool boolParam;

 public:

  TestModule(const I3Context& context) : I3Module(context) 
    {
      module = this;
      boolParam = false;
      AddParameter("boolParam","",boolParam);
      intParam = 3;
      AddParameter("intParam","",intParam);
      doubleParam = 2.2222;
      AddParameter("doubleParam","",doubleParam);
      stringParam = "defaultvalue";
      AddParameter("stringParam","",stringParam);
      longParam = 123123123;
      AddParameter("longParam","",longParam);
    }

  void Configure()
    {
      log_debug("%s",__PRETTY_FUNCTION__);
      GetParameter("boolParam",boolParam);
      GetParameter("intParam",intParam);
      GetParameter("doubleParam",doubleParam);
      GetParameter("stringParam",stringParam);
      GetParameter("longParam",longParam);
    }

  void Process()
    {
      log_debug("%s",__PRETTY_FUNCTION__);
    }

  virtual ~TestModule()
    {
      module = 0;
    }

};

#endif 
