/**
 *  $Id$
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
#ifndef I3SERVICEFACTORY_H
#define I3SERVICEFACTORY_H

#include <icetray/I3Context.h>
#include <icetray/I3Configuration.h>
#include <boost/python/object.hpp>
#include <boost/python/extract.hpp>
#include <string>
 
/**
 * @brief This class defineds the interfaces used to install a service into an
 * I3Tray object.
 *
 * @version $Id$
 * @author patton
 */
class I3ServiceFactory
{
public:

  I3ServiceFactory(const I3Context& context);

  // Constructors and destructor

  virtual ~I3ServiceFactory();

  // public member functions

  /**
   * Initialize a service object (initialize and parameters coming from I3Tray
   * and install the service object)
   *
   * @param services the I3Services into which the service should be installed.
   * @return true if the services is successfully installed.
   */
  bool InitializeService(I3Context& services);

  /**
   * Installed this objects service into the specified services object
   * @param services the I3Services into which the service should be installed.
   * @return true if the services is successfully installed.
   */
  virtual bool InstallService(I3Context& services) = 0;

  /**
   * The purpose of this transition is to give this object an opportunity to
   * access all of its parameters so that it will be able to process data.
   * This transition is executed immediately before the first 'Process'
   * transition, thus any parameters that has been set, either by a person
   * or a steering file, will be available to this object before it starts
   * processing data frames. After this transition any changes to these
   * parameters will not be seen by the service factory until a 'Reconfigure'
   * transition.
   *
   * This transition also gives this object an opportunity to reserve any
   * resources it will need to use during the subsequent 'Process'
   * transitions.
   */
  virtual void Configure();
  const I3Configuration &GetConfiguration() {return configuration_;}

  /**
   * The purpose of this transition is to give this object the opportunity to
   * re-access all of its parameters as they may have changed since the
   * previous 'Configure' or 'Reconfigure' transition. In many cases this
   * may simply take the form of repeating whatever was executed at the
   *'Configure' transition.
   *
   * Of course, this transition, like the 'Configure' transition, also gives
   * this object the opportunity to reserve any resources that may have been
   * released during the 'Suspend' transition.
   */
  virtual void Reconfigure();

  /**
   * The purpose of this transition is to give this object the opportunity to
   * wind up gracefully. For example a service factory can use this transition
   * to create a summary of its activities during the execution of the job.
   * Note that after this transition the service factory is still in existence.
   */
  virtual void Finish();


  /**
   * Signals to the Framework that an I3ServiceFactory has encountered a fatal
   * error and that it wishes the Framework to terminate execution by
   * sending 'Abort' transistions to all modules.
   *
   * @param message the message explaining what cause the Fatal call.
   * @param status the status value to use on exit.
   */
  void Fatal(const std::string& message,
             int status = EXIT_FAILURE) const;

  

  /**
     Adds a new parameter to the local configuration.
    
     @param parameter the name of the parameter.
     @param description a desciption of the parameter to the users.
     @param defaultValue the default value of the parameter.
     @return void

     Values are converted to/from strings by boost::lexical_cast<>

  */
  template <typename T>
  void AddParameter(const std::string& parameter,
                    const std::string& description,
                    const T& defaultValue)
  {
    boost::python::object o(defaultValue);
    configuration_.Add(parameter, description, o);
  }

  /**
     Gets the value of some parameter from the configuraiton.
     @param name the name of the parameter.
     @param value where to put the value.
     @return void
   
     Values are converted to/from strings by boost::lexical_cast<>.
  */
  template <typename T>
  typename boost::disable_if<boost::is_const<T>, void>::type
  GetParameter(const std::string& name, T& value) const
  {
    try {
      value = configuration_.Get<T>(name);
    } catch (...) {
      try {
        std::string context_name = configuration_.Get<std::string>(name);
        value = context_.Get<T>(context_name);
        // NB: we got here by catching an error thrown by boost::python::extract(). 
        // All subsequent calls will fail unless we clean it up. 
        PyErr_Clear(); 
      } catch (...) {
        log_error("Error in %s service '%s', getting parameter '%s'",
                I3::name_of(typeid(*this)).c_str(), GetName().c_str(),
                name.c_str());
        throw;
      }
    }
  }

  const std::string GetName() const { return name_; }

  virtual void SetName(const std::string& name) { name_ = name; }

  SET_LOGGER("I3ServiceFactory");

protected:

  const I3Context& context_;
  I3Configuration configuration_;
  friend class I3Tray;

private:

  std::string name_;

  I3ServiceFactory(const I3ServiceFactory& rhs); // stop default
  I3ServiceFactory& operator=(const I3ServiceFactory& rhs); // stop default

};

typedef boost::shared_ptr<I3ServiceFactory> I3ServiceFactoryPtr;

#include <icetray/I3Factory.h>

#endif // I3SERVICEFACTORY_H
