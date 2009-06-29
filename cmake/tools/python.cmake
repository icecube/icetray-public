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
message(STATUS "python ...")

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

if(NOT PYTHON_EXECUTABLE)

  set(PYTHON_FOUND FALSE)

elseif(APPLE)

  find_package(PythonLibs)

else()

  message(STATUS "+   binary: ${PYTHON_EXECUTABLE}")	

  execute_process(COMMAND 
    ${PYTHON_EXECUTABLE} -c "import sys; print sys.version[:3]"
    OUTPUT_VARIABLE PYTHON_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  message(STATUS "+  version: ${PYTHON_VERSION}") 

  execute_process(COMMAND 
    ${PYTHON_EXECUTABLE} -c 
    "import sys; print '%s/include/python%s' % (sys.prefix, sys.version[:3])"
    OUTPUT_VARIABLE include_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  if (EXISTS ${include_dir}/Python.h)
    set(PYTHON_INCLUDE_DIR ${include_dir} 
      CACHE PATH 
      "Python include directory")
    message(STATUS "+ includes: ${PYTHON_INCLUDE_DIR}")	
  else()
    message(STATUS "Error configuring python:  ${PYTHON_INCLUDE_DIR}/Python.h does not exist.\n")
    set(PYTHON_FOUND FALSE)
    set(PYTHON_CONFIG_ERROR TRUE)
  endif()

  #
  # YAAAY HACK
  #
  # Shout to my people keeping it real with python 2.3
  #
  if (PYTHON_VERSION STREQUAL "2.3")
    set(lib_finder
      "import sys, distutils.sysconfig; print '%s/lib/libpython%s.so' % (distutils.sysconfig.get_python_lib(0,1), sys.version[:3])"
      )
  else()
    set(lib_finder
      "import sys; print '%s/lib/libpython%s.so' % (sys.prefix, sys.version[:3])"
      )
  endif()  

  execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "${lib_finder}"
    OUTPUT_VARIABLE python_lib
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  if (EXISTS ${python_lib})
    set(PYTHON_LIBRARIES ${python_lib} CACHE FILEPATH "Python library")
  else()
    message(STATUS "Error configuring python:  ${python_lib} does not exist.\n")
    set(PYTHON_FOUND FALSE)
    set(PYTHON_CONFIG_ERROR TRUE)
  endif()

  message(STATUS "+     libs: ${PYTHON_LIBRARIES}")	


endif(NOT PYTHON_EXECUTABLE)

# Python <2.4 does not include subprocess.py which is used by
# runtests.py and run_continuous_slave.py. Mark it for installation if
# needed.
#
if(PYTHON_VERSION STREQUAL "2.3")
  message(STATUS "+ including subprocess module")
  set(INSTALL_PYTHON_SUBPROCESS TRUE)
else()
  set(INSTALL_PYTHON_SUBPROCESS FALSE)
endif()

