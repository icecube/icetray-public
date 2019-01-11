/**
 *  $Id$
 *  
 *  Copyright (C) 2003-2007
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
#ifndef ICETRAY_I3CONTEXT_H_INCLUDED
#define ICETRAY_I3CONTEXT_H_INCLUDED

#include <string>

#include <icetray/IcetrayFwd.h>
#include <icetray/I3Logging.h>
#include <icetray/I3DefaultName.h>

#include <icetray/is_shared_ptr.h>
#include <I3/name_of.h>
#include <I3/hash_map.h>

#include <boost/any.hpp>
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

  I3Context();

  virtual ~I3Context();

  template <typename Service>
  DSOLOCAL
  bool
  Has (const std::string& where = I3DefaultName<Service>::value(),
       typename boost::disable_if<is_shared_ptr<Service>, bool>::type* enabler = 0) const
  {
    //    log_trace("%s at %s", __PRETTY_FUNCTION__, where.c_str());
    map_t::const_iterator iter = map_.find(where);
    if (iter == map_.end())
      {
	log_trace("not found @ %s", where.c_str());
	return false;
      }
    return boost::any_cast<shared_ptr<Service> >(&(iter->second));
  }

  template <typename Service>
  DSOLOCAL
  bool
  Has (const std::string& where = I3DefaultName<typename Service::value_type>::value(),
       typename boost::enable_if<is_shared_ptr<Service>, bool>::type* enabler = 0) const
  {
    //    log_trace("%s at %s", __PRETTY_FUNCTION__, where.c_str());
    map_t::const_iterator iter = map_.find(where);
    if (iter == map_.end())
      {
	log_trace("not found @ %s", where.c_str());
	return false;
      }
    return boost::any_cast<Service>(&(iter->second));
  }

  template <typename T>
  DSOLOCAL
  bool
  Put (boost::shared_ptr<T> what, const std::string& where = I3DefaultName<T>::value())
  {
    if (map_.find(where) != map_.end())
      log_fatal("context already contains object named %s", where.c_str());
    map_[where] = what;
    return true;
  }
    
  template <typename T>
  DSOLOCAL
  bool
  Put (const std::string& where, boost::shared_ptr<T> what)
  {
    return Put(what, where);
  }

  template <typename T>
  DSOLOCAL
  T&
  Get (const std::string& where = I3DefaultName<T>::value(),
       typename boost::disable_if<is_shared_ptr<T>, bool>::type* enabler = 0) const
  {
    //    log_trace("%s at %s", __PRETTY_FUNCTION__, where.c_str());
    map_t::const_iterator iter = map_.find(where);
    if (iter == map_.end())
      log_fatal("context contains nothing at slot %s", where.c_str());
    shared_ptr<T> sp_t;

    try {
      sp_t = boost::any_cast<shared_ptr<T> >(iter->second);
    } catch (const boost::bad_any_cast& e) {
      log_fatal("bad any cast getting object \"%s\" out of context as \"%s\"",
		where.c_str(), I3::name_of<T>().c_str());
    }

    if (!sp_t)
      log_fatal("didn't get thing out of context at slot %s correctly", where.c_str());
    return *sp_t;
  }

  template <typename T>
  DSOLOCAL
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
    try {
      sp_t = boost::any_cast<T>(iter->second);
    } catch (const boost::bad_any_cast& e) {
      log_trace("cast failed, returning null");
      sp_t = T();
    }
    return sp_t;
  }

 private:

  DSOLOCAL void dump() const;

  typedef __gnu_cxx::hash_map<std::string, boost::any> map_t;
  map_t map_;

  DSOLOCAL I3Context(const I3Context& rhs); // stop default
  DSOLOCAL I3Context& operator=(const I3Context& rhs); // stop default

};

typedef boost::shared_ptr<I3Context> I3ContextPtr;
typedef boost::shared_ptr<const I3Context> I3ContextConstPtr;

#endif // I3CONTEXT_H
