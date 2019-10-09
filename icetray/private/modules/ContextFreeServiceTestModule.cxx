/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @version $Revision$
 * @date $Date$
 * @author troy d. straszheim
 *
 * This tests that the global GetService<> works; that the underlying
 * context-switching is done correctly.
 */

#include <icetray/I3Tray.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>
#include <icetray/OMKey.h>
#include <boost/assign/std/vector.hpp>

using namespace boost::assign;

struct MyService{};
static boost::shared_ptr<MyService> service_ptr_param;

struct ContextFreeServiceTestModule : I3Module
{
  ContextFreeServiceTestModule(const I3Context& context) : I3Module(context) 
  { 
    service_ptr_param = boost::shared_ptr<MyService>();
    AddParameter("service_ptr_param", "pointer to service.",service_ptr_param);
  }

  virtual void Configure() 
  { 
    GetParameter("service_ptr_param",service_ptr_param);
  }

  virtual void Process() { 
    I3FramePtr frame = PopFrame(); 
    PushFrame(frame, "OutBox"); 
}

  virtual void Finish() { ; }
};

I3_MODULE(ContextFreeServiceTestModule);
