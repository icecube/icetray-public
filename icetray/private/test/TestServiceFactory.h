#ifndef TESTSERVICEFACTORY_H
#define TESTSERVICEFACTORY_H

#include <I3Test.h>
#include "icetray/I3ServiceFactory.h"


class TestServiceFactory : public I3ServiceFactory
{
 public:
  static double doubleParam;
  static std::string stringParam;
  static bool boolParam;
  static float floatParam;
  static int intParam;
  static long longParam;

  bool configured;
  bool finishCalled;

 public:

  static TestServiceFactory* instance;

  ~TestServiceFactory() 
  { 
    instance = NULL; 
  }

  TestServiceFactory(const I3Context& context) 
    : I3ServiceFactory(context), configured(false)
    {
      instance = this;
      AddParameter("intParam","",intParam);
      AddParameter("stringParam","",stringParam);
      AddParameter("boolParam","",boolParam);
      AddParameter("floatParam","",floatParam);
      AddParameter("doubleParam","",doubleParam);
      AddParameter("longParam","",longParam);
      finishCalled = false;
    }

  void Configure()
    {
      log_trace("%s",__PRETTY_FUNCTION__);
      log_trace("configured=%d", configured);
      ENSURE(!configured);
      GetParameter("intParam",intParam);
      GetParameter("stringParam",stringParam);      
      GetParameter("boolParam",boolParam);
      GetParameter("floatParam",floatParam);
      GetParameter("doubleParam",doubleParam);
      GetParameter("longParam",longParam);
      configured = true;
    }

  bool InstallService(I3Context&) 
    {
      ENSURE(configured);
      return true;
    }

  void Finish()
    {
      ENSURE(!finishCalled);
      finishCalled = true;
    }
};

#endif
