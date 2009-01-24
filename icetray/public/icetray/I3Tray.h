/**
 *  $Id: I3Tray.h 47987 2008-08-08 14:09:48Z troy $
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  Simon Patton
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
#ifndef ICETRAY_I3TRAY_H_INCLUDED
#define ICETRAY_I3TRAY_H_INCLUDED

#include <map>
#include <string>
#include <exception>
#include <icetray/IcetrayFwd.h>

#include <icetray/I3Configuration.h>
#include <icetray/I3TrayInfo.h>
#include <icetray/I3Module.h>
#include <icetray/I3Context.h>

class I3ServiceFactory;

using namespace std;

/**
   This is I3Tray.

   The I3Tray provides a certain configuration interface:

   @code
   I3Tray tray;

   tray.AddService("TestServiceFactory", "service")
      ("boolParam", true)
      ("intParam",  52)
      ("floatParam", 1.2)
      ("doubleParam", 4.14159)
      ("longParam", 68);

    tray.AddModule("TestModule", "test")
      ("boolParam", false)
      ("intParam", 53)
      ("floatParam", 1.1)
      ("doubleParam", 3.14159)
      ("stringParam", "it puts the lotion in the basket")
      ("longParam", 67);

    tray.Execute();

    @endcode

    The modules and service will be created and added to the tray in
    the order that they appear.  The parenthesized ("variable-name",
    @em variable-value) pairs will become calls to @c SetParameter for
    the module that they are associated with,
    e.g. "TestServiceFactory", above will have it's "doubleParam" set
    to 4.14159.

    If, as above, <tt>tray.ConnectBoxes()</tt> is not called by the
    user, each module's @em OutBox will be connected to the following
    Module's @em InBox, in the order they appear, before Execute()
    begins.  If it is called at all, no default ConnectBoxes will be
    performed.

 */

class I3Tray
{

public:

  /**
   * local convenience class: functor that eats pairs of arguments and
   * calls SetParameter.  For terseness in creating/configuring
   * I3Trays.
   */
  class param_setter 
  {
    I3Tray& tray_;
    const std::string module_name_;

  public:
    param_setter(I3Tray &tray, const string& module_name) 
      : tray_(tray), module_name_(module_name) { }
    param_setter(const param_setter& rhs)
      : tray_(rhs.tray_), module_name_(rhs.module_name_) { }
  
    template <typename T>
    param_setter& operator()(const std::string& param, T value)
    {
      log_trace("%s", __PRETTY_FUNCTION__);
      tray_.SetParameter(module_name_.c_str(), param.c_str(), value);
      return *this;
    }
  };

  I3Tray();  

  ~I3Tray();

  /**
   * Adds the specified I3ServiceFactory to the framework.
   *
   * @param clazz the class of the I3ServiceFactory.
   * @param name the name associated with services the created by the factory.
   */
  param_setter 
  AddService(const std::string& clazz, const std::string& name);

  /**
   * Adds the specified I3ServiceFactory to the framework.
   */
  template <class Type>
  param_setter 
  AddService(const std::string& name);


  template <class Type>
  param_setter 
  AddModule(const std::string& name);


  param_setter
  AddModule(boost::python::object obj, const std::string& instancename);

  param_setter
  AddModule(const std::string& name, const std::string& instancename);

  /**
   * Connects the specified OutBox to the specified InBox. If not name is
   * specified for the InBox then the OutBox will be connected to the
   * default InBox of the module associated with the 'toModule' name.
   *
   * @param fromModule the name to associated with the OutBox's module.
   * @param fromOutBox the name of the OutBox.
   * @param toModule the name to associated with the InBox's module.
   * @param toInBox the name of the InBox.
   */
  bool ConnectBoxes(const std::string& fromModule,
		    const std::string& fromOutBox,
		    const std::string& toModule);

  /**
     Executes a procesing loop which will continue until there is no more
     data to process (that is, a module invokes RequestSuspension).
  */
  void Execute(); 

  /**
     Runs the tray until the specified maximum count is reached, or a
     module invokes RequestSuspension();
   
     @param maxCount the maximum process transistions for the driving module.
  */
  void Execute(unsigned maxCount);

  /**
     Report per-module physics ncalls/system/user time usage.  Have to call this
     *after* Execute()
  */
  map<string, I3PhysicsUsage> Usage(); 

  /**
   * Finishes everything.  It is assumed that if the modules are to be used
   * again that they will be freshly 'Configured'
   */
  void Finish();

  bool
  SetParameter(const std::string& module,
	       const std::string& parameter,
	       const char* value);

  bool
  SetParameter(const std::string& module,
	       const std::string& parameter,
	       boost::python::object value);

  template <class Type>
  bool 
  SetParameter(const string& module,
	       const string& parameter,
	       const Type &value)
  {
    return this->SetParameter(module, parameter, boost::python::object(value));
  }

private:

  I3Tray(const I3Tray& rhs); // stop default 
  I3Tray& operator=(const I3Tray& rhs); // stop default

  shared_ptr<I3Module> CreateModule(I3Context& context,
				    const std::string& clazz);

  shared_ptr<I3ServiceFactory> CreateService(I3Context& context,
					     const std::string& clazz);


  /**
   * Connects modules in a simple chain, each's outbox to the next's
   * default inbox, for use in simple module chain situaitons so taht
   * the order of AddModule can also be  used to set the outbox connections.
   * 
   */
  void ConnectBoxesInOrderTheyWereAdded();

  bool SetParameterFailure(const string& module, const string& parameter);

  /**
   * initialize services
   */
  void InitializeServices(I3Context & context);

  /**
   * loops over all services and tries configures them if they
   * haven't been configured yet.
   */
  void ConfigureServices();
  
  /**
     called just before Execute starts.
  */
  void Configure();

  /**
   * Generates an Abort transition in the main module if it is in the
   * appropraite state. Otherwise it does nothing.
   */
  void Abort();

  /** The set of services to load into each context. */
  map<string,I3ServiceFactoryPtr> factories;
  map<string,I3ContextPtr> factory_contexts;
  map<string,bool> configuredFactories;
  std::vector<std::string> factories_in_order;

  map<string,I3ContextPtr> module_contexts;
  map<string,I3ModulePtr> modules;
  std::vector<std::string> modules_in_order;
  I3ModulePtr driving_module;

  bool boxes_connected;
  bool driving_module_already_added;

  SET_LOGGER("I3Tray");

  static bool suspension_requested_;

  friend void I3Module::RequestSuspension() const;

  static void RequestSuspension() { suspension_requested_ = true; }

  static const I3Context* active_context_;

  friend void I3Module::Do(void (I3Module::*)());

  friend class I3TrayInfoService;

public:

  static bool SomeContextIsActive();
  static const I3Context& GetActiveContext();
  static void SetActiveContext(const I3Context* newactive);
};

template <class Type>
I3Tray::param_setter 
I3Tray::AddModule(const std::string& instancename)
{
  return this->AddModule(I3::name_of<Type>(), instancename);
}

template <class Type>
I3Tray::param_setter 
I3Tray::AddService(const std::string& instancename)
{
  return this->AddService(I3::name_of<Type>(), instancename);
}

// Use argument
template <class T>
T& 
GetService(const std::string &where = I3DefaultName<T>::value(), 
	   typename boost::disable_if<is_shared_ptr<T>, bool>::type* enabler = 0)
{
  const I3Context& context = I3Tray::GetActiveContext();

  //Fix to get around the 3.2 compiler
  return context.template Get<T>(where);
}

template <class T>
T 
GetService(const std::string &where = I3DefaultName<typename T::value_type>::value(),
	   typename boost::enable_if<is_shared_ptr<T>, bool>::type* enabler = 0)
{
  const I3Context& context = I3Tray::GetActiveContext();

  //Fix to get around the 3.2 compiler
  return context.template Get<T>(where);
}

#endif
