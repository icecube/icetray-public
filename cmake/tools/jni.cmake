#
#  $Id$
#  
#  Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#  
#  This file is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>
#  

# we don't use the tooldef() macro, so we have to fudge pretty-printing
colormsg("")
colormsg(HICYAN "jni")

if(NOT IS_DIRECTORY $ENV{JAVA_HOME})
  colormsg(YELLOW "*** JAVA_HOME ($ENV{JAVA_HOME}) is either unset or does not point to")
  colormsg(YELLOW "*** a valid directory. This could be a problem if some project in your")
  colormsg(YELLOW "*** workspace requires java.")
else()
  found_ok("JAVA_HOME=$ENV{JAVA_HOME}")
endif()

find_package(JNI)

if(JNI_FOUND)
  set(JNI_FOUND TRUE CACHE BOOL "Jni found flag" FORCE)
endif(JNI_FOUND)
