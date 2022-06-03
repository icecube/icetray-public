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
option(USE_ROOT "Build with root" ON)
option(USE_CINT "Build dictionaries with rootcint" OFF)

set(SYSTEM_PACKAGES_ROOT FALSE)

if(NOT USE_ROOT AND USE_CINT)
  message(FATAL_ERROR "Cannot use rootcint without root")
endif(NOT USE_ROOT AND USE_CINT)

if(APPLE)
  unset(USE_CINT)
  unset(USE_CINT CACHE)
endif()

if (USE_ROOT)
  if(NOT ROOT_VERSION)
    # try for system root
    find_program(ROOT_CONFIG_EXECUTABLE root-config
                 HINTS $ENV{ROOTSYS}/bin)
    if(ROOT_CONFIG_EXECUTABLE)
      set(SYSTEM_PACKAGES_ROOT TRUE)
      execute_process(
        COMMAND ${ROOT_CONFIG_EXECUTABLE} --prefix 
        OUTPUT_VARIABLE ROOTSYS 
        OUTPUT_STRIP_TRAILING_WHITESPACE)
      execute_process(
        COMMAND ${ROOT_CONFIG_EXECUTABLE} --version 
        OUTPUT_VARIABLE ROOT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)
      string(REPLACE "/" "." ROOT_VERSION ${ROOT_VERSION})
      execute_process(
        COMMAND ${ROOT_CONFIG_EXECUTABLE} --incdir
        OUTPUT_VARIABLE ROOT_INCLUDE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE)
      execute_process(
        COMMAND ${ROOT_CONFIG_EXECUTABLE} --libs
        OUTPUT_VARIABLE ROOT_LIB_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE)
      execute_process(
        COMMAND ${ROOT_CONFIG_EXECUTABLE} --auxlibs
        OUTPUT_VARIABLE ROOT_AUXLIB_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE)
      string(STRIP ${ROOT_AUXLIB_OUTPUT} ROOT_AUXLIB_OUTPUT)
      separate_arguments(ROOT_AUXLIB_OUTPUT)
      execute_process(
        COMMAND ${ROOT_CONFIG_EXECUTABLE} --libdir
        OUTPUT_VARIABLE ROOT_LIB_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE)
      get_filename_component(ROOT_LIB_DIR ${ROOT_LIB_DIR} ABSOLUTE)
      execute_process(
        COMMAND ${ROOT_CONFIG_EXECUTABLE} --auxcflags
        OUTPUT_VARIABLE ROOT_CXX_FLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE)

      string(REGEX REPLACE "-[L]([^ ]*)" "" ROOT_LIBRARIES ${ROOT_LIB_OUTPUT})
      string(STRIP ${ROOT_LIBRARIES} ROOT_LIBRARIES)
      separate_arguments(ROOT_LIBRARIES)

      # ROOT_LIBRARIES might be cached which apparently can break cdr()
      set(ROOT_LIBRARIES_WORK ${ROOT_LIBRARIES})

      while(ROOT_LIBRARIES_WORK)
	car(foo ${ROOT_LIBRARIES_WORK})
	list(FIND ROOT_AUXLIB_OUTPUT ${foo} bar)
	if(${bar} EQUAL -1)
	  string(REGEX REPLACE "-l([^ ]+)" "\\1" foo ${foo})
	  list(APPEND ROOT_${ROOT_VERSION}_LIBS ${foo})
	endif(${bar} EQUAL -1)
	cdr(ROOT_LIBRARIES_WORK ${ROOT_LIBRARIES_WORK})
      endwhile(ROOT_LIBRARIES_WORK)
      set(ROOT_${ROOT_VERSION}_LIBS "${ROOT_${ROOT_VERSION}_LIBS}")

      # Get Minuit and Gui libs if they exist
      find_library(FIND_ROOT_MINUIT_LIB
      Minuit
      PATHS ${ROOT_LIB_DIR}
      NO_DEFAULT_PATH)

      if (FIND_ROOT_MINUIT_LIB)
        set(ROOT_MINUIT_LIB Minuit)
        list(APPEND ROOT_${ROOT_VERSION}_LIBS Minuit)
      endif()

      find_library(FIND_ROOT_GUI_LIB
      Gui
      PATHS ${ROOT_LIB_DIR}
      NO_DEFAULT_PATH)

      if (FIND_ROOT_GUI_LIB)
        set(ROOT_GUI_LIB Gui)
      endif()

      set(ROOTSYS ${ROOTSYS} CACHE STRING "Found ROOTSYS in ${ROOTSYS}")
      set(ROOT_LIB_DIR ${ROOT_LIB_DIR} CACHE STRING "Found ROOT_LIB_DIR in ${ROOT_LIB_DIR}")
      set(ROOT_INCLUDE_DIR ${ROOT_INCLUDE_DIR} CACHE STRING "Found ROOT include dir in ${ROOT_INCLUDE_DIR}")
    else()
      set(ROOT_LIB_DIR "" CACHE STRING "No ROOT found")
    endif()
  endif(NOT ROOT_VERSION)

  if(NOT ROOT_VERSION)
    # make sure to clear the old value from the cache first
    unset(USE_ROOT CACHE)
    set(USE_ROOT OFF CACHE BOOL "Root not detected")
  endif(NOT ROOT_VERSION)
endif (USE_ROOT)

if(NOT USE_ROOT OR NOT ROOT_VERSION)
  colormsg("")
  colormsg(HICYAN "root")
  message(STATUS "+ ROOT not found or disabled: building without ROOT support")
  #add_definitions(-UI3_USE_ROOT -UI3_USE_CINT)
  set(ROOT_INCLUDE_DIR "" CACHE STRING "USE_ROOT is OFF")
  set(ROOT_BIN_DIR "" CACHE STRING "USE_ROOT is OFF")
  # this is added to LD_LIBRARY_PATH
  set(ROOT_LIB_DIR "/USE_ROOT/IS/OFF" CACHE STRING "USE_ROOT is OFF")
  set(ROOT_LIBRARIES "" CACHE STRING "USE_ROOT is OFF")
  set(ROOTSYS "/USE_ROOT/IS/OFF" CACHE STRING "USE_ROOT is OFF")
  unset(ROOT_FOUND)
  unset(ROOT_FOUND CACHE)	
  unset(USE_CINT)
  unset(USE_CINT CACHE)
  set(USE_CINT OFF CACHE BOOL "USE_ROOT is OFF (Can't have CINT without ROOT)")
else()

  set(ROOT_5.18.00_LIBS Core Cint RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics Minuit)

  add_definitions(-DI3_USE_ROOT)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-strict-aliasing>)
  if (USE_CINT)
    add_definitions(-DI3_USE_CINT)
  endif (USE_CINT)

  if(NOT SYSTEM_PACKAGES_ROOT)
    set(old_TOOL_SYSTEM_PATH ${TOOL_SYSTEM_PATH})
    set(TOOL_SYSTEM_PATH NO_DEFAULT_PATH)
    tooldef (root 
      ${ROOTSYS}/include
      TObject.h
      ${ROOTSYS}/lib
      ${ROOTSYS}/bin
      ${ROOT_${ROOT_VERSION}_LIBS}
      )
    set(TOOL_SYSTEM_PATH ${old_TOOL_SYSTEM_PATH})
    set(ROOT_LIBRARIES ${ROOT_LIBRARIES} ${pthread_LIBRARIES})
  else()
    tooldef (root
      ${ROOT_INCLUDE_DIR}
      TObject.h
      ${ROOT_LIB_DIR}
      ${ROOTSYS}/bin
      ${ROOT_${ROOT_VERSION}_LIBS}
      )
    set(ROOT_LIBRARIES ${ROOT_LIBRARIES} ${pthread_LIBRARIES})
  endif(NOT SYSTEM_PACKAGES_ROOT)

  string(REGEX MATCH "std=c\\+\\+([0-9]+)" DUMMY ${ROOT_CXX_FLAGS})
  if (${CMAKE_MATCH_1} EQUAL ${CMAKE_CXX_STANDARD})
    message("-- + ROOT compiled with c++${CMAKE_MATCH_1}")
  else()
    set(ROOT_CXX_STANDARD ${CMAKE_MATCH_1})
    colormsg(RED "+ ROOT was compiled with c++${ROOT_CXX_STANDARD}, compiling projects that depend on ROOT with same standard")
  endif()

endif()
