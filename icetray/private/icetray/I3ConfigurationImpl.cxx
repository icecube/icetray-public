/**
 *  $Id: I3ConfigurationImpl.cxx 47404 2008-07-22 17:25:47Z troy $
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
#include <boost/python/object.hpp>
#include <boost/foreach.hpp>

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

  I3Parameter pb;
  pb.name(name_);
  pb.set_configured(value);
  // retrieval is case-insensitive
  pair<parameters_t::iterator, bool> pr = parameters->insert(pb);
  if (pr.second == false)
    log_fatal("Attempt to double-set parameter %s?", name_.c_str());

}

void 
I3ConfigurationImpl::Add(const string& name_, 
		     const std::string& description, 
		     const boost::python::object& default_value)
{
  log_trace("%s (%s)", __PRETTY_FUNCTION__, name_.c_str());

  // we don't care if it was inserted or not, we just want what's there
  I3Parameter pb;
  pb.name(name_);
  parameters_t::iterator iter = parameters->insert(pb).first;

  pb = *iter;
  pb.set_default(default_value);
  pb.description(description);
  parameters->replace(iter, pb);

  log_trace("%s default=%s", name_.c_str(), iter->default_value_str().c_str());
}

void 
I3ConfigurationImpl::Add(const string& name_, 
			 const std::string& description) 
{
  log_trace("%s (%s)", __PRETTY_FUNCTION__, name_.c_str());

  I3Parameter pb;
  pb.name(name_);
  parameters_t::iterator iter = parameters->insert(pb).first;

  pb = *iter;
  pb.description(description);
  parameters->replace(iter, pb);

  log_trace("%s default=%s", name_.c_str(), iter->default_value_str().c_str());
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

void
I3ConfigurationImpl::Connect(const std::string& boxname, const std::string& modulename)
{
  if (outboxes.find(boxname) != outboxes.end())
    log_fatal("trying to connect outbox %s of %s to %s, "
              "but it's already connected to %s!",
              boxname.c_str(),
              InstanceName().c_str(),
              modulename.c_str(),
              outboxes.find(boxname)->second.c_str()); 
  outboxes[boxname] = modulename;
}

template <typename Archive>
void
I3ConfigurationImpl::serialize(Archive &ar, unsigned version)
{
  if (version > 1)
    log_fatal("Attempt to read I3ConfigurationImpl version %u, this software knows only versions <= 1",
	      version);
  ar & make_nvp("parameters", parameters);
  ar & make_nvp("outboxes", outboxes);
  ar & make_nvp("classname", classname);
  ar & make_nvp("instancname", instancename);
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
     
  for (map<string,string>::const_iterator iter = config.outboxes.begin();
       iter != config.outboxes.end();
       iter++)
    {
      os << "  Outbox: " << iter->first << " ==> " << iter->second << "\n";
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
      if (iter->has_configured() && ! iter->has_default())
	{
	  log_error("Parameter '%s' was configured in the steering file" 
		    "but never Added by the module... misspelled?",
		    iter->name().c_str());
	  return false;
	}
      // Not fatal: a module might check either param B or C (but not
      // both) depending on the value of A
      if (!iter->got_by_module())
	{
	  log_warn("parameter %s was set but never Got by the module...?!?",
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
