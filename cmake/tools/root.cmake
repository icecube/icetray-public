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
option(USE_ROOT "Build with root" ON)
option(USE_CINT "Build dictionaries with rootcint" OFF)

set(SYSTEM_PACKAGES_ROOT FALSE)

if(NOT USE_ROOT AND USE_CINT)
  message(FATAL_ERROR "Cannot use rootcint without root")
endif(NOT USE_ROOT AND USE_CINT)

if (USE_ROOT)
  if(NOT ROOT_VERSION)
    foreach(ROOTVER 5.34.19 5.34.18 5.34.04 5.34.03 5.32.00 5.30.06 5.30.05 5.30.00 5.28.00h 5.28.00d 5.28.00 5.27.06b 5.26.00e 5.24.00b 5.24.00 5.20.00 5.18.00)
      if(IS_DIRECTORY ${I3_PORTS}/root-v${ROOTVER})
        set(ROOT_VERSION "${ROOTVER}")
        set(ROOT_${ROOT_VERSION}_LIBS Core Cint RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread Minuit Gui)
        break()
      endif()
    endforeach()
  endif(NOT ROOT_VERSION)

  if(NOT ROOT_VERSION)
    # try for system root
    find_program(ROOT_CONFIG_EXECUTABLE root-config
                 PATHS $ENV{ROOTSYS}/bin)
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
	  list(APPEND pewp ${foo})
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
        list(APPEND ROOT_${ROOT_VERSION}_LIBS Gui)
      endif()

      set(ROOTSYS ${ROOTSYS} CACHE STRING "Found ROOTSYS in ${ROOTSYS}")
      set(ROOT_LIB_DIR ${ROOT_LIB_DIR} CACHE STRING "Found ROOT_LIB_DIR in ${ROOT_LIB_DIR}")
      set(ROOT_INCLUDE_DIR ${ROOT_INCLUDE_DIR} CACHE STRING "Found ROOT include dir in ${ROOT_INCLUDE_DIR}")
    else()
      set(ROOT_LIB_DIR "" CACHE STRING "No ROOT found")
    endif()
  endif(NOT ROOT_VERSION)

  if(NOT ROOT_VERSION)
    set(USE_ROOT OFF CACHE BOOL "Root not detected")
  endif(NOT ROOT_VERSION)
endif (USE_ROOT)

if(NOT USE_ROOT)
  colormsg("")
  colormsg(HICYAN "root")
  message(STATUS "+ ROOT not found or disabled: building without ROOT support")
  add_definitions(-UI3_USE_ROOT -UI3_USE_CINT)
  set(ROOT_INCLUDE_DIR "" CACHE STRING "USE_ROOT is OFF")
  set(ROOT_BIN_DIR "" CACHE STRING "USE_ROOT is OFF")
  # this is added to LD_LIBRARY_PATH
  set(ROOT_LIB_DIR "/USE_ROOT/IS/OFF" CACHE STRING "USE_ROOT is OFF")
  set(ROOT_LIBRARIES "" CACHE STRING "USE_ROOT is OFF")
  set(ROOTSYS "/USE_ROOT/IS/OFF" CACHE STRING "USE_ROOT is OFF")
else(NOT USE_ROOT)

  set(ROOT_5.18.00_LIBS Core Cint RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics Minuit)

  add_definitions(-DI3_USE_ROOT -fno-strict-aliasing)
  if (USE_CINT)
    add_definitions(-DI3_USE_CINT)
  endif (USE_CINT)
  if(NOT ROOTSYS)
    set(ROOTSYS ${I3_PORTS}/root-v${ROOT_VERSION} CACHE STRING "The beloved ROOTSYS.")
  endif(NOT ROOTSYS)

  if (EXISTS ${ROOTSYS}/include/TXNetFile.h)
    find_library(FIND_ROOT_NETX_LIBRARY 
      Netx 
      PATHS ${ROOTSYS}/lib
      NO_DEFAULT_PATH)
  endif()

  if (FIND_ROOT_NETX_LIBRARY)
    set(ROOT_NETX_LIBRARY Netx)
    add_definitions(-DROOT_HAS_NETX)
  endif()

  if(NOT SYSTEM_PACKAGES_ROOT)
    tooldef (root 
      ${ROOTSYS}/include
      TObject.h
      ${ROOTSYS}/lib
      ${ROOTSYS}/bin
      ${ROOT_NETX_LIBRARY} ${ROOT_${ROOT_VERSION}_LIBS}
      )
    set(ROOT_LIBRARIES ${ROOT_LIBRARIES} ${pthread_LIBRARIES})
  else()
    tooldef (root
      ${ROOT_INCLUDE_DIR}
      TObject.h
      ${ROOT_LIB_DIR}
      ${ROOTSYS}/bin
      ${ROOT_NETX_LIBRARY} ${ROOT_${ROOT_VERSION}_LIBS}
      )
    set(ROOT_LIBRARIES ${ROOT_LIBRARIES} ${pthread_LIBRARIES})
  endif(NOT SYSTEM_PACKAGES_ROOT)
endif(NOT USE_ROOT)
