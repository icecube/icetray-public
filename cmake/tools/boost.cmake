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

if(SYSTEM_PACKAGES)
	find_package(Boost COMPONENTS python)
endif(SYSTEM_PACKAGES)

if(Boost_INCLUDE_DIR)
	SET(BOOST_VERSION "new" CACHE PATH "The boost version.")
	SET(BOOST_INCLUDE_DIR ${Boost_INCLUDE_DIR} CACHE PATH "Path to the boost include directories." )
	SET(BOOST_LIBRARY_PATH ${Boost_LIBRARY_DIRS} CACHE PATH "Path to the boost libraries." )
	# Boost >= 1.42 has no library suffixes on *nix
	SET(BOOST_LIB_SUFFIX "")
else(Boost_INCLUDE_DIR)
	SET(BOOST_VERSION "1.38.0" CACHE PATH "The boost version.")
	SET(BOOST_INCLUDE_DIR  ${I3_PORTS}/include/boost-${BOOST_VERSION} CACHE PATH "Path to the boost include directories." )
	SET(BOOST_LIBRARY_PATH  ${I3_PORTS}/lib/boost-${BOOST_VERSION} CACHE PATH "Path to the boost libraries." )
	if(CMAKE_BUILD_TYPE MATCHES "Rel")
		set(BOOST_LIB_SUFFIX "-mt")
	else()
		set(BOOST_LIB_SUFFIX "-mt-d")
	endif()
endif(Boost_INCLUDE_DIR)

# The following prevent these variables from being cached and force 
#   them to be rechecked across changes from Release/Debug builds.
SET(BOOST_PYTHON "" CACHE FILEPATH "Cleared with rebuild_cache." FORCE)
SET(BOOST_SYSTEM "" CACHE FILEPATH "Cleared with rebuild_cache." FORCE)
SET(BOOST_SIGNALS "" CACHE FILEPATH "Cleared with rebuild_cache." FORCE)
SET(BOOST_THREAD "" CACHE FILEPATH "Cleared with rebuild_cache." FORCE)
SET(BOOST_DATE_TIME "" CACHE FILEPATH "Cleared with rebuild_cache." FORCE)

# need to include the right patches 
if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/cmake/tool-patches/boost-${BOOST_VERSION})
  include_directories(${CMAKE_SOURCE_DIR}/cmake/tool-patches/boost-${BOOST_VERSION})
else()
  message(ERROR " No tool patch found. ${CMAKE_SOURE_DIR}/cmake/tool-patches/boost-${BOOST_VERSION}.  This likely won't build.")
endif() 

if (NOT IS_DIRECTORY  ${BOOST_LIBRARY_PATH})
    message(ERROR " ${BOOST_LIBRARY_PATH} is not a valid directory.")
endif() 

if (NOT IS_DIRECTORY ${BOOST_INCLUDE_DIR} )
    message(ERROR " ${BOOST_INCLUDE_DIR} is not a valid directory.")
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
  ${BOOST_INCLUDE_DIR}
  boost/version.hpp
  ${BOOST_LIBRARY_PATH}
  NONE
  ${BOOST_ALL_LIBRARIES}
  )


