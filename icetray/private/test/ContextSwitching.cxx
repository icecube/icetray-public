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

#include <I3Test.h>

#include "icetray/I3Tray.h"

#include "icetray/I3ServiceFactory.h"
#include "icetray/I3Frame.h"

#include <icetray/I3DefaultName.h>

TEST_GROUP(ContextSwitching);

struct ServiceWithDefaultName 
{
  const int the_int;
  ServiceWithDefaultName(int i) : the_int(i) { }
};

I3_POINTER_TYPEDEFS(ServiceWithDefaultName);
I3_DEFAULT_NAME(ServiceWithDefaultName);

struct IntegerServiceFactory : I3ServiceFactory
{
  static int service_number;

  IntegerServiceFactory(const I3Context& context)
    : I3ServiceFactory(context) { }

  virtual ~IntegerServiceFactory() { }

  virtual bool InstallService(I3Context& services)
  {
    log_trace(__PRETTY_FUNCTION__);
    shared_ptr<int> the_service(new int(service_number));
    log_debug("putting service with %d", *the_service);
    ENSURE(services.Put(the_service, "int"));
    shared_ptr<ServiceWithDefaultName> 
      defname_service(new ServiceWithDefaultName(service_number));
    ENSURE(services.Put(defname_service));
    service_number++;
    return true;
  }
};
int IntegerServiceFactory::service_number = 0;

I3_SERVICE_FACTORY(IntegerServiceFactory);

// gets an int from the service outside of any module context, once
// from a POD service (type int) and once from a service with a
// default name.
int GetsAnInt() {
  // the "int" service is just an int, it has no nested static
  // DefaultName() method.  Therefore you must specify a name.  If
  // you don't, on the Mac you get a fatal compiler bug, Bus Error.
  // Gah.  Apple is not fixing these bugs, they're concentrating on
  // the next compiler.
  int i = GetService<int>("int"); 
  log_trace("GetsAnInt got %d", i);
  int j = GetService<ServiceWithDefaultName>().the_int;
  ServiceWithDefaultNamePtr swdefnp = GetService<ServiceWithDefaultNamePtr>();
  ENSURE(i == j);
  ENSURE(swdefnp->the_int == i);
  return i;
}

class ContextCheckingSource : public I3Module
{
public:
  ContextCheckingSource(const I3Context& context) 
    : I3Module(context)
  {
    AddOutBox("OutBox");
    CheckContext();
  }

  void CheckContext() {
    log_trace(__PRETTY_FUNCTION__);
    int i = context_.Get<int>("int");
    const I3Context &context = I3Tray::GetActiveContext();
    ENSURE(&context_ == &context);
    log_trace("mine:%p active:%p", &context_ , &context);
    int j = GetsAnInt();
    log_trace("Got int %d", i);
    ENSURE(i == j);
  }

  virtual void Configure() { CheckContext(); }
  virtual void Process()
  {
    CheckContext();
    I3FramePtr frame(new I3Frame(I3Frame::Physics));
    PushFrame(frame,"OutBox");
  }

  virtual void Finish()
  {
    CheckContext();
  }
};

I3_MODULE(ContextCheckingSource);

struct GetsAnIntModule : I3Module
{
  GetsAnIntModule(const I3Context& context) : I3Module(context) 
  { 
    AddOutBox("OutBox");
    CheckContext();
  }

  void CheckContext() {
    log_trace(__PRETTY_FUNCTION__);
    int i = GetContext().Get<int>("int");
    const I3Context &context = I3Tray::GetActiveContext();
    ENSURE(&(GetContext()) == &context);
    log_trace("mine:%p active:%p", &(GetContext()), &context);
    int j = GetsAnInt();
    log_trace("Got int %d", i);
    ENSURE(i == j);
  }

  virtual void Configure() { CheckContext(); }
  virtual void Reconfigure() { CheckContext(); }

  virtual void Process() 
  { 
    log_trace(__PRETTY_FUNCTION__);
    I3FramePtr frame = PopFrame();
    int i = GetContext().Get<int>("int");
    const I3Context &context = I3Tray::GetActiveContext();
    ENSURE(&(GetContext()) == &context);
    log_trace("mine:%p active:%p", &(GetContext()), &context);
    int j = GetsAnInt();
    ENSURE(i == j);
    log_trace("Got int %d", i);
    PushFrame(frame, "OutBox");
  }
  virtual void Resume() { CheckContext(); }
  virtual void Finish() { CheckContext(); }
  virtual void Abort() { CheckContext(); }
};

I3_MODULE(GetsAnIntModule);

TEST(one) 
{
  I3Tray tray;
  tray.AddService<IntegerServiceFactory>("int");

  tray.AddModule<ContextCheckingSource>("source");
  tray.AddModule<GetsAnIntModule>("getsint0");
  tray.AddModule<GetsAnIntModule>("getsint1");
  tray.AddModule<GetsAnIntModule>("getsint2");
  tray.AddModule("TrashCan", "trash");

  tray.Execute(4);
  tray.Finish();
}


