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

/**
   splits one big string up into a set of strings.  

   @param what what to split

   @param exp regular expression specify what to split on.  This gets
   eaten and ignored
*/
set<string> 
split(const string& what, const string& exp)
{
  regex reg(exp);

  sregex_token_iterator it(what.begin(), what.end(), reg, -1);
  sregex_token_iterator end;
  
  set<string> stringset;  // "stringset".  You know, like on the playground.

  while(it != end)
    stringset.insert(*it++);
  
  return stringset;
}

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

  // work, but don't look good.
#if 0
  const static regex closetag_alonereg("^\\s+</\\w+>\n");
  newstring = regex_replace(newstring, closetag_alonereg, "");

  const static regex closetag("</\\w+>");
  newstring = regex_replace(newstring, closetag, "");
#endif

  newstring = stlfilt(newstring);
  return newstring;
}

vector<string> 
to_vector(string s0,
	  string s1,
	  string s2,
	  string s3,
	  string s4,
	  string s5,
	  string s6,
	  string s7,
	  string s8,
	  string s9,
	  string s10,
	  string s11,
	  string s12,
	  string s13,
	  string s14,
	  string s15,
	  string s16,
	  string s17,
	  string s18,
	  string s19)
{
  vector<string> the_vector;
  if (s0.length()) the_vector.push_back(s0);
  if (s1.length()) the_vector.push_back(s1);
  if (s2.length()) the_vector.push_back(s2);
  if (s3.length()) the_vector.push_back(s3);
  if (s4.length()) the_vector.push_back(s4);
  if (s5.length()) the_vector.push_back(s5);
  if (s6.length()) the_vector.push_back(s6);
  if (s7.length()) the_vector.push_back(s7);
  if (s8.length()) the_vector.push_back(s8);
  if (s9.length()) the_vector.push_back(s9);
  if (s10.length()) the_vector.push_back(s10);
  if (s11.length()) the_vector.push_back(s11);
  if (s12.length()) the_vector.push_back(s12);
  if (s13.length()) the_vector.push_back(s13);
  if (s14.length()) the_vector.push_back(s14);
  if (s15.length()) the_vector.push_back(s15);
  if (s16.length()) the_vector.push_back(s16);
  if (s17.length()) the_vector.push_back(s17);
  if (s18.length()) the_vector.push_back(s18);
  if (s19.length()) the_vector.push_back(s19);
  return the_vector;
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
