/**
 *  $Id: load_project.h 165886 2018-10-01 14:37:58Z nwhitehorn $
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
#ifndef ICETRAY_LOAD_PROJECT_H_INCLUDED
#define ICETRAY_LOAD_PROJECT_H_INCLUDED

//
// "proj" here is "dataio" or "dataclasses", not "libdataio.so" or "libdataio" 
//
int load_project(std::string proj, bool verbose);

//
// we do this instead of default-argument for the sake of boost::python
//
int load_project(std::string proj) { return load_project(proj, true); }
#endif
