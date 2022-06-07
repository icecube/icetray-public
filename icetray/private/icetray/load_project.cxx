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

#include <dlfcn.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cstdlib>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace{

  /**
   * The project_name should *not* have the preceding 'lib' unless
   * that's acutally part of the project name.  For example, if the
   * project is actually library_reader, then the shared library
   * that's built (on linux) is liblibrary_reader.so.
   */
  std::string construct_libname(const std::string& project_name){
    const std::string prefix("lib");    
#ifdef __APPLE_CC__
    const std::string suffix(".dylib");
#else
    const std::string suffix(".so");
#endif
    return prefix + project_name + suffix;
  }
  
  /**
   * The filename is passed directly to dlopen, so can accept
   * libfoo.so and will search in LD_LIBRARY_PATH, or path
   * names (both absolute and relative) like /usr/local/lib/libfoo.so.
   * or ../lib/libfoobar.so.
   *
   * This functions throws std::runtime_error on error.
   */
  void load_dynamic_library(const std::string& filename){
    void* v = dlopen(filename.c_str(), RTLD_NOW | RTLD_GLOBAL);
    char* errmsg = dlerror();
    
    if(v == NULL || errmsg != NULL){
      std::string errormsg("dynamic loading error ");
      errormsg += "(" + filename + ")\n"; 
      errormsg += (errmsg == NULL) ? "dlopen: unknown error" : errmsg;
      errormsg += '\n';
      throw std::runtime_error(errormsg);
    }
  }
  
}

/**
 * This takes the project_name...and when I say project name, I mean
 * PROJECT NAME...it's the name of the project, not the library.
 * There's no '.so' suffix nor is there a 'lib' prefix, unless that's
 * actually part of the project's name.  For example, if the project
 * name is 'librarians_rock' then it's entirely appropriate to look
 * for and load the library called liblibrarians_rock.so (on linux).
 */
void
load_icecube_library(const std::string& project_name){
  load_dynamic_library(construct_libname(project_name));
}

/**
 * This should typically just thunk to 'load_icecube_library'
 * but has two functions:
 * 1) Maintain the interface, so no downstream code has to change.
 * 2) Provide functionality for any projects out there that might
 *    be constructing their own paths and using this function to
 *    dynamically load a (possibly) non-IceCube project.
 */
int 
load_project(std::string path, bool verbose){
  if (path.find("/") == std::string::npos){
    // The 'path' is not a path, but a project name, which is
    // actually standard-ish, with the exception of sometimes
    // leading 'lib's

    // Also to maintain the old pathology for this function, 
    // if there's a leading 'lib' assume it's *not* part
    // of the project name, so lop it off...it'll be added back
    // later.
    std::string project_name(path);
    const std::string lib("lib");
    if(project_name.find(lib) == 0){
      // 'lib' is at the beginning
      project_name = path.substr(lib.size());
    }
    load_icecube_library(project_name);
  }else{

    if(!fs::exists(fs::path(path))){
      std::string errormsg("Cannot find library ");
      errormsg += path;
      throw std::runtime_error(errormsg);
    }
    load_dynamic_library(path);    
  }
  return 0;
}

