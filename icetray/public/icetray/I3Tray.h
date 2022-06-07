/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  Simon Patton
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
#ifndef ICETRAY_I3TRAY_H_INCLUDED
#define ICETRAY_I3TRAY_H_INCLUDED

#include <signal.h>

#include <map>
#include <string>
#include <exception>
#include <functional>
#include <iostream>
#include <icetray/IcetrayFwd.h>

#include <icetray/I3Configuration.h>
#include <icetray/I3TrayInfo.h>
#include <icetray/I3Module.h>
#include <icetray/I3Context.h>
#include <icetray/init.h>
#include <icetray/is_shared_ptr.h>

#include <boost/mpl/or.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/type_traits/is_base_of.hpp>

class I3ServiceFactory;

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
    param_setter(I3Tray &tray, const std::string& module_name) 
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
  AddService(const std::string& clazz, std::string name="");

  /**
   * Adds the specified I3ServiceFactory to the framework.
   */
  template <class Type>
  param_setter 
  AddService(std::string name="");


  template <class Type>
  typename boost::enable_if<boost::is_base_of<I3Module,Type>,param_setter>::type
  AddModule(std::string name="");
  param_setter
  AddModule(boost::python::object obj, std::string instancename="");
  param_setter
  AddModule(const std::string& name, std::string instancename="");
  param_setter
  AddModule(I3ModulePtr module, std::string instancename="");
  /**
   * Adds an arbitrary object as a module which is callable with an I3FramePtr
   * and returns nothing or a boolean result. 
   */
  template<typename Type>
  typename boost::disable_if<boost::mpl::or_<boost::is_base_of<I3Module,Type>,
                                             boost::is_same<boost::python::object,Type>,
                                             boost::is_convertible<Type,std::string>,
                                             is_shared_ptr<Type> >,
                             param_setter>::type
  AddModule(Type func, std::string instancename="");

  void MoveModule(const std::string& name, const std::string& anchor, bool before=true);

  /**
   * Connects the specified OutBox to the specified InBox. If not name is
   * specified for the InBox then the OutBox will be connected to the
   * default InBox of the module associated with the 'toModule' name.
   *
   * @param fromModule the name to associated with the OutBox's module.
   * @param fromOutBox the name of the OutBox.
   * @param toModule the name to associated with the InBox's module.
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
  std::map<std::string, I3PhysicsUsage> Usage(); 

  /**
   * Finishes everything.  It is assumed that if the modules are to be used
   * again that they will be freshly 'Configured'
   */
  void Finish();

  /**
   * Get the tray info object for this tray.
   */
  I3TrayInfo TrayInfo();

  /**
   * Return the tray's master context. This allows manual inspection and
   * addition of services.
   */
  I3Context &GetContext();

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
  SetParameter(const std::string& module,
	       const std::string& parameter,
	       const Type &value)
  {
    return this->SetParameter(module, parameter, boost::python::object(value));
  }
	
  void RequestSuspension() { suspension_requested=true; }

private:

  I3Tray(const I3Tray& rhs); // stop default 
  I3Tray& operator=(const I3Tray& rhs); // stop default

  boost::shared_ptr<I3Module> CreateModule(I3Context& context,
				    const std::string& clazz);

  boost::shared_ptr<I3ServiceFactory> CreateService(I3Context& context,
					     const std::string& clazz);


  /**
   * Connects modules in a simple chain, each's outbox to the next's
   * default inbox, for use in simple module chain situaitons so taht
   * the order of AddModule can also be  used to set the outbox connections.
   * 
   */
  void ConnectBoxesInOrderTheyWereAdded();

  bool SetParameterFailure(const std::string& module, const std::string& parameter);

  /**
     called just before Execute starts.
  */
  void Configure();

  /**
   * called by Execute() or Execute(unsigned maxCount)
   *
   * @note
   * Calling Execute(std::numeric_limits<unsigned>::max())
   * from Execute() may look like a good idea - a huge maxCount
   * means more or less 'forever' - but it isn't.
   * It's like O(10 days) run-time for some trays only!
   * Add a boolean parameter to disable the testing of maxCount
   * against the number of frames that have been processed so far.
   *
   * @param executeForever disable maxCount and run/execute forever
   * if true.
   * @param maxCount the maximum number of frames to process.
   */
  void Execute(bool executeForever, unsigned maxCount);

  /**
   * Generates an Abort transition in the main module if it is in the
   * appropraite state. Otherwise it does nothing.
   */
  void Abort();
  
  /**
   * Generate a new name for a module or service which had none specified.
   * 
   * @param type the type of object being added (e.g. I3Reader)
   * @param kind the cateogry of object being added (e.g. Module or Service)
   * @param existingNames the names already in use by objects of the same kind
   */
  std::string CreateName(const std::string& type, const std::string& kind,
                         const std::vector<std::string>& existingNames);
  
  /*
   * Overloading doesn't work properly with boost::function types, so we
   * depend on AddModule deducing the return type and then dispatching to
   * this helper function
   */
  template<typename RetType>
  param_setter
  AddFunctionModule(std::function<RetType(boost::shared_ptr<I3Frame>)>,
                    const std::string& instancename);

  /** Context, modules, and factories: oh my! */
  I3Context master_context;
  std::map<std::string,I3ServiceFactoryPtr> factories;
  std::vector<std::string> factories_in_order;

  std::map<std::string,I3ModulePtr> modules;
  std::vector<std::string> modules_in_order;
  I3ModulePtr driving_module;

  bool boxes_connected;
  bool configure_called;
  bool execute_called;
  bool suspension_requested;

  SET_LOGGER("I3Tray");

  static volatile sig_atomic_t global_suspension_requested;

  static void set_suspend_flag(int sig);
  static void die_messily(int sig);
  static void report_usage(int sig);

  friend void I3Module::Do(void (I3Module::*)());

  friend class I3TrayInfoService;
};

std::ostream& operator<<(std::ostream& os, I3Tray& tray);

template <class Type>
typename boost::enable_if<boost::is_base_of<I3Module,Type>,I3Tray::param_setter>::type
I3Tray::AddModule(std::string instancename)
{
  return this->AddModule(I3::name_of<Type>(), instancename);
}

template<typename Type>
typename boost::disable_if<boost::mpl::or_<boost::is_base_of<I3Module,Type>,
                                           boost::is_same<boost::python::object,Type>,
                                           boost::is_convertible<Type,std::string>,
                                           is_shared_ptr<Type> >,
                           I3Tray::param_setter>::type
I3Tray::AddModule(Type func, std::string instancename){
  typedef typename boost::result_of<Type(boost::shared_ptr<I3Frame>)>::type ResultType;
  return this->AddFunctionModule<ResultType>(func, instancename);
}

template <class Type>
I3Tray::param_setter 
I3Tray::AddService(std::string instancename)
{
  return this->AddService(I3::name_of<Type>(), instancename);
}

template<typename RetType>
I3Tray::param_setter
I3Tray::AddFunctionModule(std::function<RetType(boost::shared_ptr<I3Frame>)>,
                          const std::string& instancename){
  BOOST_STATIC_ASSERT(sizeof(RetType) == 0 &&
    "Only callable objects returning void or bool may be added to I3Tray as modules");
  return param_setter(*this,"never_used");
}
template<>
I3Tray::param_setter
I3Tray::AddFunctionModule<void>(std::function<void(boost::shared_ptr<I3Frame>)>,
                                const std::string& instancename);
template<>
I3Tray::param_setter
I3Tray::AddFunctionModule<bool>(std::function<bool(boost::shared_ptr<I3Frame>)>,
                                const std::string& instancename);

class sigint_exception : public std::exception
{
public:
  virtual const char* what() const throw()
  {
    return "SIGINT called";
  }
};

#endif
