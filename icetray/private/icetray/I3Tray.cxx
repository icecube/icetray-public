/**
 *  $Id$
 *
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  Simon Patton
 *  John Pretz
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *  
 *  SPDX-License-Identifier: BSD-2-Clause
 *  
 */
#include <signal.h>

#include <string>
#include <map>
#include <iostream>
#include <exception>
#include <deque>

#include <boost/python.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

#include <icetray/I3Tray.h>
#include <icetray/I3TrayInfoService.h>
#include <icetray/I3Module.h>
#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <icetray/I3PhysicsUsage.h>
#include <icetray/serialization.h>

#ifdef MEMORY_TRACKING
#include <icetray/memory.h>
#endif

#include "PythonFunction.h"
#include "FunctionModule.h"

using namespace std;

volatile sig_atomic_t I3Tray::global_suspension_requested;

namespace bp = boost::python;

void
I3Tray::set_suspend_flag(int sig)
{
	global_suspension_requested = 1;

	if (sig == SIGINT) {
		std::cerr << "\n***\n*** SIGINT received. "
		    "Calling Finish() at the end of the current frame. \n"
		    "*** Hit ^C again to force quit.\n***\n";
		signal(SIGINT, die_messily);
	}
}

void
I3Tray::die_messily(int sig)
{
	if (sig == SIGINT) {
		std::cerr << "\n***\n*** SIGINT received again; "
		    "exiting immediately. \n***\n";
		exit(1);
	}
}

namespace {

class scoped_signal_handler {
public:
    scoped_signal_handler(int signum, void (*handler)(int))
    {
        signum_ = signum;
        current_.sa_handler = handler;
        sigaction(signum_, &current_, &previous_);
    }
    ~scoped_signal_handler()
    {
        sigaction(signum_, &previous_, &current_);
    }
private:
    int signum_;
    struct sigaction current_, previous_;
};

}

I3Tray *executing_tray = NULL;

void
I3Tray::report_usage(int sig)
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
		log_info("%40s: %6u calls to daq + physics %9.2fs user %9.2fs system",
		    name.c_str(), ru.ncall, ru.usertime, ru.systime);
		if ((acc_time += ru.usertime)/total_time > 0.9)
			break;
	}
}

void noOpDeleter(I3Tray*){}

I3Tray::I3Tray() :
    boxes_connected(false), configure_called(false),
    execute_called(false), suspension_requested(false)
{
#ifdef MEMORY_TRACKING
	memory::set_scope("I3Tray");
#endif
	master_context.Put(boost::shared_ptr<I3Tray>(this,noOpDeleter),"I3Tray");
	// Note that the following is deeply unsafe, but necessary for
	// tray info service for now
	master_context.Put(boost::shared_ptr<I3TrayInfoService>(new
	    I3TrayInfoService(this)), "__tray_info_service");
}

I3Tray::~I3Tray()
{
	global_suspension_requested = false;
}

void
I3Tray::Abort()
{
}

I3Tray::param_setter
I3Tray::AddModule(const std::string& classname, std::string instancename)
{
	if(instancename.empty())
		instancename=CreateName(classname, "Module", modules_in_order);
	return AddModule(bp::object(classname), instancename);
}

I3Tray::param_setter
I3Tray::AddModule(bp::object obj, std::string instancename)
{
	if (configure_called)
		log_fatal("I3Tray::Configure() already called -- "
		    "cannot add new modules");
	if (modules.find(instancename) != modules.end())
		log_fatal("Tray already contains module named \"%s\" of "
		    "type %s", instancename.c_str(),
		    modules[instancename]->GetConfiguration().ClassName().c_str());

	I3ModulePtr module;
	if (bp::extract<std::string>(obj).check()) {
		// obj is a string... construct C++ module from factory
		std::string name = bp::extract<std::string>(obj);
		if (instancename.empty())
			instancename=CreateName(name,"Module",modules_in_order);
		module =
		    I3::Singleton<I3ModuleFactory>::get_const_instance()
		    .Create(name)(master_context);
		module->GetConfiguration().ClassName(name);
	} else if (PyType_Check(obj.ptr())) {
		// Try to instantiate a python I3Module
		bp::object instance = obj(bp::ptr(&master_context));
		module = bp::extract<I3ModulePtr>(instance);
		std::string pyname =
		    boost::python::extract<std::string>(obj.attr("__name__"));
		if (instancename.empty())
			instancename=CreateName(pyname,"Module",modules_in_order);

		std::string pymod = boost::python::extract<std::string>(
		    obj.attr("__module__"));
		module->GetConfiguration().ClassName(pymod + "." + pyname);
	} else if (PyCallable_Check(obj.ptr())) {
		// it is a python function... put the object in the context and
		// call it a PythonFunction
		if (PyObject_HasAttrString(obj.ptr(), "im_func") &&
		    (bp::object(obj.attr("im_self")).ptr()) == Py_None)
			log_fatal("'%s%s', passed to AddModule with instance "
			    "name %s, is an unbound instance method. Only "
			    "Python functions, callable classes, and bound "
			    "instance methods can be run as I3Modules!",
			    PyEval_GetFuncName(obj.ptr()),
			    PyEval_GetFuncDesc(obj.ptr()),
			    instancename.c_str());

		module = I3ModulePtr(new PythonFunction(master_context, obj));
		std::string repr;
		if (PyObject_HasAttrString(obj.ptr(), "func_name"))
			repr = boost::python::extract<std::string>(
			    obj.attr("func_name"));
		else if (PyObject_HasAttrString(obj.ptr(), "__name__"))
			repr = boost::python::extract<std::string>(
			    obj.attr("__name__"));
		else
			repr = boost::python::extract<std::string>(
			    obj.attr("__repr__")());
		module->GetConfiguration().ClassName(repr);
		if (instancename.empty())
			instancename=CreateName(repr,"Module",modules_in_order);
	} else {
		log_fatal("'%s%s' passed to AddModule with instance name %s. "
		   "Must be a string, a python function, or a python I3Module.",
		   PyEval_GetFuncName(obj.ptr()),
		   PyEval_GetFuncDesc(obj.ptr()),
		   instancename.c_str());
	}

	log_trace("%s : %s", __PRETTY_FUNCTION__, instancename.c_str());

	module->GetConfiguration().InstanceName(instancename);
	module->SetName(instancename);
	modules[instancename] = module;
	modules_in_order.push_back(instancename);

	return param_setter(*this, instancename);
}

I3Tray::param_setter
I3Tray::AddModule(I3ModulePtr module, std::string instancename)
{
	if (configure_called)
		log_fatal("I3Tray::Configure() already called -- "
		    "cannot add new modules");
	I3Module& modRef=*module;
	module->GetConfiguration().ClassName(I3::name_of(typeid(modRef)));
	if (instancename.empty())
		instancename=CreateName(module->GetConfiguration().ClassName(),"Module",
		                        modules_in_order);
	if (modules.find(instancename) != modules.end())
		log_fatal("Tray already contains module named \"%s\" of "
		    "type %s", instancename.c_str(),
		    modules[instancename]->GetConfiguration().ClassName().c_str());

	log_trace("%s : %s", __PRETTY_FUNCTION__, instancename.c_str());

	module->GetConfiguration().InstanceName(instancename);
	module->SetName(instancename);
	modules[instancename] = module;
	modules_in_order.push_back(instancename);

	return param_setter(*this, instancename);
}

I3Tray::param_setter
I3Tray::AddService(const std::string& classname,
    std::string instancename)
{
	if (configure_called)
		log_fatal("I3Tray::Configure() already called -- "
		    "cannot add new services");

	if (instancename.empty())
		instancename=CreateName(classname,"Service",factories_in_order);

	if (factories.find(instancename) != factories.end())
		log_fatal("More than one service added with the name '%s'",
		    instancename.c_str());

	log_trace("AddService %s %s", classname.c_str(), instancename.c_str());

	i3_assert(!classname.empty());
	I3ServiceFactoryPtr servicefactory =
	    I3::Singleton<I3ServiceFactoryFactory>::get_const_instance()
	    .Create(classname)(master_context);
	servicefactory->GetConfiguration().ClassName(classname);
	servicefactory->GetConfiguration().InstanceName(instancename);
	servicefactory->SetName(instancename);

	factories[instancename] = servicefactory;
	factories_in_order.push_back(instancename);

	return param_setter(*this, instancename);
}

void
I3Tray::MoveModule(const std::string& name, const std::string& anchor,
    bool after)
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
		    (after ? "after" : "before"), anchor.c_str(),
		    anchor.c_str());
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

	map<string, I3ModulePtr>::const_iterator iter =
	    modules.find(fromModule);

	if (iter == modules.end())
		log_fatal("couldn't find module \"%s\"", fromModule.c_str());

	I3ModulePtr module = iter->second;
	log_debug("connecting outbox \"%s\" on module \"%s\" to module \"%s\"",
	    fromOutBox.c_str(), fromModule.c_str(), toModule.c_str());

	map<string, I3ModulePtr>::const_iterator toiter =
	    modules.find(toModule);
	if (toiter == modules.end())
		log_fatal("couldn't find module \"%s\"", toModule.c_str());

	module->ConnectOutBox(fromOutBox, toiter->second);
	return true;
}

void
I3Tray::Configure()
{
	if (configure_called)
		return;
	if (modules_in_order.size() == 0)
		log_fatal("Calling %s with no modules added. "
		    "You probably want some.", __PRETTY_FUNCTION__);

	//
	// Create the services in the order they were added.
	//
	BOOST_FOREACH(const string& objectname, factories_in_order) {
#ifdef MEMORY_TRACKING
		memory::set_scope(objectname);
#endif
		I3ServiceFactoryPtr factory = factories[objectname];
		try {
			factory->Configure();
		} catch (...) {
			PyObject *type, *value, *traceback;
			PyErr_Fetch(&type, &value, &traceback);
			log_error("Exception thrown while configuring "
			    "service factory \"%s\".", objectname.c_str());
			std::cerr << factory->GetConfiguration();
			PyErr_Restore(type, value, traceback);
			throw;
		}
		if (!factory->GetConfiguration().is_ok()) {
			std::cerr << factory->GetConfiguration();
			log_fatal("Error in configuration for service factory "
			    "\"%s\".  Turn up your logging to see just what.",
			    objectname.c_str());
		}
		factory->InitializeService(master_context);
	}

	//
	// Configure modules in added order
	// Note that we can't use I3Module::Do() because outboxes have not been
	// set up yet
	//
	BOOST_FOREACH(const string& objectname, modules_in_order) {
#ifdef MEMORY_TRACKING
		memory::set_scope(objectname);
#endif
		I3ModulePtr module = modules[objectname];
		try {
			module->Configure_();
		} catch (...) {
			PyObject *type, *value, *traceback;
			PyErr_Fetch(&type, &value, &traceback);
			log_error("Exception thrown while configuring "
			    "module \"%s\".", objectname.c_str());
			std::cerr << module->GetConfiguration();
			PyErr_Restore(type, value, traceback);
			throw;
		}
		if (!module->GetConfiguration().is_ok()) {
			std::cerr << module->GetConfiguration();
			log_fatal("Error in configuration for module "
			    "\"%s\".  Turn up your logging to see just what.",
			    objectname.c_str());
		}
	}

	configure_called = true;

#ifdef MEMORY_TRACKING
	memory::set_scope("I3Tray");
#endif

	//
	//  If we never explicity called ConnectBoxes, connect the
	//  modules to one another in the order they were added.
	//
	if (!boxes_connected) {
		vector<std::string>::const_iterator to =
		    ++modules_in_order.begin();
		while (to != modules_in_order.end()) {
			log_trace("%s => %s", (to-1)->c_str(), to->c_str());
			I3ModulePtr frommod = modules[*(to-1)];
			if (!frommod->HasOutBox("OutBox"))
				log_fatal("Module \'%s\' does not have an "
				    "OutBox, but has following modules.",
				    (to-1)->c_str());

			ConnectBoxes(*(to-1), "OutBox", *to);
			to++;
		}
	}

	// Find the module without an inbox and set to be the "driving" module.
	driving_module.reset();
	BOOST_FOREACH(const std::string &modname, modules_in_order) {
		I3ModulePtr module = modules[modname];
		if (!module->HasInBox()) {
			driving_module = module;
			log_debug("Driving module is %s", modname.c_str());
		}
	}
	if (!driving_module)
		log_fatal("No driving module! Have you set up a circular "
		    "tray?");

	// check that all parameters set in steering file were
	// AddParametered and GetParametered, that all modules not the
	// driving module have an inbox, and that every outbox is connected
	BOOST_FOREACH(const std::string &modname, modules_in_order) {
		I3ModulePtr module = modules[modname];
		if (!module->GetConfiguration().is_ok()) {
			std::cerr << module->GetConfiguration();
			log_fatal("Configuration error.  Turn up your logging "
			    "to see just what.");
		}
		if (module != driving_module && !module->HasInBox()){
		  log_fatal("Module %s has a disconnected inbox",modname.c_str());
		}
	}
}

void
I3Tray::Execute()
{
  Execute(true, 0u);
}

void
I3Tray::Execute(unsigned maxCount)
{
  Execute(false, maxCount);
}

void
I3Tray::Execute(bool executeForever, unsigned maxCount)
{
	if (execute_called) {
		log_error("This I3Tray has already been executed. "
		    "Ignoring second call to Execute()");
		return;
	}

	execute_called = true;
	scoped_signal_handler trap_sigint(SIGINT, set_suspend_flag);
	scoped_signal_handler trap_sigterm(SIGTERM, set_suspend_flag);

#ifdef SIGINFO
	executing_tray = this;
	scoped_signal_handler trap_siginfo(SIGINFO, report_usage);
#endif

	Configure();

	for (unsigned i=0;
	     (executeForever || (i < maxCount)) &&
		!suspension_requested &&
		!global_suspension_requested;
             i++) {
		log_trace("%u/%u icetray dispatching Process_", i, maxCount);
		driving_module->Do(&I3Module::Process_);
	}

	// call every module's Finish() function
	// (this used to be in I3Tray::Finish())
	if (modules_in_order.size() == 0 || !driving_module)
		return;

	log_notice("I3Tray finishing...");

	driving_module->Do(&I3Module::Finish);

	BOOST_FOREACH(const std::string& factname, factories_in_order) {
#ifdef MEMORY_TRACKING
		memory::set_scope(factname);
#endif
		log_trace("calling finish on factory %s", factname.c_str());
		factories[factname]->Finish();
	}
#ifdef MEMORY_TRACKING
	memory::set_scope("I3Tray");
#endif

        if (global_suspension_requested) {
                throw sigint_exception();
        }
}

map<string, I3PhysicsUsage>
I3Tray::Usage()
{
	map<string, I3ModulePtr>::iterator iter = modules.begin();
	map<string, I3PhysicsUsage> mspu;
	while(iter != modules.end()) {
		I3PhysicsUsage ru = iter->second->ReportUsage();
		log_debug("Module %s reports %f %f %u",
		    iter->first.c_str(),
		    ru.systime, ru.usertime, ru.ncall);
		mspu[iter->first] = ru;
		iter++;
	}
	return mspu;
}


void I3Tray::Finish(){}

I3TrayInfo
I3Tray::TrayInfo()
{
	I3TrayInfoService srv(this);
	return srv.GetConfig();
}

I3Context &
I3Tray::GetContext()
{
	return master_context;
}

std::ostream&
operator<<(std::ostream& os, I3Tray& tray)
{
	return (os << tray.TrayInfo());
}

// special overload to help with compiled steering files.
bool
I3Tray::SetParameter(const string& module, const string& parameter,
    const char* value)
{
	log_trace("%s", __PRETTY_FUNCTION__);
	return SetParameter(module, parameter, string(value));
}

bool
I3Tray::SetParameter(const string& module, const string& parameter,
    bp::object value)
{
	log_trace("%s", __PRETTY_FUNCTION__);

	I3Configuration *config;
	if (modules.find(module) != modules.end())
		config = &modules[module]->GetConfiguration();
	else if (factories.find(module) != factories.end())
		config = &factories[module]->GetConfiguration();
	else
		log_fatal("couldn't find configuration for \"%s\"",
		    module.c_str());

	config->Set(parameter, value);

	string value_as_string =
	    bp::extract<std::string>(value.attr("__str__")());
	log_debug("setting %s (%s => %s) in config record",
	    module.c_str(), parameter.c_str(), value_as_string.c_str());

	return true;
}

//The behavior of this function should be kept the same
//as the python I3Tray's _createName
std::string
I3Tray::CreateName(const std::string& type, const string& kind,
                   const std::vector<std::string>& existingNames){
	std::string name;
	unsigned int i=0;
	while(true){
		std::ostringstream ss;
		ss << type << '_' << std::setfill('0') << std::setw(4) << i;
		name=ss.str();
		if(std::find(existingNames.begin(),existingNames.end(),name)
		   == existingNames.end())
			break;
		i++;
	}
	log_debug_stream("Adding Anonymous " << kind << " of type '"
	                << type << "' with name '" << name << "'");
	return(name);
}

template<>
I3Tray::param_setter
I3Tray::AddFunctionModule<void>(std::function<void(boost::shared_ptr<I3Frame>)> func,
                                const std::string& instancename){
	return AddModule(boost::make_shared<FunctionModule>(master_context,func));
}

template<>
I3Tray::param_setter
I3Tray::AddFunctionModule<bool>(std::function<bool(boost::shared_ptr<I3Frame>)> func,
                                const std::string& instancename){
	return AddModule(boost::make_shared<FunctionModule>(master_context,func));
}
