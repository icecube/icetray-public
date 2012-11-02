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
#include <icetray/init.h>

namespace I3
{
  //
  //  This is needed for icetray to work correctly from a freestanding program. 
  //  You need to have an interpreter initialized in order to convert module parameters, etc
  //
  //  This replaces an older approach that initialized the library as part
  //  of static initialization.  That caused python to always be initialized
  //  before main() began.
  //
  //  This init function will not be called when icetray is used from a
  //  python script.  Do not add features to this function that must run
  //  in such a situation.
  void init_icetray_lib()
  { 
    if (!Py_IsInitialized())
      {
        Py_Initialize();
      }
    boost::python::import("icecube.icetray");
  }

}
