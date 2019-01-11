/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: ServicesAtDestruction.cxx 165886 2018-10-01 14:37:58Z nwhitehorn $
 *
 * @version $Revision: 165886 $
 * @date $Date: 2018-10-01 07:37:58 -0700 (Mon, 01 Oct 2018) $
 * @author <a href="http://icecube.umd.edu/pretz">John Pretz</a>
 *
 * This is a test which tests that the services are available to modules during
 * the call of the module's destructor
 */

#include <I3Test.h>

#include "icetray/I3Tray.h"
#include "icetray/I3ServiceFactory.h"

TEST_GROUP(ServicesAtDestruction);

// The service we're looking for at destruction
struct ServiceAtDestructionService
{
  vector<bool*> watched_;
  ServiceAtDestructionService()
  {
    log_trace(__PRETTY_FUNCTION__);
  }  
  
  ~ServiceAtDestructionService()
  {
    log_trace(__PRETTY_FUNCTION__);
    for(unsigned int i = 0 ; i < watched_.size() ; i++)
      {
	log_trace("falsifying the bool at %p which is already set to %d",
		  watched_[i], *watched_[i]);
	(*watched_[i]) = false;
	log_trace("And now is set to %d", *watched_[i]);
      }
  }
    
  void AddMonitoredBool(bool* b)
  {
    watched_.push_back(b);
    *b = true;
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
  bool serviceExists_;
public:
  ServicesAtDestructionModule(const I3Context& context) : I3Module(context)
  {
    AddOutBox("OutBox");
  }
    
  ~ServicesAtDestructionModule()
  {
    log_trace(__PRETTY_FUNCTION__);
    log_trace("service status at destruction: address: %p value: %d",
	      &serviceExists_, serviceExists_);
    ENSURE(serviceExists_);
  }
    
  void Configure()
  {
    log_trace(__PRETTY_FUNCTION__);
    ENSURE(context_.Has<ServiceAtDestructionService>("Foo"));
    ServiceAtDestructionService& sads = context_.Get<ServiceAtDestructionService>("Foo");
    sads.AddMonitoredBool(&serviceExists_);
  }

  void Process()
  {
    ENSURE(serviceExists_);
    RequestSuspension();
  }
};

I3_MODULE(ServicesAtDestructionModule);


TEST(plural)
{
  I3Tray tray;
  tray.AddService<ServiceAtDestructionPluralFactory>("serv");

  tray.AddModule("ServicesAtDestructionModule", "source");
  tray.AddModule("TrashCan", "trash");
    
  tray.Execute();
  tray.Finish();
    
}

TEST(single)
{
  I3Tray tray;
  tray.AddService<ServiceAtDestructionServiceSingletonFactory>("serv");

  tray.AddModule("ServicesAtDestructionModule", "source");
  tray.AddModule("TrashCan", "trash");
    
  tray.Execute();
  tray.Finish();
}

