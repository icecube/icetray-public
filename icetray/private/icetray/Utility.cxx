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
#include <icetray/Utility.h>
#include <vector>
#include <boost/regex.hpp>
#include <glob.h>

using namespace std;
using namespace boost;

string
stlfilt(const string& what)
{
  const static regex stdregex("(?:std::)|(?:__1::)");
  string newstring = regex_replace(what, stdregex, "");

  const static regex vecreg("vector<(\\w+), allocator<(\\w+)> >");
  newstring = regex_replace(newstring, vecreg, "vector<$1>");

  const static regex mapreg("map<(\\w+), (\\w+), less<(\\w+)>, allocator<pair<const (\\w+), (\\w+)> > >");
  newstring = regex_replace(newstring, mapreg, "map<$1, $2>");

  const static regex stringreg("basic_string<char, char_traits<char>, allocator<char> >");
  newstring = regex_replace(newstring, stringreg, "string");

  return newstring;
}

string 
prettify_xml(const string& what)
{
  const static regex foreg("^\\s+<I3FrameObject [\\w=\" ]+></I3FrameObject>\\n");
  string newstring = regex_replace(what, foreg, "");

  const static regex trackingreg(" tracking_level=\"\\d+\"");
  newstring = regex_replace(newstring, trackingreg, "");

  const static regex objectidreg(" object_id=\"[_\\d]+\"");
  newstring = regex_replace(newstring, objectidreg, "");

  const static regex classidreg(" class_id=\"\\d+\"");
  newstring = regex_replace(newstring, classidreg, "");

  newstring = stlfilt(newstring);
  return newstring;
}

int
glob(const string& expression, vector<string>& results)
{
  glob_t globbuf;
  int rv = ::glob(expression.c_str(), 0, NULL, &globbuf);

  if (rv != 0)
    return rv;

  for (unsigned i=0; i<globbuf.gl_pathc; i++)
    results.push_back(globbuf.gl_pathv[i]);

  globfree(&globbuf);

  return 0;
}

//
// do conversions like & => &amp; so that an xml parser won't get
// confused by messages and test suite spew
//
string encode_entities(const string &encode_me)
{
  string xmlized;
  for (unsigned i=0; i<encode_me.length(); i++)
    {
      switch (encode_me[i])
	{
	case '<':
	  xmlized += "&lt;";
	  break;
	case '>':
	  xmlized += "&gt;";
	  break;
	case '&':
	  xmlized += "&amp;";
	  break;
	case '\"':
	  xmlized += "&quot;";
	  break;
	case '\'':
	  xmlized += "&apos;";
	  break;
	default:
	  xmlized.push_back(encode_me[i]);
	}
    }
  return xmlized;
}
