/**
 *  $Id$
 *  
 *  Copyright (C) 2003-2007
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
#ifndef ICETRAY_I3CONTEXT_H_INCLUDED
#define ICETRAY_I3CONTEXT_H_INCLUDED

#include <string>
#include <vector>

#include <icetray/IcetrayFwd.h>
#include <icetray/I3Logging.h>
#include <icetray/I3DefaultName.h>

#include <icetray/is_shared_ptr.h>
#include <I3/name_of.h>
#include <I3/hash_map.h>

#include <boost/any.hpp>
#include <boost/python.hpp>
#include <boost/utility/enable_if.hpp>

/**
 * @brief This class defines the interface used by I3ContextAccess to gain 
 * type-safe access to a module's context. 
 *
 * Users should not directly access data using
 * this class, but should hand instances of this class to I3ContextAccess to
 * handle such issues.
 *
 * @version $Id$
 * @author patton
 */
class I3Context
{

 public:

  I3Context(){};

  virtual ~I3Context(){};

  std::vector<std::string> keys() const;

  template <typename Service>
  bool
  Has (const std::string& where = I3DefaultName<Service>::value(),
       typename boost::disable_if<is_shared_ptr<Service>, bool>::type* enabler = 0) const
  {
    //    log_trace("%s at %s", __PRETTY_FUNCTION__, where.c_str());
    map_t::const_iterator iter = map_.find(where);
    if (iter == map_.end()) {
	log_trace("not found @ %s", where.c_str());
	return false;
    }
    try {
        boost::python::object obj;
        obj = boost::any_cast<boost::python::object>(iter->second);
        return boost::python::extract<boost::shared_ptr<Service> >(obj).check();
    } catch (const boost::bad_any_cast& e) {
        return boost::any_cast<boost::shared_ptr<Service> >(&(iter->second));
    }

    return false;
  }

  template <typename Service>
  bool
  Has (const std::string& where = I3DefaultName<typename Service::element_type>::value(),
       typename boost::enable_if<is_shared_ptr<Service>, bool>::type* enabler = 0) const
  {
    //    log_trace("%s at %s", __PRETTY_FUNCTION__, where.c_str());
    map_t::const_iterator iter = map_.find(where);
    if (iter == map_.end()) {
	log_trace("not found @ %s", where.c_str());
	return false;
    }
    try {
        boost::python::object obj;
        obj = boost::any_cast<boost::python::object>(iter->second);
        return boost::python::extract<Service>(obj).check();
    } catch (const boost::bad_any_cast& e) {
        return boost::any_cast<Service>(&(iter->second));
    }

    return false;
  }

  bool
  Has (const std::string& where) const
  {
    //    log_trace("%s at %s", __PRETTY_FUNCTION__, where.c_str());
    map_t::const_iterator iter = map_.find(where);
    if (iter == map_.end())
      {
	log_trace("not found @ %s", where.c_str());
	return false;
      }
    return true;
  }

  template <typename T>
  bool
  Put (boost::shared_ptr<T> what, const std::string& where = I3DefaultName<T>::value())
  {
    if (map_.find(where) != map_.end())
      log_fatal("context already contains object named %s", where.c_str());
    try {
      map_[where] = boost::python::object(what);
    } catch (const boost::python::error_already_set &e) {
      map_[where] = what;
      PyErr_Clear();
    }
    return true;
  }
    
  template <typename T>
  bool
  Put (const std::string& where, boost::shared_ptr<T> what)
  {
    return Put(what, where);
  }

  bool
  Put (const std::string& where, boost::python::object what)
  {
    if (map_.find(where) != map_.end())
      log_fatal("context already contains object named %s", where.c_str());
    map_[where] = what;
    return true;
  }

  template <typename T>
  T&
  Get (const std::string& where = I3DefaultName<T>::value(),
       typename boost::disable_if<is_shared_ptr<T>, bool>::type* enabler = 0) const
  {
    //    log_trace("%s at %s", __PRETTY_FUNCTION__, where.c_str());
    map_t::const_iterator iter = map_.find(where);
    if (iter == map_.end())
      log_fatal("context contains nothing at slot %s", where.c_str());
    boost::shared_ptr<T> sp_t;
    boost::python::object obj;
    try {
      obj = boost::any_cast<boost::python::object>(iter->second);
      sp_t = boost::python::extract<boost::shared_ptr<T> >(obj);
    } catch (const boost::bad_any_cast &e) {
      try {
        sp_t = boost::any_cast<boost::shared_ptr<T> >(iter->second);
      } catch (const boost::bad_any_cast &e) {
        log_fatal("error getting object \"%s\" out of context as \"%s\"",
	  	  where.c_str(), I3::name_of<T>().c_str());
        sp_t = boost::shared_ptr<T>();
      }
    } catch (const boost::python::error_already_set &e) {
      PyErr_Clear();
      log_fatal("error getting object \"%s\" out of context as \"%s\"",
		where.c_str(), I3::name_of<T>().c_str());
      sp_t = boost::shared_ptr<T>();
    }

    if (!sp_t)
      log_fatal("didn't get thing out of context at slot %s correctly", where.c_str());
    return *sp_t;
  }

  template <typename T>
  T
  Get (const std::string& where = I3DefaultName<T>::value(),
       typename boost::enable_if<is_shared_ptr<T> >::type * = 0) const
  {
    //    log_trace("%s at %s", __PRETTY_FUNCTION__, where.c_str());
    map_t::const_iterator iter = map_.find(where);
    if (iter == map_.end())
      {
	log_trace("context contains nothing at slot %s", where.c_str());
	return T();
      }
    T sp_t;
    boost::python::object obj;
    try {
      obj = boost::any_cast<boost::python::object>(iter->second);
      sp_t = boost::python::extract<T>(obj);
    } catch (const boost::bad_any_cast &e) {
      try {
        sp_t = boost::any_cast<T>(iter->second);
      } catch (const boost::bad_any_cast &e) {
        log_trace("cast failed, returning null");
        sp_t = T();
      }
    } catch (const boost::python::error_already_set &e) {
      PyErr_Clear();
      log_trace("cast failed, returning null");
      sp_t = T();
    }
    return sp_t;
  }

  boost::python::object
  Get (const std::string &where) const
  {
    map_t::const_iterator iter = map_.find(where);
    if (iter == map_.end()) {
	log_trace("context contains nothing at slot %s", where.c_str());
	return boost::python::object();
    }
    try {
      return boost::any_cast<boost::python::object>(iter->second);
    } catch (const boost::bad_any_cast& e) {
      std::string error_message = "Context item at ";
      error_message += where;
      error_message += " of type ";
      error_message += I3::name_of(typeid(iter->second));
      error_message += " has no loaded pybindings";
      PyErr_SetString(PyExc_TypeError, error_message.c_str());
      boost::python::throw_error_already_set();
    }

    return boost::python::object();
  }
  
  typedef hash_map<std::string, boost::any> map_t;
  typedef map_t::const_iterator const_iterator;
  const_iterator begin() const { return map_.begin(); }
  const_iterator end() const { return map_.end(); }

  std::string dump() const;

 private:

  map_t map_;

  I3Context(const I3Context& rhs); // stop default
  I3Context& operator=(const I3Context& rhs); // stop default

};

typedef boost::shared_ptr<I3Context> I3ContextPtr;
typedef boost::shared_ptr<const I3Context> I3ContextConstPtr;

#endif // I3CONTEXT_H
