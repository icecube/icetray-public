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
macro(report_find tool what where)
  if(${where} MATCHES ".*NOTFOUND$")
    message(STATUS "- ${what}")
    set(${tool}_CONFIG_ERROR TRUE)
  else(${where} MATCHES ".*NOTFOUND$")
    message(STATUS "+ ${what} found at ${where}")
  endif(${where} MATCHES ".*NOTFOUND$")
endmacro(report_find tool what where)

macro(found_not_ok msg)
  message(STATUS "- ${msg}")
endmacro(found_not_ok msg)

macro(found_ok msg)
  message(STATUS "+ ${msg}")
endmacro(found_ok msg)

if (SYSTEM_PACKAGES)
   message(STATUS "Using system packages when I3_PORTS not available (Foot Shooting mode ENABLED)")
else (SYSTEM_PACKAGES)
   set(TOOL_SYSTEM_PATH NO_DEFAULT_PATH)
endif (SYSTEM_PACKAGES)

#
# Sets various env variables for tool
#
macro(tooldef tool_ incdir incfile libdir bindir)
  string(TOUPPER ${tool_} TOOL)
  colormsg("")
  colormsg(HICYAN "${tool_}")

  # Set the config error to false by default		
  set(${TOOL}_CONFIG_ERROR FALSE)

  if(NOT "${incdir}" STREQUAL "NONE")
    find_path(${TOOL}_INCLUDE_DIR NAMES ${incfile} PATHS ${I3_PORTS}/${incdir} ${incdir} ${TOOL_SYSTEM_PATH})
    if(${${TOOL}_INCLUDE_DIR} MATCHES ".*NOTFOUND$")
      found_not_ok("${incfile} not found in ${incdir}")
      set(${TOOL}_CONFIG_ERROR TRUE)
    else(${${TOOL}_INCLUDE_DIR} MATCHES ".*NOTFOUND$")
      found_ok("${incfile} found at ${${TOOL}_INCLUDE_DIR}")
    endif(${${TOOL}_INCLUDE_DIR} MATCHES ".*NOTFOUND$")
  else(NOT "${incdir}" STREQUAL "NONE")
    set(${TOOL}_INCLUDE_DIR "" CACHE PATH "${TOOL} include dir")
  endif(NOT "${incdir}" STREQUAL "NONE")

  if(NOT "${bindir}" STREQUAL "NONE")
    set(${TOOL}_BIN_DIR  ${I3_PORTS}/${bindir} CACHE PATH "${TOOL} binary directory")
  else(NOT "${bindir}" STREQUAL "NONE")
    set(${TOOL}_BIN_DIR  "" CACHE PATH "${TOOL} binary directory")
  endif(NOT "${bindir}" STREQUAL "NONE")

  foreach(lib ${ARGN})
    set(foundlib${lib} "NOTFOUND" CACHE INTERNAL "tmp" FORCE)
    if (NOT ${lib})
      # if it is nothing, go find it
      find_library(foundlib${lib} 
	${lib} 
	${I3_PORTS}/${libdir} ${libdir}
	${TOOL_SYSTEM_PATH})
    else (NOT ${lib})
      #else go try to find it
      set(foundlib${lib} ${lib})
    endif(NOT ${lib})
    if(${foundlib${lib}} MATCHES ".*NOTFOUND$" AND NOT ${libdir} STREQUAL "NONE")
      found_not_ok("${lib}")
      set(${TOOL}_CONFIG_ERROR TRUE)
    else(${foundlib${lib}} MATCHES ".*NOTFOUND$" AND NOT ${libdir} STREQUAL "NONE")
      if(NOT ${libdir} STREQUAL "NONE")
	found_ok("${foundlib${lib}}")
	list(APPEND ${TOOL}_LIB_ACCUM ${foundlib${lib}})
	add_custom_command(TARGET install_tool_libs
	  PRE_BUILD
	  COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/lib/tools
	  COMMAND ${CMAKE_SOURCE_DIR}/cmake/install_shlib.py ${foundlib${lib}} ${CMAKE_INSTALL_PREFIX}/lib/tools
	  ) 
	add_dependencies(install_tool_libs install_${TOOL}_libs)
      endif(NOT ${libdir} STREQUAL "NONE")
    endif(${foundlib${lib}} MATCHES ".*NOTFOUND$" AND NOT ${libdir} STREQUAL "NONE")
  endforeach(lib ${ARGN})

  set(${TOOL}_LIBRARIES "${${TOOL}_LIB_ACCUM}" CACHE PATH "Libraries for tool ${TOOL}" FORCE)

  if (NOT ${TOOL}_CONFIG_ERROR)
    set(${TOOL}_FOUND TRUE CACHE BOOL "Tool '${TOOL}' found successfully" FORCE)
  else (NOT ${TOOL}_CONFIG_ERROR)
    set(${TOOL}_FOUND FALSE CACHE BOOL "Tool '${TOOL}' found successfully" FORCE)
  endif (NOT ${TOOL}_CONFIG_ERROR)

endmacro(tooldef)

