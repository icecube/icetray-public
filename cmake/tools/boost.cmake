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

if(NOT SYSTEM_PACKAGES)
  set(BOOST_PORTSVERSION "1.38.0" CACHE PATH "The boost version.")
  set(BOOST_INCLUDEDIR ${I3_PORTS}/include/boost-${BOOST_PORTSVERSION})
  set(BOOST_LIBRARYDIR ${I3_PORTS}/lib/boost-${BOOST_PORTSVERSION})
  set(Boost_NO_SYSTEM_PATHS TRUE)
endif(NOT SYSTEM_PACKAGES)

colormsg("")
colormsg(HICYAN "Boost")
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
if (SYSTEM_PACKAGES)
        find_package(Boost COMPONENTS python system signals thread date_time serialization filesystem program_options regex iostreams)
else (SYSTEM_PACKAGES)
        find_package(Boost ${BOOST_PORTSVERSION} EXACT COMPONENTS python system signals thread date_time serialization filesystem program_options regex iostreams)
endif (SYSTEM_PACKAGES)

if(Boost_FOUND)
  set(BOOST_FOUND TRUE CACHE BOOL "Boost found successfully" FORCE)
  set(BOOST_INCLUDE_DIR ${Boost_INCLUDE_DIR} CACHE PATH "Path to the boost include directories.")
  set(BOOST_LIBRARIES ${Boost_LIBRARIES} CACHE PATH "Boost libraries")
  if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/cmake/tool-patches/boost-${BOOST_PORTSVERSION})
    include_directories(${CMAKE_SOURCE_DIR}/cmake/tool-patches/boost-${BOOST_PORTSVERSION})
  else()
    include_directories(${CMAKE_SOURCE_DIR}/cmake/tool-patches/boost-new)
  endif()

  foreach(lib ${BOOST_LIBRARIES})
    if(NOT ${lib} STREQUAL "optimized" AND NOT ${lib} STREQUAL "debug")
      add_custom_command(TARGET install_tool_libs
        PRE_BUILD
        COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/lib/tools
        COMMAND ${CMAKE_SOURCE_DIR}/cmake/install_shlib.py ${lib} ${CMAKE_INSTALL_PREFIX}/lib/tools) 
    endif(NOT ${lib} STREQUAL "optimized" AND NOT ${lib} STREQUAL "debug")
  endforeach(lib ${BOOST_LIBRARIES})
endif(Boost_FOUND)

