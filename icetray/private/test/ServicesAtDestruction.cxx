/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @version $Revision$
 * @date $Date$
 * @author <a href="http://icecube.umd.edu/pretz">John Pretz</a>
 *
 * This is a test which tests that the services are available to modules during
 * the call of the module's destructor
 */

#include <I3Test.h>

#include "icetray/I3Tray.h"
#include "icetray/I3ServiceFactory.h"

#include <set>


TEST_GROUP(ServicesAtDestruction);

// The service we're looking for at destruction
struct ServiceAtDestructionService;

typedef std::set<ServiceAtDestructionService*> set_t;
set_t existing_services;

struct ServiceAtDestructionService
{
  ServiceAtDestructionService()
  {
    existing_services.insert(this);
  }  
  
  ~ServiceAtDestructionService()
  {
    existing_services.erase(this);
  }
    
  ServiceAtDestructionService* that()
  {
    return this;
  }
};
  

// something that installs this service one instance into each
// context that comes by.
struct ServiceAtDestructionPluralFactory : public I3ServiceFactory
{
  ServiceAtDestructionPluralFactory(const I3Context& context) : I3ServiceFactory(context){}

  bool InstallService(I3Context& s)
  {
    boost::shared_ptr<ServiceAtDestructionService> sads(new ServiceAtDestructionService());
    return s.Put(sads, "Foo");
  }    
};

I3_SERVICE_FACTORY(ServiceAtDestructionPluralFactory);

// something that installs this service with just one instance for
// every module
struct ServiceAtDestructionServiceSingletonFactory : public I3ServiceFactory
{
  ServiceAtDestructionServiceSingletonFactory(const I3Context& context) 
    : I3ServiceFactory(context) { }
    
  boost::shared_ptr<ServiceAtDestructionService> sads;

  bool InstallService(I3Context& s)
  {
    if(!sads)
      sads = boost::shared_ptr<ServiceAtDestructionService>
	(new ServiceAtDestructionService());
    return s.Put(sads, "Foo");
  }    
};
  
I3_SERVICE_FACTORY(ServiceAtDestructionServiceSingletonFactory);

// a module to check for whether or not the service exists
struct ServicesAtDestructionModule : public I3Module
{

  ServiceAtDestructionService* that;

public:
  ServicesAtDestructionModule(const I3Context& context) : I3Module(context)
  {}
    
  ~ServicesAtDestructionModule()
  {
    ENSURE(existing_services.find(that) != existing_services.end());
  }
    
  void Configure()
  {
    log_trace("%s",__PRETTY_FUNCTION__);
    ENSURE(context_.Has<ServiceAtDestructionService>("Foo"));
    ServiceAtDestructionService& sads = context_.Get<ServiceAtDestructionService>("Foo");
    that = sads.that();
    ENSURE(existing_services.find(that) != existing_services.end());
  }

  void Process()
  {
    ENSURE(existing_services.find(that) != existing_services.end());
    RequestSuspension();
  }
};

I3_MODULE(ServicesAtDestructionModule);


TEST(plural)
{
  I3Tray tray;
  tray.AddService<ServiceAtDestructionPluralFactory>("serv");
  tray.AddModule("ServicesAtDestructionModule");    
  tray.Execute();
    
}

TEST(single)
{
  I3Tray tray;
  tray.AddService<ServiceAtDestructionServiceSingletonFactory>("serv");
  tray.AddModule("ServicesAtDestructionModule");    
  tray.Execute();
}

