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
#ifndef ICETRAY_I3CONFIGURATIONIMPL_H_INCLUDED
#define ICETRAY_I3CONFIGURATIONIMPL_H_INCLUDED

#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <icetray/IcetrayFwd.h>
#include <icetray/I3DefaultName.h>
#include <icetray/I3Parameter.h>
#include <icetray/serialization.h>
#include <iosfwd>

#define BOOST_MULTI_INDEX_DISABLE_SERIALIZATION
#include <boost/multi_index_container_fwd.hpp>
#include <boost/multi_index/ordered_index_fwd.hpp>
#include <boost/multi_index/mem_fun.hpp>

/**
 * @brief This class holds the configuration.
 */

class I3ConfigurationImpl
{

public:

  virtual ~I3ConfigurationImpl();

  I3ConfigurationImpl();

  void Set(const std::string& key, const boost::python::object& value);

  bool Has(const std::string& name) const;

  void 
  Add(const std::string& name, const std::string& description, 
      const boost::python::object& default_value);

  void 
  Add(const std::string& name, const std::string& description);

  boost::python::object
  Get(const std::string& name) const;

  I3Parameter
  GetParameter(const std::string& name) const;

  std::string ClassName() const 
  { 
    if (classname == "")
      log_fatal("classname is empty?");
    return classname; 
  }

  void ClassName(const std::string& s) 
  { 
    log_trace("classname: <<<%s>>>", s.c_str());
    if (s.empty())
      log_fatal("classname is empty?");
    classname = s; 
  }

  std::string InstanceName() const { return instancename; }
  void InstanceName(const std::string& s) { instancename = s; }

  bool is_ok() const;

  ///
  /// prints in xml format for consumption by xsltproc.  You can't just use
  /// the icecube::serialization xml since this isn't well formed
  /// due to <px class_id="9" class_name="typeholder<bool>" ...
  ///
  std::string inspect() const;

  std::vector<std::string> keys() const;

  void merge(const I3ConfigurationImpl& rhs);

private:

  std::string classname;
  std::string instancename;

  friend class icecube::serialization::access;
  friend class I3Configuration;

  template <typename Archive> void serialize(Archive&, unsigned);

  // case-insensitive string compare
  struct case_insensitive_lt 
  {
    static int lowercase(int c) { return std::tolower(c); }
    bool operator()(const std::string& lhs, const std::string& rhs) const
    {
      std::string lhs_ = lhs;
      transform (lhs_.begin(),lhs_.end(), lhs_.begin(), lowercase);
      std::string rhs_ = rhs;
      transform (rhs_.begin(),rhs_.end(), rhs_.begin(), lowercase);
      return (lhs_ < rhs_);
    }
  };

public:
  /// these are the new parameters.  I3Parameter saves the full type information.
  typedef boost::multi_index_container<
    I3Parameter,
    boost::multi_index::indexed_by<
      boost::multi_index::ordered_unique<
	boost::multi_index::const_mem_fun<I3Parameter, const std::string&, &I3Parameter::name>,
	case_insensitive_lt
	>
      > 
    > parameters_t;

private:
  // the parameters
  boost::shared_ptr<parameters_t> parameters;

  friend std::ostream& operator<<(std::ostream&, const I3ConfigurationImpl&);

};

std::ostream& operator<<(std::ostream&, const I3ConfigurationImpl&);

I3_POINTER_TYPEDEFS(I3ConfigurationImpl);
I3_CLASS_VERSION(I3ConfigurationImpl, 2);

#endif
