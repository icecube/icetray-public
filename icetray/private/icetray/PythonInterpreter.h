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

#ifndef ICETRAY_PYTHONINTERPRETER_H_INCLUDED
#define ICETRAY_PYTHONINTERPRETER_H_INCLUDED

//
//  This is needed for icetray-inspect, etc.   You need to have an 
//  interpreter initialized in order to convert module parameters, etc
//  in a freestanding program.
//
//
//  It is less-than-ideal that this module knows that it is normally
//  found in namespace "icecube".  I'm not sure it matters very much
//  what scope receives the symbols that init_module_icetray
//  receive, as this will only be called if we are loaded by a
//  commandline binary that has linked to us...  though if that binary
//  in turn contains an embedded python interpreter...
//
namespace I3
{
  struct InitializeInterpreter
  {
    // There are several different logging configurations
    // and each needs to be sure that the python interpreter exists
    // before they initialize themselves
    InitializeInterpreter& init();
  };
}

#endif
