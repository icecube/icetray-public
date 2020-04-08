#
#  $Id$
#  
#  Copyright (C) 2012 the IceCube Collaboration <http://www.icecube.wisc.edu>
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

if (SYSTEM_PACKAGES)
  find_path(ZMQ_INCLUDE_DIR
    NAMES include/zmq.hpp)
  tooldef(zmq
    ${ZMQ_INCLUDE_DIR}
    zmq.hpp
    lib
    NONE
    zmq
    )
endif ()

if (ZMQ_FOUND AND NOT ZMQ_VERSION)
  file(READ "${ZMQ_INCLUDE_DIR}/include/zmq.h" _ZMQ_H_CONTENTS)
  string(REGEX REPLACE ".*#define ZMQ_VERSION_MAJOR ([0-9]+).*" "\\1" ZMQ_VERSION_MAJOR "${_ZMQ_H_CONTENTS}")
  string(REGEX REPLACE ".*#define ZMQ_VERSION_MINOR ([0-9]+).*" "\\1" ZMQ_VERSION_MINOR "${_ZMQ_H_CONTENTS}")
  string(REGEX REPLACE ".*#define ZMQ_VERSION_PATCH ([0-9]+).*" "\\1" ZMQ_VERSION_PATCH "${_ZMQ_H_CONTENTS}")
  set(ZMQ_VERSION "${ZMQ_VERSION_MAJOR}.${ZMQ_VERSION_MINOR}.${ZMQ_VERSION_PATCH}")
endif ()

# Detect cppzmq versions from before there were versions
if (ZMQ_FOUND AND NOT CPPZMQ_VERSION)
  include(CheckCXXSymbolExists)
  if (CMAKE_VERSION VERSION_GREATER "2.8.5")
    include(CMakePushCheckState)
    CMAKE_PUSH_CHECK_STATE()
  endif ()
  set(CMAKE_REQUIRED_INCLUDES "${ZMQ_INCLUDE_DIR}/include")
  CHECK_CXX_SYMBOL_EXISTS(
    CPPZMQ_VERSION_MAJOR
    "${ZMQ_INCLUDE_DIR}/include/zmq.hpp"
    _have_cppzmq_version)
  if (CMAKE_VERSION VERSION_GREATER "2.8.5")
    CMAKE_POP_CHECK_STATE()
  endif ()
  # This is how mainline cppzmq do version detection.
  if (_have_cppzmq_version)
    file(READ "${ZMQ_INCLUDE_DIR}/include/zmq.hpp" _CPPZMQ_H_CONTENTS)
    string(REGEX REPLACE ".*#define CPPZMQ_VERSION_MAJOR ([0-9]+).*" "\\1" CPPZMQ_VERSION_MAJOR "${_CPPZMQ_H_CONTENTS}")
    string(REGEX REPLACE ".*#define CPPZMQ_VERSION_MINOR ([0-9]+).*" "\\1" CPPZMQ_VERSION_MINOR "${_CPPZMQ_H_CONTENTS}")
    string(REGEX REPLACE ".*#define CPPZMQ_VERSION_PATCH ([0-9]+).*" "\\1" CPPZMQ_VERSION_PATCH "${_CPPZMQ_H_CONTENTS}")
    set(CPPZMQ_VERSION "${CPPZMQ_VERSION_MAJOR}.${CPPZMQ_VERSION_MINOR}.${CPPZMQ_VERSION_PATCH}")
  else ()
    # a copy from before there was a release policy or real versioning
    set(CPPZMQ_VERSION "4.2.0")
  endif ()
endif ()
