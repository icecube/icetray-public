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
FILE(WRITE ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/testheader.cpp "#include <iostream>\n")
MESSAGE(STATUS "Checking if ${CMAKE_CXX_COMPILER} can do precompiled headers...")
EXECUTE_PROCESS(COMMAND 
  ${CMAKE_CXX_COMPILER} -x c++-header 
  -o testheader.hpp.gch 
  -c ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/testheader.cpp
  ERROR_VARIABLE USE_PRECOMPILED_HEADERS_GCC_ERROR)

# MESSAGE("Precompiled headers manually disabled.")
SET(USE_PRECOMPILED_HEADERS_GCC_ERROR "error")

IF("${USE_PRECOMPILED_HEADERS_GCC_ERROR}" STREQUAL "")
  SET(USE_PRECOMPILED_HEADERS TRUE CACHE BOOL "Use precompiled headers.  Currently does NOT work, but in general works on gcc >= 3.4 and speeds compilation.")
  MESSAGE(STATUS "Enabling precompiled headers.")
ELSE("${USE_PRECOMPILED_HEADERS_GCC_ERROR}" STREQUAL "")
  MESSAGE(STATUS "Disabling precompiled headers.")
ENDIF("${USE_PRECOMPILED_HEADERS_GCC_ERROR}" STREQUAL "")

