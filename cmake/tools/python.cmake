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

set(PYTHON_FOUND TRUE)
if(EXISTS ${I3_PORTS}/bin/python)
  message(STATUS "***")
  message(STATUS "***")
  message(STATUS "***  $I3_PORTS/bin/python exists in your toolset...  toolset is too old.")
  message(STATUS "***  You'll probably want to install the toolset fresh from the latest")
  message(STATUS "***  ports distribution")
  message(STATUS "***")
  message(STATUS "***")
  message(FATAL_ERROR "Stopping makefile generation.   Sorry.")
endif(EXISTS ${I3_PORTS}/bin/python)

find_package(PythonInterp)
find_package(PythonLibs)

if(NOT PYTHON_EXECUTABLE)
  set(PYTHON_FOUND FALSE)
endif(NOT PYTHON_EXECUTABLE)

# 
# determine version of the system python.
#
execute_process(COMMAND ${PYTHON_EXECUTABLE} -V
  ERROR_VARIABLE PYTHON_VERSION
  ERROR_STRIP_TRAILING_WHITESPACE)
#
# Provide version in numeric form for comparison
#
execute_process(COMMAND ${CMAKE_SOURCE_DIR}/cmake/pythonversion.pl ${PYTHON_VERSION}
  OUTPUT_VARIABLE PYTHON_NUMERIC_VERSION)

message(STATUS "+  version: ${PYTHON_VERSION}") 
set(PYTHON_INCLUDE_DIR ${PYTHON_INCLUDE_PATH})

if(NOT EXISTS "${PYTHON_INCLUDE_DIR}/Python.h")
  message(STATUS "Error configuring python:  ${PYTHON_INCLUDE_DIR}/Python.h does not exist.\n")
  set(PYTHON_FOUND FALSE)
  set(PYTHON_CONFIG_ERROR TRUE)
endif(NOT  EXISTS "${PYTHON_INCLUDE_DIR}/Python.h")

message(STATUS "+   binary: ${PYTHON_EXECUTABLE}")	
message(STATUS "+ includes: ${PYTHON_INCLUDE_DIR}")	
message(STATUS "+     libs: ${PYTHON_LIBRARIES}")	

# Python <2.4 does not include subprocess.py which is used by
# runtests.py and run_continuous_slave.py. Mark it for installation if
# needed.
#
set(INSTALL_PYTHON_SUBPROCESS FALSE)
if(PYTHON_NUMERIC_VERSION LESS 20400)
  message(STATUS "+ including subprocess module")
  set(INSTALL_PYTHON_SUBPROCESS TRUE)
endif(PYTHON_NUMERIC_VERSION LESS 20400)

# look for numpy
execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import numpy"
    RESULT_VARIABLE NUMPY_FOUND)

if(NOT NUMPY_FOUND EQUAL 0)
	set(NUMPY_FOUND FALSE CACHE BOOL "Numpy found successfully" FORCE)
else(NOT NUMPY_FOUND EQUAL 0)
	set(NUMPY_FOUND TRUE CACHE BOOL "Numpy found successfully" FORCE)
	execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
	    "from numpy.distutils.misc_util import get_numpy_include_dirs; print get_numpy_include_dirs()[0]"
	    OUTPUT_VARIABLE NUMPY_INCLUDE_DIR
	    OUTPUT_STRIP_TRAILING_WHITESPACE)
	set(NUMPY_INCLUDE_DIR ${NUMPY_INCLUDE_DIR} CACHE STRING "Numpy inc directory" FORCE)
	message(STATUS "+    numpy: ${NUMPY_INCLUDE_DIR}")
endif(NOT NUMPY_FOUND EQUAL 0)


