#
#  $Id$
#  
#  Copyright (C) 2012 the IceCube Collaboration <http://www.icecube.wisc.edu>
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

if (SYSTEM_PACKAGES)
  find_path(ZMQ_INCLUDE_DIR
    NAMES include/zmq.h)
  tooldef(zmq
    ${ZMQ_INCLUDE_DIR}
    zmq.h
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
  include(CMakePushCheckState)
  CMAKE_PUSH_CHECK_STATE()
  set(CMAKE_REQUIRED_INCLUDES "${ZMQ_INCLUDE_DIR}/include")
  CHECK_CXX_SYMBOL_EXISTS(
    CPPZMQ_VERSION_MAJOR
    "${ZMQ_INCLUDE_DIR}/include/zmq.hpp"
    _have_cppzmq_version)
    CMAKE_POP_CHECK_STATE()
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
