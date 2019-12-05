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

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12)
	find_package(Python COMPONENTS Interpreter Development NumPy)
else()
	find_package(PythonInterp)
	find_package(PythonLibs ${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR})
	string(REGEX REPLACE ".*libpython([0-9])\\.[0-9]+.*\\..*" "\\1" Python_VERSION_MAJOR ${PYTHON_LIBRARIES})
	string(REGEX REPLACE ".*libpython[0-9]\\.([0-9]+).*\\..*" "\\1" Python_VERSION_MINOR ${PYTHON_LIBRARIES})
	set(Python_VERSION ${Python_VERSION_MAJOR}.${Python_VERSION_MINOR})
	set(Python_INCLUDE_DIRS ${PYTHON_INCLUDE_PATH})
	set(Python_LIBRARIES ${PYTHON_LIBRARIES})
	set(Python_EXECUTABLE ${PYTHON_EXECUTABLE})
endif()

message(STATUS "+  version: ${Python_VERSION}")

# Compatibility variables
set(PYTHON_EXECUTABLE ${Python_EXECUTABLE})
set(PYTHON_VERSION ${Python_VERSION})
set(PYTHON_STRIPPED_VERSION ${Python_VERSION})
set(PYTHON_LIBRARY ${Python_LIBRARIES})
set(PYTHON_LIBRARIES ${Python_LIBRARIES})
set(PYTHON_INCLUDE_DIR ${Python_INCLUDE_DIRS})

# In order to ensure all python executables (i.e. '#!/usr/bin/env python') run
# under both python2 and python3 after spawning a new shell via env-shell.sh
# we create a soft link in $I3_BUILD/bin to whatever python executable is found.
# This is really only needed on systems that contain both python2 and python3,
# where we don't want to force a system wide adoption of python3, potentially
# breaking other non-IceCube applications on the user's machine.
execute_process(COMMAND ln -sf ${PYTHON_EXECUTABLE} ${CMAKE_BINARY_DIR}/bin/python)

#
# Get the root dir of the python install
#
if(${Python_VERSION} VERSION_GREATER_EQUAL 3.0)
  execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import sys; sys.stdout.write(sys.real_prefix if hasattr(sys, 'real_prefix') else sys.prefix)"
                  OUTPUT_VARIABLE PYTHON_ROOT)
elseif(${Python_VERSION} VERSION_LESS 3.0)
  execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import sys; sys.stdout.write(sys.base_prefix)"
                  OUTPUT_VARIABLE PYTHON_ROOT)
endif(${Python_VERSION} VERSION_GREATER_EQUAL 3.0)
message(STATUS "+ base dir: ${PYTHON_ROOT}")

if(NOT EXISTS "${PYTHON_INCLUDE_DIR}/Python.h")
  message(STATUS "Error configuring python:  ${PYTHON_INCLUDE_DIR}/Python.h does not exist.\n")
  set(PYTHON_FOUND FALSE CACHE BOOL "Python found successfully")
  set(PYTHON_CONFIG_ERROR TRUE)
endif(NOT  EXISTS "${PYTHON_INCLUDE_DIR}/Python.h")

message(STATUS "+   binary: ${PYTHON_EXECUTABLE}")	
message(STATUS "+ includes: ${PYTHON_INCLUDE_DIR}")	
message(STATUS "+     libs: ${PYTHON_LIBRARIES}")

if(${Python_VERSION} VERSION_LESS 2.6)
  message(FATAL_ERROR "A Python version >= 2.6 is required.")
endif(${Python_VERSION} VERSION_LESS 2.6)

# look for numpy
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12)
  set(NUMPY_FOUND ${Python_NumPy_FOUND} CACHE BOOL "Numpy found successfully")
  set(NUMPY_INCLUDE_DIR ${Python_NumPy_INCLUDE_DIRS})
  message(STATUS "+    numpy: ${Python_NumPy_INCLUDE_DIRS}")
else(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12)
  # Old, crummy cmake -- try our best. Use better cmake for special cases.

  execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import numpy"
    RESULT_VARIABLE NUMPY_FOUND)
  # let's make our xxx_FOUND variable like CMake ones
  if(NUMPY_FOUND EQUAL 0)
    set(NUMPY_FOUND TRUE)
    set(NUMPY_FOUND TRUE CACHE BOOL "Numpy found successfully")
  else(NUMPY_FOUND EQUAL 0)
    set(NUMPY_FOUND FALSE CACHE BOOL "Numpy found successfully")
    set(NUMPY_FOUND FALSE)
  endif(NUMPY_FOUND EQUAL 0)

  execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
    "import numpy; print(numpy.get_include())"
    OUTPUT_VARIABLE _NUMPY_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  find_path(NUMPY_INCLUDE_DIR NAMES numpy/ndarrayobject.h HINTS ${_NUMPY_INCLUDE_DIR})
  set(NUMPY_INCLUDE_DIR ${NUMPY_INCLUDE_DIR} CACHE STRING "Numpy inc directory")
  message(STATUS "+    numpy: ${NUMPY_INCLUDE_DIR}")
endif(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12)

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


if(${Python_VERSION} VERSION_LESS 3.0)
  colormsg(RED "-  WARNING: Python<3.0 detected")
#omit python2 warning for now  
#  set(PYTHON_WARNING
#"WARNING: Your current version of python is ${Python_VERSION}.
#Python 2.7 is being retired and will not be maintained upstream past January 1, 
#2020. Therefore IceCube software must transition to python 3. You do not need to
#do anything at this moment, this version of IceCube software will still work 
#with python 2.7. However, you should consider migrating to python 3 at the next
# available opportunity.  Please see 
#http://software.icecube.wisc.edu/documentation/projects/cmake/tools/python.html
#for more details on how to migrate.")
else(${Python_VERSION} VERSION_LESS 3.0)
  set(PYTHON_WARNING "")
endif(${Python_VERSION} VERSION_LESS 3.0)

