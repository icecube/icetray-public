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
#include "util.h"

#include <iostream>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <dlfcn.h>

using namespace std;

namespace fs = boost::filesystem;

vector<string> 
all_projects(const string& dirname)
{
#ifdef __APPLE_CC__
  string extension = ".dylib";
#else
  string extension = ".so";
#endif

  vector<string> rv;

  fs::path datadir(dirname, boost::filesystem::native);
  fs::directory_iterator diter(datadir), end_iter;
  

  while(diter != end_iter)
    {
#if BOOST_VERSION >= 103600
      string thisone = diter->filename();
#else
      string thisone = diter->leaf();
#endif
      diter++;

      if (thisone.rfind(extension) != (thisone.length() - extension.length()))
	continue;
      if (thisone.find("lib") != 0)
	continue;
      string projname = thisone.substr(3, thisone.length() - extension.length() - 3);
      rv.push_back(projname);
    }

  return rv;
}



std::string 
project2libpath(const std::string& project)
{
#ifdef __APPLE_CC__
  string extension = ".dylib";
#else
  string extension = ".so";
#endif

  string libpath = "lib" + project + extension;

  return libpath;
}

