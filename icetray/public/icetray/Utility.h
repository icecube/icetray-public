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

//
// works like stlfilt.pl, changes 
//   std::vector<T, std::allocator<T> > 
// to
//   vector<T>
//
std::string stlfilt(const std::string& s);
std::string prettify_xml(const std::string& s);

int glob(const std::string& expression, std::vector<std::string>& results);

std::string encode_entities(const std::string& encode_me);

#endif
