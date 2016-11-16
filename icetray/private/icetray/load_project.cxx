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
//
//  loads a project's shared library.
//
#include <dlfcn.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cstdlib>

int 
load_project (std::string path, bool verbose)
{
  if (path.find("lib") != 0)
    path = std::string("lib") + path;

#ifdef __APPLE_CC__
  path += ".dylib";
#else
  path += ".so";
#endif

  // first try via LD_LIBRARY_PATH search
  void *v = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL);
  char *errmsg = dlerror();
  
  // not found, then try $I3_BUILD/lib specifically
  if ((v == NULL || errmsg != NULL) && getenv("I3_BUILD") != NULL)
    {
      std::string fullpath(getenv("I3_BUILD"));
      fullpath += "/lib/";
      fullpath += path;
      v = dlopen(fullpath.c_str(), RTLD_NOW | RTLD_GLOBAL);
      errmsg = dlerror();
    }
  if (v != NULL && errmsg == NULL) 
    {
      return 0;
    }
  std::string errormsg("dlopen() dynamic loading error: ");
  errormsg += (errmsg == NULL) ? "unknown error" : errmsg;

  throw std::runtime_error(errormsg);
}

