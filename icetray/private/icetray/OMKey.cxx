/**
 *  $Id$
 *  
 *  Copyright (C) 2007
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
#include <icetray/OMKey.h>
#include <sstream>
#include <icetray/I3FrameObject.h>

OMKey::~OMKey() { }

template <class Archive>
void OMKey::save(Archive& ar, unsigned version) const
{
  ar & make_nvp("StringNumber",  stringNumber_);
  ar & make_nvp("OMNumber",  omNumber_);
  ar & make_nvp("PMTNumber",  pmtNumber_);
}

template <class Archive>
void OMKey::load(Archive& ar, unsigned version)
{
  if (version>omkey_version_)
    log_fatal("Attempting to read version %u from file but running version %u of OMKey class.",
              version,omkey_version_);

  if (version<=1) {
    I3FrameObject fo;
    ar & make_nvp("I3FrameObject", fo);
  }
  ar & make_nvp("StringNumber",  stringNumber_);
  ar & make_nvp("OMNumber",  omNumber_);
    
  if (version>=1) {
    ar & make_nvp("PMTNumber",  pmtNumber_);
  } else {
    pmtNumber_=0;    
  }
}

I3_SPLIT_SERIALIZABLE(OMKey);

#include <sstream>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;

ostream& operator<<(ostream& os, const OMKey& key)
{
  os << "OMKey(" << key.GetString() << "," << key.GetOM() << "," << static_cast<unsigned int>(key.GetPMT()) << ")";
  return os;
}

istream& operator>>(istream& is, OMKey& key)
{
  std::string s;
  is >> s;

  boost::regex reg("OMKey\\(([\\-\\d]+),(\\d+),(\\d+)\\)");
  boost::smatch matches;
  if (boost::regex_search(s, matches, reg)) {
    // try the 3 entry version first
    OMKey newkey;
    log_trace("matches: %s %s %s", matches.str(1).c_str(), matches.str(2).c_str(), matches.str(3).c_str());
    newkey.SetString(boost::lexical_cast<int>(matches.str(1)));
    newkey.SetOM(boost::lexical_cast<unsigned>(matches.str(2)));
    newkey.SetPMT(static_cast<unsigned char>(boost::lexical_cast<unsigned int>(matches.str(3))));
    key = newkey;
    return is;
  } else {
    boost::regex reg("OMKey\\(([\\-\\d]+),(\\d+)\\)");
    boost::smatch matches;
    if (!boost::regex_search(s, matches, reg))
      log_fatal("Error parsing OMKey value from string \"%s\"", s.c_str());

    OMKey newkey;
    log_trace("matches: %s %s [old-style OMKey]", matches.str(1).c_str(), matches.str(2).c_str());
       
    newkey.SetString(boost::lexical_cast<int>(matches.str(1)));
    newkey.SetOM(boost::lexical_cast<unsigned>(matches.str(2)));
    newkey.SetPMT(0);
    key = newkey;
    return is;
  }
}

string OMKey::str()const{
  stringstream s;
  s<<*this;
  return s.str();
}



