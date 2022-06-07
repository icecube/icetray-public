/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
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
#ifndef ICETRAY_I3PARAMETER_H_INCLUDED
#define ICETRAY_I3PARAMETER_H_INCLUDED

#include <icetray/IcetrayFwd.h>
#include <boost/python/object.hpp>
#include <boost/optional.hpp>
#include <string>
#include <icetray/serialization.h>
	
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
  friend class icecube::serialization::access;

  template <typename Archive>
  void
  serialize (Archive& ar, unsigned version);
};

std::ostream& operator<<(std::ostream&, const I3Parameter&); 

#endif
