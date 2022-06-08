#
#  $Id$
#
#  Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
#  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
#  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
#  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
#  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#  SUCH DAMAGE.
#  
#  SPDX-License-Identifier: BSD-2-Clause
#  
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
  message(STATUS "Using system packages.")
  find_program(BREW brew)
  if(BREW)
    execute_process(COMMAND brew --prefix COMMAND tr -d \\n RESULT_VARIABLE BREW_RESULT OUTPUT_VARIABLE BREW_PREFIX)
  endif()
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
  set(${TOOL}_LIB_ACCUM)

  if(NOT "${incdir}" STREQUAL "NONE")
    find_path(${TOOL}_INCLUDE_DIR NAMES ${incfile} PATHS ${BREW_PREFIX}/${incdir} ${incdir} ${TOOL_SYSTEM_PATH})
    if(${${TOOL}_INCLUDE_DIR} MATCHES ".*NOTFOUND$")
      found_not_ok("${incfile} not found in ${incdir}")
      set(${TOOL}_CONFIG_ERROR TRUE)
    else(${${TOOL}_INCLUDE_DIR} MATCHES ".*NOTFOUND$")
      STRING(REGEX REPLACE "//" "/" ${TOOL}_INCLUDE_DIR ${${TOOL}_INCLUDE_DIR})
      found_ok("${incfile} found at ${${TOOL}_INCLUDE_DIR}")
    endif(${${TOOL}_INCLUDE_DIR} MATCHES ".*NOTFOUND$")
  else(NOT "${incdir}" STREQUAL "NONE")
    set(${TOOL}_INCLUDE_DIR "/doesnt/exist" CACHE PATH "${TOOL} include dir")
  endif(NOT "${incdir}" STREQUAL "NONE")

  set(${TOOL}_BIN_DIR  "" CACHE PATH "${TOOL} binary directory")

  foreach(lib ${ARGN})
    set(foundlib${lib} "NOTFOUND" CACHE INTERNAL "tmp" FORCE)
    if (NOT ${lib})
      # if it is nothing, go find it
      find_library(foundlib${lib} ${lib} ${BREW_PREFIX}/${libdir} ${libdir} ${TOOL_SYSTEM_PATH})
    else ()
      #else go try to find it
      set(foundlib${lib} ${lib})
    endif()
    if(${foundlib${lib}} MATCHES ".*NOTFOUND$" AND NOT ${libdir} STREQUAL "NONE")
      found_not_ok("${lib}")
      set(${TOOL}_CONFIG_ERROR TRUE)
    else()
      if(NOT ${libdir} STREQUAL "NONE")
        STRING(REGEX REPLACE "//" "/" foundlib${lib} "${foundlib${lib}}")
        found_ok("${foundlib${lib}}")
        list(APPEND ${TOOL}_LIB_ACCUM ${foundlib${lib}})
        add_custom_command(TARGET install_tool_libs
          PRE_BUILD
          COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/lib/tools
          COMMAND ${CMAKE_SOURCE_DIR}/cmake/install_shlib.py ${foundlib${lib}} ${CMAKE_INSTALL_PREFIX}/lib/tools
          )
        if(TARGET "install_${TOOL}_libs")
          add_dependencies(install_tool_libs install_${TOOL}_libs)
        endif()
      endif()
    endif()
  endforeach()

  set(${TOOL}_LIBRARIES "${${TOOL}_LIB_ACCUM}" CACHE PATH "Libraries for tool ${TOOL}" FORCE)

  if (NOT ${TOOL}_CONFIG_ERROR)
    set(${TOOL}_FOUND TRUE CACHE BOOL "Tool '${TOOL}' found successfully" FORCE)
  else (NOT ${TOOL}_CONFIG_ERROR)
    set(${TOOL}_FOUND FALSE CACHE BOOL "Tool '${TOOL}' NOT found successfully...!" FORCE)
  endif (NOT ${TOOL}_CONFIG_ERROR)

endmacro(tooldef)
