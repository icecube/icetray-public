/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
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
#ifndef ICETRAY_I3PARAMETER_H_INCLUDED
#define ICETRAY_I3PARAMETER_H_INCLUDED

#include <icetray/IcetrayFwd.h>
#include <boost/python/object.hpp>
#include <boost/optional.hpp>
#include <string>
	
struct I3Parameter
{
  std::string name_;
  std::string description_;

  mutable bool got_by_module_;

  boost::optional<boost::python::object> default_;
  boost::optional<boost::python::object> configured_;

public:
  
  I3Parameter();
  I3Parameter(const I3Parameter& rhs);
  I3Parameter& operator=(const I3Parameter&);

  ~I3Parameter();
  const std::string& name() const { return name_; } 
  void name(const std::string& newname) { name_ = newname; }

  std::string description() const { return description_; }
  void description(const std::string& d) { description_ = d; }

  std::string default_value_str() const; 
  std::string configured_value_str() const;

  bool has_default() const { return bool(default_); }
  bool has_configured() const { return bool(configured_); }
  bool got_by_module() const { return got_by_module_; }

  void set_configured(const boost::python::object& t);
  void set_default(const boost::python::object& t);
  boost::python::object value() const;

private:
  friend class boost::serialization::access;

  template <typename Archive>
  void
  serialize (Archive& ar, unsigned version);
};

std::ostream& operator<<(std::ostream&, const I3Parameter&); 

#endif
