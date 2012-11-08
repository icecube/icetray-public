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
#include <icetray/Utility.h>
#include <vector>
#include <boost/regex.hpp>
#include <glob.h>

using namespace std;
using namespace boost;

string
stlfilt(const string& what)
{
  const static regex stdregex("std::");
  string newstring = regex_replace(what, stdregex, "");

  const static regex vecreg("vector<(\\w+), allocator<(\\w+)> >");
  newstring = regex_replace(newstring, vecreg, "vector<$1>");

  const static regex mapreg("map<(\\w+), (\\w+), less<(\\w+)>, allocator<pair<const (\\w+), (\\w+)> > >");
  newstring = regex_replace(newstring, mapreg, "map<$1, $2>");

  const static regex stringreg("string");
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
