/**
 *  $Id$
 *  
 *  Copyright (C) 2012
 *  The IceCube Collaboration <http://www.icecube.wisc.edu>
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
#include <icetray/serialization.h>
#include <dataclasses/ModuleKey.h>
#include <sstream>

template <typename Archive>
void 
ModuleKey::serialize (Archive & ar, unsigned version)
{
  if (version>modulekey_version_) {
    log_fatal("Attempting to read version %u from file but running version %u of ModuleKey class.",version,modulekey_version_);
  }

  if (version == 0) {
    I3FrameObject dummy;
    ar & make_nvp("I3FrameObject", dummy);
  }
  ar & make_nvp("StringNumber",  stringNumber_);
  ar & make_nvp("OMNumber",  omNumber_);
}

I3_SERIALIZABLE(ModuleKey);
I3_SERIALIZABLE(I3MapModuleKeyString);

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

std::ostream& operator<<(std::ostream& os, const ModuleKey& key)
{
  os << "ModuleKey(" << key.GetString() << "," << key.GetOM() << ")";
  return os;
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
