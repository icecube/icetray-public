/**
 *    $Id$
 *
 *    Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
 *    and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *    
 *    This file is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 3 of the License, or
 *    (at your option) any later version.
 *    
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *    
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ICETRAY_I3LOGGING_H_INCLUDED
#define ICETRAY_I3LOGGING_H_INCLUDED

#if defined(__cplusplus)

#  if defined(I3_PRINTF_LOGGING)

#    include <icetray/detail/PrintfLogging.h>

#  elif defined(__CINT__)

#    include <icetray/detail/CintLogging.h>

#  elif defined(I3_PYTHON_LOGGING)

#    include <icetray/detail/PythonLogging.h>

#  else 

#    include <icetray/detail/Log4cplusLogging.h>
#    define I3_LOG4CPLUS_LOGGING

#  endif

#endif

#include <icetray/detail/CLogging.h>

#endif //ndef ICETRAY_I3LOGGING_H_INCLUDED
