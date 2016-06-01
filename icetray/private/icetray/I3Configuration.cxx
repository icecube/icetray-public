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
#include <icetray/I3Logging.h>
#include <icetray/I3Configuration.h>
#include <icetray/I3ConfigurationImpl.h>

#include <icetray/serialization.h>
#include <serialization/scoped_ptr.hpp>

#include <icetray/Utility.h>

#include <boost/preprocessor.hpp>

#include <algorithm>
#include <cctype>

#include <boost/python/object.hpp>
#include <boost/foreach.hpp>

using namespace std;

I3Configuration::I3Configuration() :
  impl_(new I3ConfigurationImpl)
{ }

I3Configuration::I3Configuration(const I3Configuration &old) :
  impl_(new I3ConfigurationImpl(*old.impl_))
{ }

I3Configuration &
I3Configuration::operator = (const I3Configuration &old)
{ impl_.reset(new I3ConfigurationImpl(*old.impl_)); return *this; }

I3Configuration::~I3Configuration() 
{ }

bool 
I3Configuration::Has(const string &name) const
{
  return impl_->Has(name);
}

void
I3Configuration::Set(const string& name_, const boost::python::object& value)
{
  return impl_->Set(name_, value);
}

void 
I3Configuration::Add(const string& name_, 
		     const std::string& description, 
		     const boost::python::object& default_value)
{
  return impl_->Add(name_, description, default_value);
}

void 
I3Configuration::Add(const string& name_, 
		     const std::string& description)
{
  return impl_->Add(name_, description);
}

boost::python::object
I3Configuration::Get(const string& name_) const
{
  return impl_->Get(name_);
}

std::string
I3Configuration::GetDescription(const string& name_) const
{
  return impl_->GetParameter(name_).description();
}

template <typename Archive>
void 
I3Configuration::serialize(Archive& ar, unsigned version)
{
  ar & make_nvp("impl", impl_);
}

I3_BASIC_SERIALIZABLE(I3Configuration);

#include <iomanip>

std::string
I3Configuration::inspect() const
{
  return impl_->inspect();
}

ostream& operator<<(ostream& os, const I3Configuration& config)
{
  os << *config.impl_;
  return os;
}

bool
I3Configuration::is_ok() const
{
  return impl_->is_ok();
}

std::string 
I3Configuration::ClassName() const 
{  
  return impl_->ClassName();
}

void 
I3Configuration::ClassName(const std::string& s) 
{ 
  log_trace("ClassName: %s", s.c_str());
  i3_assert(!s.empty());
  impl_->ClassName(s); 
}

std::string I3Configuration::InstanceName() const
{
  return impl_->InstanceName();
}

void
I3Configuration::InstanceName(const std::string& s)
{
  impl_->InstanceName(s);
}

std::vector<std::string>
I3Configuration::keys() const
{
  return impl_->keys();
}
