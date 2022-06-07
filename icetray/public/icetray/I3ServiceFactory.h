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

 I3ServiceFactory(const I3Context& context) :
  context_(context){};

  virtual ~I3ServiceFactory(){};

  /**
   * Initialize a service object (initialize and parameters coming from I3Tray
   * and install the service object)
   *
   * @param services the I3Services into which the service should be installed.
   * @return true if the services is successfully installed.
   */
  bool InitializeService(I3Context& services){
    return InstallService(services);
  };

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
   * processing data frames. 
   *
   * This transition also gives this object an opportunity to reserve any
   * resources it will need to use during the subsequent 'Process'
   * transitions.
   */
  virtual void Configure(){};
  const I3Configuration &GetConfiguration() const {return configuration_;}
  I3Configuration &GetConfiguration() {return configuration_;}

  /**
   * The purpose of this transition is to give this object the opportunity to
   * wind up gracefully. For example a service factory can use this transition
   * to create a summary of its activities during the execution of the job.
   * Note that after this transition the service factory is still in existence.
   */
  virtual void Finish(){
    log_trace("%s", __PRETTY_FUNCTION__);
  };

  /**
     Adds a new parameter to the local configuration.
    
     @param parameter the name of the parameter.
     @param description a desciption of the parameter to the users.
     @param defaultValue the default value of the parameter.

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

private:

  std::string name_;

  I3ServiceFactory(const I3ServiceFactory& rhs); // stop default
  I3ServiceFactory& operator=(const I3ServiceFactory& rhs); // stop default

};

typedef boost::shared_ptr<I3ServiceFactory> I3ServiceFactoryPtr;

#include <icetray/I3Factory.h>

#endif // I3SERVICEFACTORY_H
