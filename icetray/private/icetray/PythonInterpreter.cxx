/**
 *  $Id$
 *  
 *  Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
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

#include <boost/python.hpp>
#include <icetray/I3Logging.h>
#include <icetray/Singleton.h>
#include <icetray/PythonInterpreter.h>

// this is created by the BOOST_PYTHON_MODULE macro elsewhere
void init_module_icetray();

namespace I3
{
  //
  //  This is needed for icetray-inspect, etc.   You need to have an 
  //  interpreter initialized in order to convert module parameters, etc
  //  in a freestanding program.
  //
  //
  //  It is less-than-ideal that this module knows that it is normally
  //  found in namespace "icecube".  I'm not sure it matters very much
  //  what scope receives the symbols that init_module_icetray
  //  receives, as this will only be called if we are loaded by a
  //  commandline binary that has linked to us...  though if that binary
  //  in turn contains an embedded python interpreter...
  //
  InitializeInterpreter&
  InitializeInterpreter::init() 
  { 
    if (!Py_IsInitialized())
      {
	Py_Initialize();
	boost::python::detail::init_module("icecube.icetray", &init_module_icetray);
      }
    return *this;
  }

  namespace {
    InitializeInterpreter const& interp = I3::Singleton<InitializeInterpreter>::get_mutable_instance().init();
  }

}
