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
colormsg(HICYAN "python")

set(PYTHON_FOUND TRUE CACHE BOOL "Python found successfully")

find_program(PYTHON_EXECUTABLE python
      PATHS ENV PATH         # look in the PATH environment variable
      NO_DEFAULT_PATH        # do not look anywhere else...
      )
find_package(PythonInterp QUIET)

# 
# determine version of the system python.
#
execute_process(COMMAND ${PYTHON_EXECUTABLE} -V
  OUTPUT_VARIABLE STDOUT_VERSION
  ERROR_VARIABLE PYTHON_VERSION
  ERROR_STRIP_TRAILING_WHITESPACE)

if(STDOUT_VERSION MATCHES "Python")
  set(PYTHON_VERSION "${STDOUT_VERSION}")
endif()

#
# Provide version in numeric form for comparison
#
string(REGEX MATCH "([0-9]+)\\.([0-9]+)\\.?([0-9]*)"
  PYTHON_STRIPPED_VERSION
  ${PYTHON_VERSION})
string(REGEX MATCH "([0-9]+)\\.([0-9]+)"
  PYTHON_STRIPPED_MAJOR_MINOR_VERSION
  ${PYTHON_VERSION})
numeric_version(${PYTHON_STRIPPED_VERSION} PYTHON)
message(STATUS "+  version: ${PYTHON_VERSION}") 

STRING(REPLACE "." "" PYTHON_VERSION_NO_DOTS ${PYTHON_STRIPPED_MAJOR_MINOR_VERSION})

#
# Get the (probable) root dir of the python install
#
get_filename_component(PYTHON_BIN_DIR ${PYTHON_EXECUTABLE} PATH)
get_filename_component(PYTHON_ROOT ${PYTHON_BIN_DIR} PATH)
#message(STATUS "+  base dir: ${PYTHON_ROOT}")

#
# Find the library and header file manually,
# using the python root that we found above.
#
FIND_LIBRARY(PYTHON_LIBRARY
  NAMES python${PYTHON_VERSION_NO_DOTS} python${PYTHON_STRIPPED_MAJOR_MINOR_VERSION}
  HINTS ${PYTHON_ROOT} ${PYTHON_ROOT}/lib
  NO_SYSTEM_ENVIRONMENT_PATH
)
FIND_PATH(PYTHON_INCLUDE_DIR
  NAMES Python.h
  PATHS
    ${PYTHON_ROOT}/include
  PATH_SUFFIXES
    python${PYTHON_STRIPPED_MAJOR_MINOR_VERSION}
)
# required for ubuntu, because their version of FindPythonLibs is different
set(PYTHON_INCLUDE_DIR2 ${PYTHON_INCLUDE_DIR})

#
# Now do the full python detection, which includes special
# things for frameworks detection.
#
find_package(PythonLibs ${PYTHON_STRIPPED_VERSION} EXACT QUIET)
# Store in plural form for consistency with other tools
set(PYTHON_LIBRARIES "${PYTHON_LIBRARIES}" CACHE FILEPATH "")

if(NOT PYTHON_EXECUTABLE)
  set(PYTHON_FOUND FALSE CACHE BOOL "Python found successfully")
endif(NOT PYTHON_EXECUTABLE)

if(NOT PYTHON_INCLUDE_DIR)
  set(PYTHON_INCLUDE_DIR ${PYTHON_INCLUDE_PATH})
endif(NOT PYTHON_INCLUDE_DIR)

if(NOT EXISTS "${PYTHON_INCLUDE_DIR}/Python.h")
  message(STATUS "Error configuring python:  ${PYTHON_INCLUDE_DIR}/Python.h does not exist.\n")
  set(PYTHON_FOUND FALSE CACHE BOOL "Python found successfully")
  set(PYTHON_CONFIG_ERROR TRUE)
endif(NOT  EXISTS "${PYTHON_INCLUDE_DIR}/Python.h")

message(STATUS "+   binary: ${PYTHON_EXECUTABLE}")	
message(STATUS "+ includes: ${PYTHON_INCLUDE_DIR}")	
message(STATUS "+     libs: ${PYTHON_LIBRARIES}")	

if(PYTHON_NUMERIC_VERSION LESS 20600)
  message(FATAL_ERROR "A Python version >= 2.6 is required.")
endif(PYTHON_NUMERIC_VERSION LESS 20600)

# look for numpy
execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import numpy"
    RESULT_VARIABLE NUMPY_FOUND)

# let's make our xxx_FOUND variable like CMake ones
if(NUMPY_FOUND EQUAL 0)
  set(NUMPY_FOUND TRUE)
else(NUMPY_FOUND EQUAL 0)
  set(NUMPY_FOUND FALSE)
endif(NUMPY_FOUND EQUAL 0)

if(NOT NUMPY_FOUND)
	set(NUMPY_FOUND FALSE CACHE BOOL "Numpy found successfully")
else(NOT NUMPY_FOUND)
	set(NUMPY_FOUND TRUE CACHE BOOL "Numpy found successfully")
	execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
	    "import numpy; print(numpy.get_include())"
           OUTPUT_VARIABLE _NUMPY_INCLUDE_DIR
	    OUTPUT_STRIP_TRAILING_WHITESPACE)
    
    # look in some other places, too. This should make it 
    # work on OS X, where the headers are in SDKs within XCode.app,
    # but python reports them as being available at /.
    set(NUMPY_INCLUDE_DIR_CANDIDATES ${_NUMPY_INCLUDE_DIR})
    foreach(prefix ${CMAKE_PREFIX_PATH})
        list(APPEND NUMPY_INCLUDE_DIR_CANDIDATES ${prefix}/${_NUMPY_INCLUDE_DIR})
        list(APPEND NUMPY_INCLUDE_DIR_CANDIDATES ${prefix}/../${_NUMPY_INCLUDE_DIR})
    endforeach(prefix ${CMAKE_PREFIX_PATH})
    foreach(prefix ${CMAKE_FRAMEWORK_PATH})
        list(APPEND NUMPY_INCLUDE_DIR_CANDIDATES ${prefix}/${_NUMPY_INCLUDE_DIR})
        list(APPEND NUMPY_INCLUDE_DIR_CANDIDATES ${prefix}/../../../${_NUMPY_INCLUDE_DIR})
    endforeach(prefix ${CMAKE_FRAMEWORK_PATH})

	find_path(NUMPY_INCLUDE_DIR NAMES numpy/ndarrayobject.h PATHS ${NUMPY_INCLUDE_DIR_CANDIDATES})
	set(NUMPY_INCLUDE_DIR ${NUMPY_INCLUDE_DIR} CACHE STRING "Numpy inc directory")
	message(STATUS "+    numpy: ${NUMPY_INCLUDE_DIR}")
endif(NOT NUMPY_FOUND)

## look for scipy
execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import scipy"
    RESULT_VARIABLE SCIPY_FOUND)

# let's make our xxx_FOUND variable like CMake ones
if(SCIPY_FOUND EQUAL 0)
  set(SCIPY_FOUND TRUE)
  message(STATUS "+    scipy: FOUND")
else()
  set(SCIPY_FOUND FALSE)
endif()
set(SCIPY_FOUND ${SCIPY_FOUND} CACHE BOOL "Scipy found successfully")

