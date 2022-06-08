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

