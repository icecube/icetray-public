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
#ifndef ICETRAY_UTILITY_H_INCLUDED
#define ICETRAY_UTILITY_H_INCLUDED

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <sstream>

///
/// split a string into a set of strings
///
std::set<std::string> 
split(const std::string& split_me, const std::string& on_this_expression);

//
// works like stlfilt.pl, changes 
//   std::vector<T, std::allocator<T> > 
// to
//   vector<T>
//
std::string stlfilt(const std::string& s);
std::string prettify_xml(const std::string& s);

std::vector<std::string> to_vector(std::string s0,
				   std::string s1="",
				   std::string s2="",
				   std::string s3="",
				   std::string s4="",
				   std::string s5="",
				   std::string s6="",
				   std::string s7="",
				   std::string s8="",
				   std::string s9="",
				   std::string s10="",
				   std::string s11="",
				   std::string s12="",
				   std::string s13="",
				   std::string s14="",
				   std::string s15="",
				   std::string s16="",
				   std::string s17="",
				   std::string s18="",
				   std::string s19="");

int glob(const std::string& expression, std::vector<std::string>& results);

std::string encode_entities(const std::string& encode_me);

#endif
