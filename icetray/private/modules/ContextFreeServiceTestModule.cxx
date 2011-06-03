/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: ServicesAtDestruction.cxx 5468 2005-03-30 14:48:26Z pretz $
 *
 * @version $Revision: 1.3 $
 * @date $Date: 2005-03-30 16:48:26 +0200 (Wed, 30 Mar 2005) $
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
static shared_ptr<MyService> service_ptr_param;

struct ContextFreeServiceTestModule : I3Module
{
  ContextFreeServiceTestModule(const I3Context& context) : I3Module(context) 
  { 
    AddOutBox("OutBox");

    service_ptr_param = shared_ptr<MyService>();
    AddParameter("service_ptr_param", "pointer to service.",service_ptr_param);
  }

  virtual void Configure() 
  { 
    GetParameter("service_ptr_param",service_ptr_param);
  }

  virtual void Reconfigure() { ; }

  virtual void Process() { 
    I3FramePtr frame = PopFrame(); 
    PushFrame(frame, "OutBox"); 
}

  virtual void Resume() { ; }
  virtual void Finish() { ; }
  virtual void Abort() { ; }
};

I3_MODULE(ContextFreeServiceTestModule);
