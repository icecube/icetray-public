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

if(CMAKE_BUILD_TYPE MATCHES "Rel")
  set(BOOST_LIB_SUFFIX "")
else()
  set(BOOST_LIB_SUFFIX "-d")
endif()

if (NOT BOOST_VERSION)
  if (IS_DIRECTORY ${I3_PORTS}/include/boost-1.36.0)
    set(BOOST_VERSION "1.36.0" CACHE STRING "Boost (from tools) version")
  elseif(IS_DIRECTORY ${I3_PORTS}/include/boost-1.33.1)
    set(BOOST_VERSION "1.33.1" CACHE STRING "Boost (from tools) version")
  else()
    set(BOOST_VERSION "NOTFOUND" CACHE STRING "Boost (from tools) version")
  endif()
endif()

if (BOOST_VERSION)
  TOOLDEF (boost 
    include/boost-${BOOST_VERSION}
    boost/version.hpp
    lib/boost-${BOOST_VERSION}
    NONE
    boost_system${BOOST_LIB_SUFFIX} 
    boost_filesystem${BOOST_LIB_SUFFIX} 
    boost_serialization${BOOST_LIB_SUFFIX} 
    boost_program_options${BOOST_LIB_SUFFIX} 
    boost_regex${BOOST_LIB_SUFFIX} 
    boost_iostreams${BOOST_LIB_SUFFIX} 
    )
  include_directories(cmake/tool-patches/boost-${BOOST_VERSION})
endif(BOOST_VERSION)

if (BOOST_VERSION STREQUAL "1.33.1")
  set(BOOST_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/boost/public;${BOOST_INCLUDE_DIR})
  set(BOOST_PYTHON boost_python)
  set(BOOST_DATE_TIME boost_date_time)
  set(BOOST_THREAD boost_threads)
  set(BOOST_SYSTEM boost_system)
  set(BOOST_SIGNALS boost_signals)
else()
  find_library(BOOST_PYTHON boost_python${BOOST_LIB_SUFFIX} ${I3_PORTS}/lib/boost-1.36.0
    NO_DEFAULT_PATH)
  find_library(BOOST_SYSTEM boost_system${BOOST_LIB_SUFFIX} ${I3_PORTS}/lib/boost-1.36.0
    NO_DEFAULT_PATH)
  find_library(BOOST_SIGNALS boost_signals${BOOST_LIB_SUFFIX} ${I3_PORTS}/lib/boost-1.36.0
    NO_DEFAULT_PATH)
  find_library(BOOST_THREAD boost_thread-mt${BOOST_LIB_SUFFIX} ${I3_PORTS}/lib/boost-1.36.0
    NO_DEFAULT_PATH)
  find_library(BOOST_DATE_TIME boost_date_time${BOOST_LIB_SUFFIX} ${I3_PORTS}/lib/boost-1.36.0
    NO_DEFAULT_PATH)
  set(BOOST_LIBRARIES "${BOOST_LIBRARIES};${BOOST_SYSTEM}" CACHE PATH "Libraries for tool 'boost'" FORCE)
endif()

