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

# The following prevent these variables from being cached and force 
#   them to be rechecked across changes from Release/Debug builds.
SET(BOOST_PYTHON "" CACHE FILEPATH "Cleared with rebuild_cache." FORCE)
SET(BOOST_SYSTEM "" CACHE FILEPATH "Cleared with rebuild_cache." FORCE)
SET(BOOST_SIGNALS "" CACHE FILEPATH "Cleared with rebuild_cache." FORCE)
SET(BOOST_THREAD "" CACHE FILEPATH "Cleared with rebuild_cache." FORCE)
SET(BOOST_DATE_TIME "" CACHE FILEPATH "Cleared with rebuild_cache." FORCE)

if (NOT BOOST_VERSION)
  if (IS_DIRECTORY ${I3_PORTS}/include/boost-1.38.0)
    set(BOOST_VERSION "1.38.0" CACHE STRING "Boost (from tools) version")
  else()
    set(BOOST_VERSION "NOTFOUND" CACHE STRING "Boost (from tools) version")
  endif()
endif()

if (BOOST_VERSION STREQUAL "NOTFOUND")

  message(STATUS "Eh, not looking good for boost.  Include directories not found over in $I3_PORTS")

else()

  include_directories(${CMAKE_SOURCE_DIR}/cmake/tool-patches/boost-${BOOST_VERSION})

  if(CMAKE_BUILD_TYPE MATCHES "Rel")
    set(BOOST_LIB_SUFFIX "-mt")
  else()
    set(BOOST_LIB_SUFFIX "-mt-d")
  endif()

  set(BOOST_ALL_LIBRARIES 
    boost_python${BOOST_LIB_SUFFIX} 
    boost_system${BOOST_LIB_SUFFIX} 
    boost_signals${BOOST_LIB_SUFFIX}  
    boost_thread${BOOST_LIB_SUFFIX}  
    boost_date_time${BOOST_LIB_SUFFIX}  
    boost_serialization${BOOST_LIB_SUFFIX}  
    boost_filesystem${BOOST_LIB_SUFFIX}  
    boost_program_options${BOOST_LIB_SUFFIX}  
    boost_regex${BOOST_LIB_SUFFIX}  
    boost_iostreams${BOOST_LIB_SUFFIX})

  tooldef (boost
    ${I3_PORTS}/include/boost-${BOOST_VERSION}
    boost/version.hpp
    lib/boost-${BOOST_VERSION}
    NONE
    ${BOOST_ALL_LIBRARIES}
    )

endif()

