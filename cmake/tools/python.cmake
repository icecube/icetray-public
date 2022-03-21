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

find_package(Python 3.6 QUIET REQUIRED COMPONENTS Interpreter Development NumPy)
message(STATUS "+  version: ${Python_VERSION}")
message(STATUS "+   binary: ${Python_EXECUTABLE}")
message(STATUS "+ includes: ${Python_INCLUDE_DIRS}")
message(STATUS "+     libs: ${Python_LIBRARIES}")

# Compatibility variables -- need to be cached for parasitic projects
set(PYTHON_FOUND ${PYTHON_FOUND} CACHE BOOL "Python found successfully")
set(PYTHON_EXECUTABLE ${Python_EXECUTABLE} CACHE STRING "Python executable")
set(PYTHON_VERSION ${Python_VERSION} CACHE STRING "Python Version Number")
set(PYTHON_LIBRARY ${Python_LIBRARIES} CACHE STRING "Python libraries")
set(PYTHON_LIBRARIES ${Python_LIBRARIES} CACHE STRING "Python libraries")
set(PYTHON_INCLUDE_DIR ${Python_INCLUDE_DIRS} CACHE STRING "Python include directories")

# look for numpy
set(NUMPY_FOUND ${Python_NumPy_FOUND} CACHE BOOL "Numpy found successfully")
set(NUMPY_INCLUDE_DIR ${Python_NumPy_INCLUDE_DIRS} CACHE STRING "Numpy inc directory")
message(STATUS "+    numpy: ${Python_NumPy_INCLUDE_DIRS}")

## look for scipy
execute_process(COMMAND ${Python_EXECUTABLE} -c "import scipy"
    RESULT_VARIABLE SCIPY_FOUND OUTPUT_QUIET ERROR_QUIET)

# let's make our xxx_FOUND variable like CMake ones
if(SCIPY_FOUND EQUAL 0)
  set(SCIPY_FOUND TRUE)
  message(STATUS "+    scipy: FOUND")
else()
  message(STATUS "+    scipy: NOT FOUND")
  set(SCIPY_FOUND FALSE)
endif()
set(SCIPY_FOUND ${SCIPY_FOUND} CACHE BOOL "Scipy found successfully")

