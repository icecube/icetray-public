#
#  $Id$
#
#  Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
#  Copyright (C) 2007   the IceCube Collaboration <http://www.icecube.wisc.edu>
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

find_package(Python 3.8 QUIET REQUIRED COMPONENTS Interpreter Development NumPy)

# platlib is the directory where python searches for user provided platform dependent modules. This command asks 
# sysconfig for the name to use relative to the root of the install prefix, it will be something like
# `lib/pythonX.Y/site-packages`. For inplace builds it will be relative to $I3_BUILD for tarball installs it will 
# be relative to the root of the tarball install. 
# Note: it is possible to use ${Python_SITEARCH} instead and strip the prefix from that but this is easier
execute_process(COMMAND 
  ${Python_EXECUTABLE} -c "import os,sysconfig;print(sysconfig.get_path('platlib','posix_prefix',{'platbase':''}).strip(os.sep))"
  OUTPUT_VARIABLE Python_PLATLIB
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# This command gets the extension suffix of of python modules from sysconfig. It is something like 
# `.cpython-39-x86_64-linux-gnu.so` for linux and `cpython-313-darwin.so` for macos. 
# Note: it is possible to use ${Python_SOABI} but you need to add the `.so` suffix and this is easier
execute_process(COMMAND 
  ${Python_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX'))"
  OUTPUT_VARIABLE Python_EXT_SUFFIX
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "+  version: ${Python_VERSION}")
message(STATUS "+   binary: ${Python_EXECUTABLE}")
message(STATUS "+ includes: ${Python_INCLUDE_DIRS}")
message(STATUS "+     libs: ${Python_LIBRARIES}")
message(STATUS "+  platlib: ${Python_PLATLIB}")
message(STATUS "+   suffix: ${Python_EXT_SUFFIX}")

# Compatibility variables -- need to be cached for parasitic projects
set(PYTHON_FOUND ${PYTHON_FOUND} CACHE BOOL "Python found successfully")
set(PYTHON_EXECUTABLE ${Python_EXECUTABLE} CACHE STRING "Python executable")
set(PYTHON_VERSION ${Python_VERSION} CACHE STRING "Python Version Number")
set(PYTHON_LIBRARY ${Python_LIBRARIES} CACHE STRING "Python libraries")
set(PYTHON_LIBRARIES ${Python_LIBRARIES} CACHE STRING "Python libraries")
set(PYTHON_INCLUDE_DIR ${Python_INCLUDE_DIRS} CACHE STRING "Python include directories")
set(PYTHON_PLATLIB_DIR ${CMAKE_BINARY_DIR}/${Python_PLATLIB} CACHE STRING "Python binary output directory")

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

