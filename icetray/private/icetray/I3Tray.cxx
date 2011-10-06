/**
 *  $Id: I3Tray.cxx 51710 2009-01-06 19:23:39Z troy $
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  Simon Patton
 *  John Pretz
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */
#include <signal.h>

#include <string>
#include <map>
#include <iostream>
#include <cassert>
#include <exception>
#include <deque>

#include <boost/python.hpp>
#include <boost/foreach.hpp>


#include <icetray/I3Tray.h>
#include <icetray/I3TrayInfoService.h>
#include <icetray/I3Module.h>
#include <icetray/I3OpNewServiceFactory.h>
#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <icetray/I3PhysicsUsage.h>
#include <icetray/serialization.h>


using namespace std;

volatile sig_atomic_t I3Tray::suspension_requested_;

namespace bp = boost::python;


void I3Tray::set_suspend_flag(int sig)
{
  if (sig == SIGINT) {
    std::cerr << "\n***\n*** SIGINT received.   Calling Finish() at the end of the current frame. \n*** Hit ^C again to force quit.\n***\n";
    suspension_requested_ = 1;
    signal(SIGINT, die_messily);
  }
}

void I3Tray::die_messily(int sig)
{
  if (sig == SIGINT) {
    std::cerr << "\n***\n*** SIGINT received again; exiting immediately. \n***\n";
    exit(1);
  }
}

I3Tray *executing_tray = NULL;

void I3Tray::report_usage(int sig)
{
	typedef pair<string,I3PhysicsUsage> mru_pair;
	typedef pair<double,string> name_pair;
	
	map<string, I3PhysicsUsage> mru = executing_tray->Usage();
	map<double, string> inorder;
	double total_time = 0.;
	BOOST_FOREACH(const mru_pair &pair, mru) {
		inorder[pair.second.usertime] = pair.first;
		total_time += pair.second.usertime;
	}
	double acc_time = 0.;
	BOOST_FOREACH(const name_pair &pair, inorder) {
		const string &name = pair.second;
		const I3PhysicsUsage &ru = mru[pair.second];
		printf("%40s: %6u calls to physics %9.2fs user %9.2fs system\n",
		    name.c_str(), ru.ncall, ru.usertime, ru.systime);
		if ((acc_time += ru.usertime)/total_time > 0.9)
			break;
	}
}

I3Tray::I3Tray() :
  factories(),
  factory_contexts(),
  module_contexts(),
  boxes_connected(false),
  configure_called(false),
  finish_called(false),
  execute_called(false)
{
  // This is a very special hack, and not sooo bad. We're putting a
  // shared pointer to a bald pointer, in this internal service
  // factory's context, so I3TrayInfoService can call us back and
  // ask about our configuration, so that he can write it to our
  // superfly .i3 files.

  AddService("I3TrayInfoServiceFactory", "__config");
  // careful... this is a shared pointer to a pointer to this
  shared_ptr<I3Tray*> tray_pp(new I3Tray*(this));
  assert(tray_pp);
  assert(*tray_pp);
  factory_contexts["__config"]->Put(tray_pp, "__tray");
}

I3Tray::~I3Tray()
{
  // this Finish() is introducing too many bugs.  Bad.  
  // interferes with checking for 'execute called', etc.
  // uncomment and run the test suite to see.
  // Finish();

  suspension_requested_ = false;
  active_context_ = 0;
}

void
I3Tray::Abort()
{
}

I3Tray::param_setter
I3Tray::AddModule(const std::string& classname, const std::string& instancename)
{
  return AddModule(bp::object(classname), instancename);
}

I3Tray::param_setter
I3Tray::AddModule(bp::object obj, const std::string& instancename)
{
  if (configure_called)
    log_fatal("I3Tray::Configure() already called -- cannot add new modules");

  modules_in_order.push_back(instancename);
  I3ContextPtr context(new I3Context);
  I3ConfigurationPtr config(new I3Configuration);

  if(PyString_Check(obj.ptr())) 
    // obj is a string... construct C++ module later from factory
    {
      config->ClassName(bp::extract<std::string>(obj));
    } 
  else if (PyFunction_Check(obj.ptr()))
    // it is a python function... put the object in the context and 
    // call it a PythonFunction
    {
      config->ClassName("PythonFunction"); // Used to look up Create function in factory
      // only in this case... we need the python object whose constructor will get called
      // by the Create function
      context->Put(boost::shared_ptr<bp::object>(new bp::object(obj)), 
                   "object");
    } 
  else if (PyType_Check(obj.ptr()))
    {
      config->ClassName("PythonModule"); // Used to look up Create function in factory
      // only in this case... we need the python object whose constructor will get called
      // by the Create function
      context->Put(boost::shared_ptr<bp::object>(new bp::object(obj)),
                   "class");
    }
  else
    {
      log_fatal("'%s%s' passed to AddModule with instance name %s.  Must be a string, a python function, or a python I3Module.", 
                PyEval_GetFuncName(obj.ptr()),
                PyEval_GetFuncDesc(obj.ptr()),
                instancename.c_str());
    }

  log_trace("%s : %s", __PRETTY_FUNCTION__, instancename.c_str());

  config->InstanceName(instancename);
  context->Put(config);
  log_debug("putting %p", obj.ptr());
  module_contexts[instancename] = context;

  return param_setter(*this, instancename);

}

I3Tray::param_setter
I3Tray::AddService(const std::string& classname, const std::string& instancename)
{ 
  if (configure_called)
    log_fatal("I3Tray::Configure() already called -- cannot add new services");

  if(factory_contexts.find(instancename)!=factory_contexts.end())
    log_fatal("More than one service added with the name '%s'",instancename.c_str());
  factories_in_order.push_back(instancename);

  log_trace("AddService %s %s", classname.c_str(), instancename.c_str());

  I3ContextPtr context(new I3Context);
  I3ConfigurationPtr config(new I3Configuration);
  assert(!classname.empty());
  config->ClassName(classname);
  config->InstanceName(instancename);
  context->Put(config);
  factory_contexts[instancename] = context;

  return param_setter(*this, instancename);
}

shared_ptr<I3Module> 
I3Tray::CreateModule(I3Context& context, const std::string& classname)
{
  I3ModulePtr module = I3::Singleton<I3ModuleFactory>::get_const_instance()
    .Create(classname)(context);
  return module;
}

shared_ptr<I3ServiceFactory>
I3Tray::CreateService (I3Context& context, const std::string& classname)
{
    I3ServiceFactoryPtr servicefactory = 
      I3::Singleton<I3ServiceFactoryFactory>::get_const_instance().Create(classname)(context);
    return servicefactory;
}

void dump_config(const vector<string>& names, 
                 const map<string,I3ContextPtr>& contexts,
                 ostream& os)
{
  boost::archive::xml_oarchive xoa(os);

  BOOST_FOREACH(const string& s, names)
    {
      xoa << make_nvp("name", s);
      const I3Context& context = *(contexts.find(s)->second);
      const I3Configuration& config = context.Get<I3Configuration>();
      xoa << make_nvp("configuration", config);
    }
}

void
I3Tray::MoveModule(const std::string& name, const std::string& anchor, bool after)
{
	std::vector<std::string>::iterator deleter =
	    std::find(modules_in_order.begin(), modules_in_order.end(), name);
	std::vector<std::string>::iterator inserter =
	    std::find(modules_in_order.begin(), modules_in_order.end(), anchor);
	if (deleter == modules_in_order.end())
		log_fatal("Attempted to move module '%s', which hasn't been "
		    "added to the tray yet!", name.c_str());
	if (inserter == modules_in_order.end())
		log_fatal("Attempted to move module '%s' to %s module '%s', "
		    "but '%s' hasn't been added to the tray yet!", name.c_str(),
		    (after ? "after" : "before"), anchor.c_str(), anchor.c_str());
	modules_in_order.erase(deleter);
	inserter =
	    std::find(modules_in_order.begin(), modules_in_order.end(), anchor);

	if (after && inserter == modules_in_order.end())
		modules_in_order.push_back(name);
	else
		modules_in_order.insert(after ? inserter+1 : inserter, name);
}

bool
I3Tray::ConnectBoxes(const std::string& fromModule,
                     const std::string& fromOutBox,
                     const std::string& toModule)
{
  // flag: if we don't connect any boxes, then we later do a default.
  boxes_connected = true;

  shared_ptr<I3Context> context;

  map<string, I3ContextPtr>::const_iterator iter = module_contexts.find(fromModule);

  if (iter == module_contexts.end())
    log_fatal("couldn't find context for \"%s\"", fromModule.c_str());

  context = iter->second;

  I3Configuration& config = context->Get<I3Configuration>();
  log_debug("telling config to connect outbox \"%s\" to module \"%s\"", 
            fromOutBox.c_str(), toModule.c_str());
  config.Connect(fromOutBox, toModule);
  return true;
}

void 
I3Tray::Configure()
{
  if (configure_called)
      return;
  configure_called = true;

  // 
  //  first, if we never explicity called ConnectBoxes, connect the
  //  modules to one another in the order they were added.
  //
  if (! boxes_connected)
    {
      vector<std::string>::const_iterator to = ++modules_in_order.begin();
      while(to != modules_in_order.end())
        {
          log_trace("%s => %s", (to-1)->c_str(), to->c_str());
          ConnectBoxes(*(to-1), "OutBox", *to);
          to++;
        }
    }

  //
  // Create the ServiceFactories in the order they were added.  Each
  // time, give them a context that has the previously created
  // services in it already.
  // 
  BOOST_FOREACH(const string& objectname, factories_in_order)
    {
      // at this point these context don't have factories in them yet.
      // Just the factories' I3Configurations, ready and waiting.
      I3ContextPtr context_p = factory_contexts[objectname];
      const std::string& classname = context_p->Get<I3Configuration>().ClassName();

      // use previously configured service factories to install
      // services into the context we're about to create the next
      // service factory in
      for (map<string, I3ServiceFactoryPtr>::const_iterator iter = factories.begin();
           iter != factories.end();  iter++)
        iter->second->InitializeService(*context_p);
      
      // now the context has the services and config in it.
      I3ServiceFactoryPtr factory = CreateService(*context_p, classname);

      factory->Configure();

      // when we do this, this service factory will be asked to
      // InitializeService() on the context that the *next* service
      // factory is created into.
      factories[objectname] = factory;
    }

  //
  // Similar to the ServiceFactories above, create the modules in the
  // order they were added.  Their contexts are waiting, and they
  // already contain configurations.
  // 
  BOOST_FOREACH(const string& objectname, modules_in_order)
    {
      // at this point these contexts don't have factories in them yet.
      // Just the factories' I3Configurations, ready and waiting.
      I3ContextPtr context_p = module_contexts[objectname];

      // as above, use service factories to install services into the
      // context we're about to create the next module in
      for (map<string, I3ServiceFactoryPtr>::const_iterator iter = factories.begin();
           iter != factories.end();  iter++)
        iter->second->InitializeService(*context_p);
      
      shared_ptr<map<string, pair<FrameFifoPtr, I3ModulePtr> > > connections
        (new map<string, pair<FrameFifoPtr, I3ModulePtr> >);
      const map<string, string>& outboxes = context_p->Get<I3Configuration>().Outboxes();

      for (map<string, string>::const_iterator iter = outboxes.begin();
           iter != outboxes.end();
           iter++)
        {
          log_trace("Registering outbox \"%s\" from configuration.", (iter->first).c_str());
          (*connections)[iter->first] = make_pair(FrameFifoPtr(), I3ModulePtr());
        }
      context_p->Put(connections, "OutBoxes");
      // now the context has the services and fifos in it.

      // we need the classname from the configuration (which came from
      // the steering file) for the CreateModule call
      const std::string& classname = context_p->Get<I3Configuration>().ClassName();
      SetActiveContext(context_p.get());
      I3ModulePtr module = CreateModule(*context_p, classname);
      SetActiveContext(0);
      modules[objectname] = module;
    }

  // now fly through there and connect ye olde boxes.  each inbox is
  // just a pointer to a deque of frames.  each outbox is a pointer to
  // a deque of frames plus a pointer to the module that has that
  // deque for an inbox.
  
  BOOST_FOREACH(const string& from_module_name, modules_in_order)
    {
      I3ContextPtr from_module_context_p = module_contexts[from_module_name];
      const map<string, string>& from_module_outboxes_config_map = 
        from_module_context_p->Get<I3Configuration>().Outboxes();

      shared_ptr<map<string, pair<FrameFifoPtr, I3ModulePtr> > > from_module_outboxes_map =
        from_module_context_p->Get<shared_ptr<map<string, pair<FrameFifoPtr, I3ModulePtr> > > >("OutBoxes");

      for (map<string, string>::const_iterator iter = from_module_outboxes_config_map.begin();
           iter != from_module_outboxes_config_map.end();  
           iter++)
        {
          const string& from_outbox_name = iter->first;
          const string& to_module_name   = iter->second;
          I3ModulePtr   to_module_ptr    = modules[to_module_name];

          FrameFifoPtr connection(new FrameFifo);

          (*from_module_outboxes_map)[from_outbox_name].first = connection;
          (*from_module_outboxes_map)[from_outbox_name].second = to_module_ptr;
          I3ContextPtr to_module_context = module_contexts[to_module_name];
          if (!to_module_context)
            log_fatal("\n\tUnable to get context for module \"%s\""
                      "\n\twhile connecting box \"%s\" from Module \"%s\"."
                      "\n\tDid you add module \"%s\"?", 
                      to_module_name.c_str(), 
                      from_outbox_name.c_str(), 
                      from_module_name.c_str(),
                      to_module_name.c_str());
          to_module_context->Put(connection,"InBox");
        }
    }

  // find the module without an inbox and set it as the "driving"
  // module.
  for (map<string,I3ContextPtr>::const_iterator iter = module_contexts.begin();
       iter != module_contexts.end();
       iter++)
    {
      I3Context& context = *(iter->second);
      if (!context.Has<FrameFifoPtr>("InBox"))
        {
          driving_module = modules[iter->first];
          log_trace("Driving module is %s", iter->first.c_str());
        }
    }

  // when we bang the driving module, it will bang all the modules
  // watching its outboxes, who will in turn bang the next modules....
  driving_module->Do(&I3Module::Configure_);

  // check that all parameters set in steering file were
  // AddParametered and GetParametered.
  // find the module without an inbox and set it as the "driving"
  // module.
  for (map<string,I3ContextPtr>::const_iterator iter = module_contexts.begin();
       iter != module_contexts.end();
       iter++)
    {
      const I3Configuration& config = iter->second->Get<I3Configuration>();
      if (!config.is_ok())
        {
          std::cerr << config;
          log_fatal("Configuration error.  Turn up your logging to see just what.");
        }
    }

  for (map<string,I3ContextPtr>::const_iterator iter = factory_contexts.begin();
       iter != factory_contexts.end();
       iter++)
    {
      const I3Configuration& config = iter->second->Get<I3Configuration>();
      if (!config.is_ok())
        {
          std::cerr << config;
          log_fatal("Configuration error.  Turn up your logging to see just what.");
        }
    }
}

void
I3Tray::Execute()
{
  if (execute_called) {
    log_error("This I3Tray has already been executed. Ignoring second call to Execute()");
    return;
  }

  execute_called = true;
  signal(SIGINT, set_suspend_flag);
#ifdef SIGINFO
  executing_tray = this;
  signal(SIGINFO, report_usage);
#endif

  Configure();

  if (!driving_module)
    log_fatal("No driving module?  Did you not add a module with no inbox?");

  while (!suspension_requested_)
    {
      log_trace("icetray dispatching Process_");
      driving_module->Do(&I3Module::Process_);
    }
}

void
I3Tray::Execute(unsigned maxCount)
{
  if (execute_called) {
    log_error("This I3Tray has already been executed. Ignoring second call to Execute()");
    return;
  }

  execute_called = true;
  signal(SIGINT, set_suspend_flag);
#ifdef SIGINFO
  executing_tray = this;
  signal(SIGINFO, report_usage);
#endif

  Configure();

  if (!driving_module)
    log_fatal("No driving module?  Did you not add a module with no inbox?");

  for (unsigned i=0; i<maxCount && !suspension_requested_; i++)
    {
      log_trace("%u/%u icetray dispatching Process_", i, maxCount);
      driving_module->Do(&I3Module::Process_);
    }
}

map<string, I3PhysicsUsage>
I3Tray::Usage()
{
  map<string, I3ModulePtr>::iterator iter = modules.begin();
  map<string, I3PhysicsUsage> mspu;
  while(iter != modules.end())
    {
      I3PhysicsUsage ru = iter->second->ReportUsage();
      log_debug("Module %s reports %f %f %u", 
                iter->first.c_str(),
                ru.systime, ru.usertime, ru.ncall);
      mspu[iter->first] = ru;
      iter++;
    }
  return mspu;
}


void
I3Tray::Finish()
{
  if (finish_called || !execute_called) {
    const string e = "Finish() already called, or before calling Execute()";
    log_trace("%s", e.c_str());
    throw runtime_error(e.c_str());
    return;
  }
  finish_called = true;

  std::cout << "I3Tray finishing...\n";

  if (!driving_module)
    log_fatal("Attempt to call finish, but there is no driving module.  Did you forget to call Execute()?");
  driving_module->Do(&I3Module::Finish);
  
  for (int i = factories_in_order.size()-1;
       i >= 0; 
       i--)
    {
      const std::string& factname = factories_in_order[i];
      log_trace("calling finish on factory %s", factname.c_str());
      factories[factname]->Finish();
    }
}

std::ostream&
operator<<(std::ostream& os, I3Tray& tray)
{
    tray.Configure();

    I3TrayInfoService srv(tray);
    const I3TrayInfo& config = srv.GetConfig();
    return (os << config);
}

void
I3Tray::Print()
{
    std::cout << *this;
}

const std::vector<std::string> &
I3Tray::Modules() {
    return modules_in_order;
}

const std::vector<std::string> &
I3Tray::Services() {
    return factories_in_order;
}

const I3Configuration &
I3Tray::ModuleConfiguration(std::string name) {
    I3ContextPtr context_p = module_contexts[name];
    return context_p->Get<I3Configuration>();
}

const I3Configuration &
I3Tray::ServiceConfiguration(std::string name) {
    I3ContextPtr context_p = factory_contexts[name];
    return context_p->Get<I3Configuration>();
}

// special overload to help with compiled steering files.
bool
I3Tray::SetParameter(const string& module,
                     const string& parameter,
                     const char* value)
{
  log_trace("%s", __PRETTY_FUNCTION__);
  return SetParameter(module, parameter, string(value));
}

bool 
I3Tray::SetParameter(const string& module,
                     const string& parameter,
                     bp::object value)
{
  log_trace("%s", __PRETTY_FUNCTION__);
  shared_ptr<I3Context> context;

  map<string, I3ContextPtr>::const_iterator iter = module_contexts.find(module);
  if (iter == module_contexts.end())
    iter = factory_contexts.find(module);

  if (iter == factory_contexts.end())
    log_fatal("couldn't find context for \"%s\"", module.c_str());

  context = iter->second;

  // "template" here is a fix for the 3.2 compiler
  I3Configuration& config = context->Get<I3Configuration>();

  config.Set(parameter, value);

  string value_as_string = bp::extract<std::string>(value.attr("__str__")());
  
  log_debug("setting %s (%s => %s) in config record",
            module.c_str(), parameter.c_str(), value_as_string.c_str());

  return true;
}

#include <boost/preprocessor.hpp>

const I3Context* I3Tray::active_context_;

const I3Context&
I3Tray::GetActiveContext() 
{
  return *active_context_;
}

bool
I3Tray::SomeContextIsActive()
{
  return active_context_;
}

void 
I3Tray::SetActiveContext(const I3Context* newactive)
{
  active_context_ = newactive;
}

const I3Context& GetActiveContext() 
{
  return I3Tray::GetActiveContext();
}

// test commit.

