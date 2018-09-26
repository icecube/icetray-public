/**
 *  $Id$
 *  
 *  Copyright (C) 2012
 *  The IceCube Collaboration <http://www.icecube.wisc.edu>
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
#include <icetray/serialization.h>
#include <dataclasses/ModuleKey.h>
#include <sstream>

ModuleKey::~ModuleKey() { }

template <typename Archive>
void 
ModuleKey::serialize (Archive & ar, unsigned version)
{
  if (version>modulekey_version_)
    log_fatal("Attempting to read version %u from file but running version %u of ModuleKey class.",version,modulekey_version_);

  ar & make_nvp("I3FrameObject", base_object< I3FrameObject >(*this));
  ar & make_nvp("StringNumber",  stringNumber_);
  ar & make_nvp("OMNumber",  omNumber_);
}

I3_SERIALIZABLE(ModuleKey);
I3_SERIALIZABLE(I3MapModuleKeyString);

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

std::ostream& ModuleKey::Print(std::ostream& os) const
{
  os << "ModuleKey(" << GetString() << "," << GetOM() << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const ModuleKey& key)
{
  return(key.Print(os));
}

std::istream& operator>>(std::istream& is, ModuleKey& key)
{
  std::string s;
  is >> s;

  boost::regex reg("ModuleKey\\(([\\-\\d]+),(\\d+)\\)");
  boost::smatch matches;
  if (!boost::regex_search(s, matches, reg))
    log_fatal("Error parsing ModuleKey value from string \"%s\"", s.c_str());
  log_trace("matches: %s %s", matches.str(1).c_str(), matches.str(2).c_str());
  
  ModuleKey newkey;
  try
  {
    newkey.SetString(boost::lexical_cast<int>(matches.str(1)));
    newkey.SetOM(boost::lexical_cast<unsigned>(matches.str(2)));
    key = newkey;
  }catch(boost::bad_lexical_cast& blc)
  {
    is.setstate(is.rdstate()|std::ios::failbit);
  }
  return is;
}
