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
#include <icetray/I3Logging.h>
#include <icetray/I3ConfigurationImpl.h>
#include <I3/name_of.h>
#include <icetray/serialization.h>

#include <icetray/I3Parameter.h>
#include <icetray/Utility.h>

#include <boost/preprocessor.hpp>

#include <algorithm>
#include <cctype>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/foreach.hpp>
#include <serialization/split_free.hpp>


using namespace std;

I3ConfigurationImpl::I3ConfigurationImpl() :
  parameters(new parameters_t)
{ 

}

I3ConfigurationImpl::~I3ConfigurationImpl() { }

bool 
I3ConfigurationImpl::Has(const string &name) const
{
  return parameters->find(name) != parameters->end();
}

void
I3ConfigurationImpl::Set(const string& name_, const boost::python::object& value)
{
  log_trace("%s (%s)", __PRETTY_FUNCTION__, name_.c_str());

  //if the user passed the python singleton `icetray.I3Default` then act like nothing happened
  boost::python::object I3Default = boost::python::import("icecube.icetray").attr("I3Default");
  if (value.ptr() == I3Default.ptr()){
    return;
  }
  
  parameters_t::iterator pr = parameters->find(name_);
  if (pr == parameters->end())
    log_fatal("Attempt to set parameter %s that doesn't exist", name_.c_str());
  I3Parameter pb(*pr);
  pb.set_configured(value);
  parameters->replace(pr, pb);
}

void 
I3ConfigurationImpl::Add(const string& name_, 
		     const std::string& description, 
		     const boost::python::object& default_value)
{
  log_trace("%s (%s)", __PRETTY_FUNCTION__, name_.c_str());

  I3Parameter pb;
  pb.name(name_);
  pb.set_default(default_value);
  pb.description(description);
  pair<parameters_t::iterator, bool> pr = parameters->insert(pb);
  if (pr.second == false)
    log_fatal("Attempt to double-add parameter %s?", name_.c_str());

  log_trace("%s default=%s", name_.c_str(), pb.default_value_str().c_str());
}

void 
I3ConfigurationImpl::Add(const string& name_, 
			 const std::string& description) 
{
  log_trace("%s (%s)", __PRETTY_FUNCTION__, name_.c_str());

  I3Parameter pb;
  pb.name(name_);
  pb.description(description);
  pair<parameters_t::iterator, bool> pr = parameters->insert(pb);
  if (pr.second == false)
    log_fatal("Attempt to double-add parameter %s?", name_.c_str());

  log_trace("%s default=%s", name_.c_str(), pb.default_value_str().c_str());
}

boost::python::object
I3ConfigurationImpl::Get(const string& name_) const
{
  log_trace("%s (%s)", __PRETTY_FUNCTION__, name_.c_str());

  parameters_t::const_iterator iter = parameters->find(name_);

  if (iter == parameters->end())
    log_fatal("Attempt to Get nonexistent parameter \"%s\"", name_.c_str());

  return iter->value();
}

I3Parameter
I3ConfigurationImpl::GetParameter(const string& name_) const
{
  parameters_t::const_iterator iter = parameters->find(name_);

  if (iter == parameters->end())
    log_fatal("Attempt to Get nonexistent parameter \"%s\"", name_.c_str());

  return *iter;
}

namespace icecube {
  namespace serialization {
    //
    //  Careful: multi_index has nonportable serialization (doesn't
    //  obey serialization's container_size_type).  Do it manually
    //  here.
    //

    template <class Archive>
    void save(Archive& ar, const I3ConfigurationImpl::parameters_t& params, unsigned)
    {
      uint16_t count = params.size();
      ar & icecube::serialization::make_nvp("nparams", count);
      for(I3ConfigurationImpl::parameters_t::const_iterator ci = params.begin();
          ci != params.end();
          ci++)
        ar & icecube::serialization::make_nvp("param", *ci);
    }

    template <class Archive>
    void load(Archive& ar, I3ConfigurationImpl::parameters_t& params, unsigned)
    {
      uint16_t count;
      ar & icecube::serialization::make_nvp("nparams", count);
      for (unsigned i = 0; i< count; i++)
      {
        I3Parameter p;
        ar & icecube::serialization::make_nvp("param", p);
        params.insert(p);
      }
    }
  }
}

I3_SERIALIZATION_SPLIT_FREE(I3ConfigurationImpl::parameters_t);

template <typename Archive>
void
I3ConfigurationImpl::serialize(Archive &ar, unsigned version)
{
  if (version > 3)
    log_fatal("Attempt to read I3ConfigurationImpl version %u, this software "
      "knows only versions <= 3", version);

  // unfortunately, version 1 was nonportable.  Wasn't ever officially
  // in the wild, though.
  if (version < 2)
    throw icecube::archive::archive_exception(icecube::archive::archive_exception::unsupported_version);

  // for parameters_t, use the portable save/load above
  ar & icecube::serialization::make_nvp("parameters", parameters);
  if (version == 2) {
    std::map<std::string, std::string> outboxes;
    ar & icecube::serialization::make_nvp("outboxes", outboxes);
  }
  ar & icecube::serialization::make_nvp("classname", classname);
  ar & icecube::serialization::make_nvp("instancename", instancename);
}

I3_BASIC_SERIALIZABLE(I3ConfigurationImpl);

#include <iomanip>

std::string
I3ConfigurationImpl::inspect() const
{
  std::string result;

  result += "<module>\n<type>" + encode_entities(classname) + "</type>\n";
  for(I3ConfigurationImpl::parameters_t::const_iterator ci = parameters->begin();
      ci != parameters->end();
      ci++)
    {
      result += "<parameter>\n\t<name>";
      result += encode_entities(ci->name()) + "</name>\n\t<description>" 
	+ encode_entities(ci->description()) + "</description>\n\t<default_value>"
	+ encode_entities(ci->default_value_str()) 
	+ "</default_value>\n</parameter>\n";
    }

  result += "</module>\n";
  return result;
}


ostream& operator<<(ostream& os, const I3ConfigurationImpl& config)
{
  if (config.instancename.length() > 0)
    os << config.instancename << " (" << config.classname << ")\n";
  else
    os << config.classname << "\n";

  if (config.parameters->size() == 0)
    os << "(No parameters)\n";

  for (I3ConfigurationImpl::parameters_t::const_iterator iter = config.parameters->begin();
       iter != config.parameters->end();
       iter++)
    {
      os << *iter << "\n";
    }
     
  return os;
}

  
bool
I3ConfigurationImpl::is_ok() const
{
  for (parameters_t::const_iterator iter = parameters->begin();
       iter != parameters->end();
       iter++)
    {
      // Not fatal: a module might check either param B or C (but not
      // both) depending on the value of A
      if (!iter->got_by_module())
	{
	  log_trace("parameter %s was set but never Got by the module...?!?",
		   iter->name().c_str());
	}
    }
  return true;
}


vector<string>
I3ConfigurationImpl::keys() const
{
  vector<string> thekeys;

  for (parameters_t::const_iterator iter = parameters->begin();
       iter != parameters->end();
       iter++)
    {
      thekeys.push_back(iter->name());
    }
  return thekeys;
}

void I3ConfigurationImpl::merge(const I3ConfigurationImpl& rhs)
{
  for(const auto& key : rhs.keys())
  {
    I3Parameter param = rhs.GetParameter(key);
    pair<parameters_t::iterator, bool> pr = parameters->insert(param);
    if (pr.second == false)
      log_fatal_stream("Attempt to double-add parameter " << key);

    log_trace_stream(key << " default=%s" << param.default_value_str());
  }
}
