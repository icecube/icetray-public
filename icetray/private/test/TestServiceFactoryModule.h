#ifndef TESTSERVICEFACTORYMODULE_H
#define TESTSERVICEFACTORYMODULE_H

#include <icetray/I3Module.h>

struct TestServiceFactoryModule : public I3Module
{
  TestServiceFactoryModule(const I3Context& context) 
    : I3Module(context)
  {}
};

#endif 
