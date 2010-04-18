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

set(BUILD_SHARED_LIBS SHARED)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(DOXYGEN_OUTPUT_PATH ${CMAKE_BINARY_DIR}/docs/doxygen)

if("$ENV{I3_PORTS}" STREQUAL "")
  message(STATUS "I3_PORTS not set, maybe not a problem.  Trying default of /opt/i3/ports.")
  set(ENV{I3_PORTS} "/opt/i3/ports")
endif("$ENV{I3_PORTS}" STREQUAL "")

set(I3_PORTS $ENV{I3_PORTS} CACHE STRING "Path to your icecube ports installation" FORCE)

if(NOT IS_DIRECTORY $ENV{I3_PORTS})
  message(FATAL_ERROR "I3_PORTS ($ENV{I3_PORTS}) is unset or doesn't point to a directory.")
else(NOT IS_DIRECTORY $ENV{I3_PORTS})
  message(STATUS "I3_PORTS\t${I3_PORTS}")
endif(NOT IS_DIRECTORY $ENV{I3_PORTS})

#
#  GCC_NUMERIC_VERSION is e.g. 40302 (for 4.3.2)
#
execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion 
  COMMAND ${CMAKE_SOURCE_DIR}/cmake/gccversion.pl
  OUTPUT_VARIABLE GCC_NUMERIC_VERSION)
set(GCC_NUMERIC_VERSION ${GCC_NUMERIC_VERSION} CACHE INTEGER "Numeric gcc version" FORCE)

# get just the filename
get_filename_component(CXX_COMPILER_BINNAME ${CMAKE_CXX_COMPILER} NAME)

#
# Unfortunately cmake doesn't do this on its own
#
if(CXX_COMPILER_BINNAME STREQUAL "icpc")
  set(CMAKE_COMPILER_IS_INTEL TRUE)
endif(CXX_COMPILER_BINNAME STREQUAL "icpc")

if(CMAKE_COMPILER_IS_INTEL)
  set(WARNING_FLAGS "-w1 -Wno-non-virtual-dtor")
else(CMAKE_COMPILER_IS_INTEL)
  set(WARNING_FLAGS "-Wall -Wno-non-virtual-dtor")
endif(CMAKE_COMPILER_IS_INTEL)

#
# temporary switch for testing fast omkey hash
#
option(USE_FAST_OMKEY_MAP "Use the experimental fast OMKey map" OFF)
if (USE_FAST_OMKEY_MAP)
  add_definitions(-DI3_USE_FAST_OMKEY_MAP)
endif (USE_FAST_OMKEY_MAP)

option(USE_PYTHON_LOGGING "Log to python, not to log4cplus" OFF)
if (USE_PYTHON_LOGGING)
  add_definitions(-DI3_PYTHON_LOGGING)
endif (USE_PYTHON_LOGGING)

set(NOTES_DIR ${CMAKE_BINARY_DIR}/Testing/Notes)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Testing/Notes)

execute_process(COMMAND svn info ${CMAKE_SOURCE_DIR} 
  OUTPUT_FILE ${NOTES_DIR}/svn_info.txt)

execute_process(COMMAND /usr/bin/env 
  OUTPUT_FILE ${NOTES_DIR}/env.txt)

execute_process(COMMAND uname -a 
  OUTPUT_FILE ${NOTES_DIR}/uname.txt)

execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version 
  OUTPUT_FILE ${NOTES_DIR}/compiler-version.txt)

#
# Get GCC_VERSION
#
execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
  COMMAND tr \\n \\0 
  OUTPUT_VARIABLE GCC_VERSION)

#
# Get OSTYPE
#
execute_process(COMMAND uname -s 
  COMMAND tr \\n \\0
  OUTPUT_VARIABLE OSTYPE)
message(STATUS "OSTYPE\t${OSTYPE}")

#
# Get OSVERSION
#
execute_process(COMMAND uname -r COMMAND tr \\n \\0 
  OUTPUT_VARIABLE OSVERSION)
message(STATUS "OSVERSION\t${OSVERSION}")

#
# Get ARCH
#
execute_process(COMMAND uname -m 
  COMMAND sed -e "s/Power\\ Macintosh/ppc/"
  COMMAND tr \\n \\0
  OUTPUT_VARIABLE ARCH)
message(STATUS "ARCH     \t${ARCH}")

#
# Assemble BUILDNAME
#
set(BUILDNAME "${OSTYPE}-${OSVERSION}/${ARCH}/gcc-${GCC_VERSION}" CACHE INTERNAL "buildname")
message(STATUS "BUILDNAME\t${BUILDNAME}")

set(TOOLSET "gcc-${GCC_VERSION}/${ARCH}/${CMAKE_BUILD_TYPE}" CACHE INTERNAL "toolset")

#
#  Get HOSTNAME 
#
execute_process(COMMAND hostname 
  COMMAND tr \\n \\0
  OUTPUT_VARIABLE HOSTNAME)
message(STATUS "HOSTNAME\t${HOSTNAME}")
set(SITE ${HOSTNAME})

#
# Find the svn program
#
find_program(SVN_EXECUTABLE svn)

#
# Get SVN_REVISION from svn info --xml
#
option(USE_SVN_REVISION_FLAGS "Add compiled-in svn revision information." ON)

if(NOT HAVE_SVN_REVISION)
  set(HAVE_SVN_REVISION TRUE CACHE INTERNAL "flag")
  execute_process(COMMAND ${SVN_EXECUTABLE} info --xml ${CMAKE_SOURCE_DIR} 
    COMMAND perl -e "$_ = join(\"\", <>) ; m{<entry[^>]+revision=\"([0-9]+)}s && print $1"
    OUTPUT_VARIABLE SVN_REVISION
    ERROR_VARIABLE SVN_REVISION_ERROR)

  if (SVN_REVISION_ERROR OR NOT SVN_EXECUTABLE)
    message(STATUS "******\tUnable to determine svn revision (does svn info not accept '--xml'?)")
    set(SVN_REVISION "0")
  endif (SVN_REVISION_ERROR OR NOT SVN_EXECUTABLE)

  if(NOT USE_SVN_REVISION_FLAGS)
    message(STATUS "            \tNot compiling in svn revision information.  Hope you aren't running simprod")
    set(SVN_REVISION "0")
  endif(NOT USE_SVN_REVISION_FLAGS)

  message(STATUS "SVN_REVISION\t${SVN_REVISION}")
  set(SVN_REVISION ${SVN_REVISION} CACHE INTERNAL "svn revision")

endif(NOT HAVE_SVN_REVISION)

#
# Get SVN_URL from svn info --xml
#
if(NOT HAVE_SVN_URL)
  set(HAVE_SVN_URL TRUE CACHE INTERNAL "flag")
  execute_process(COMMAND ${SVN_EXECUTABLE} info --xml ${CMAKE_SOURCE_DIR} 
    COMMAND perl -e "$_ = join(\"\",<>) ; m{<url>([^<]+)</url>}s && print $1" 
    OUTPUT_VARIABLE SVN_URL)

  if(NOT SVN_EXECUTABLE)
    set(SVN_URL "Unknown")
  endif(NOT SVN_EXECUTABLE)

  if (NOT SVN_URL)
    set(SVN_URL "Unknown")
  endif(NOT SVN_URL)
  message(STATUS "SVN_URL\t${SVN_URL}")

  set(SVN_URL ${SVN_URL} CACHE INTERNAL "svn url")

endif(NOT HAVE_SVN_URL)

#
#  this gets written to the file #included by the I3TrayInfoService.  the 
#  \n's need to be there.
#
execute_process(COMMAND svn pg svn:externals ${CMAKE_SOURCE_DIR}
  COMMAND perl -ne "s/\n/\\\\n/ ; print"
  OUTPUT_VARIABLE SVN_EXTERNALS)

#
#  get META_PROJECT (used by dart) from SVN_URL
#
if(NOT HAVE_META_PROJECT)
  set(HAVE_META_PROJECT TRUE CACHE INTERNAL "flag")
  execute_process(COMMAND echo ${SVN_URL}
    COMMAND perl -pe "s@http://code.icecube.wisc.edu/svn/meta-projects/@@"
    COMMAND tr / .
    COMMAND tr \\n \\0
    OUTPUT_VARIABLE META_PROJECT
    RESULT_VARIABLE RESULT
    )

  if(RESULT)
    message(FATAL_ERROR "Problem running sed.")
  endif(RESULT)
  message(STATUS "META_PROJECT\t${META_PROJECT}")
  set(META_PROJECT ${META_PROJECT} CACHE INTERNAL "meta project")
endif(NOT HAVE_META_PROJECT)

#
#  get svn exten
#

# JC wants it like this.  TDS: Looking in to better ways to do this 'make tarball' stuff.
# Note #2:  set it to something reasonable if it equals /usr/local.
#           what a hack.
if (CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
  if (NOT "${META_PROJECT}" STREQUAL "Unknown")
    set(CMAKE_INSTALL_PREFIX ${META_PROJECT}.r${SVN_REVISION}.${OSTYPE}-${ARCH}.gcc-${GCC_VERSION} CACHE STRING "Install prefix.  Also name of tarball." FORCE)
  else (NOT "${META_PROJECT}" STREQUAL "Unknown")
    set(CMAKE_INSTALL_PREFIX ${OSTYPE}-${ARCH}.gcc-${GCC_VERSION} CACHE STRING "Install prefix.  Also name of tarball." FORCE)
  endif (NOT "${META_PROJECT}" STREQUAL "Unknown")
endif(CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")

set(I3_UBER_HEADER ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/I3.h)
configure_file(${CMAKE_SOURCE_DIR}/cmake/I3.h.in
  ${I3_UBER_HEADER}
  @ONLY)

#
# -D's for everybody
#
add_definitions(-D_REEENTRANT -fPIC)

#
# libraries everybody links to
#
if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
	# FreeBSD keeps libdl stuff in libc
	link_libraries(m stdc++)
else (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
	link_libraries(m dl stdc++)
endif (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")

set(TESTING_LD_LIBRARY_PATH $ENV{LD_LIBRARY_PATH}:${CMAKE_BINARY_DIR}/lib:${I3_PORTS}/lib:${I3_PORTS}/lib/boost-1.33.1)
set(TESTING_PATH ${I3_PORTS}/bin:${CMAKE_BINARY_DIR}/bin:$ENV{PATH})
set(TESTING_PYTHONPATH ${CMAKE_BINARY_DIR}/lib:${ports}/bin:${CMAKE_BINARY_DIR}/lib/python)
set(TESTING_I3_WORK ${CMAKE_SOURCE_DIR})

if(APPLE)
  set(CMAKE_CXX_CREATE_SHARED_LIBRARY "/usr/bin/env MACOSX_DEPLOYMENT_TARGET=10.4 ${CMAKE_CXX_CREATE_SHARED_LIBRARY}")
  set(CMAKE_CXX_CREATE_SHARED_MODULE "/usr/bin/env MACOSX_DEPLOYMENT_TARGET=10.4 ${CMAKE_CXX_CREATE_SHARED_MODULE}")
endif(APPLE)

#
#  distcc
#
find_program(DISTCC_PROGRAM distcc)
if(DISTCC_PROGRAM)
  message(STATUS "distcc found at ${DISTCC_PROGRAM}")

  option(USE_DISTCC "Use distcc" OFF)
  if(USE_DISTCC)
    if(DISTCC_PROGRAM)
      set(CMAKE_CXX_COMPILE_OBJECT "${DISTCC_PROGRAM} ${CMAKE_CXX_COMPILE_OBJECT}")
    else(DISTCC_PROGRAM)
      message(STATUS "USE_DISTCC enabled, but distcc not found: disabling.")
    endif(DISTCC_PROGRAM)
  endif(USE_DISTCC)

else(DISTCC_PROGRAM)
  message(STATUS "distcc not found.")
endif(DISTCC_PROGRAM)


#
#  ccache
#
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  message(STATUS "ccache found at ${CCACHE_PROGRAM}")
  option(USE_CCACHE "Use ccache" OFF)

  if(USE_CCACHE)
    if(CCACHE_PROGRAM)
      set(CMAKE_CXX_COMPILE_OBJECT "${CCACHE_PROGRAM} ${CMAKE_CXX_COMPILE_OBJECT}")
      execute_process(COMMAND ${CCACHE_PROGRAM} -s 
	OUTPUT_FILE ${NOTES_DIR}/ccache-s.txt 
	OUTPUT_VARIABLE NOWHERE)
    else(CCACHE_PROGRAM)
      message(STATUS "USE_CCACHE enabled, but ccache not found: disabling.")
      file(WRITE ${NOTES_DIR}/ccache-s.txt "USE_CCACHE enabled but ccache binary not found.")
    endif(CCACHE_PROGRAM)
  endif(USE_CCACHE)

else(CCACHE_PROGRAM)
  message(STATUS "ccache not found.")
endif(CCACHE_PROGRAM)

#
#  gfilt
#
if (USE_CCACHE)
  option(USE_GFILT "Use gfilt STL error decryptor" OFF)
else()
  option(USE_GFILT "Use gfilt STL error decryptor" ON)
endif()

if(USE_GFILT)
  set(CMAKE_CXX_COMPILE_OBJECT "${GFILT_PROGRAM} ${CMAKE_CXX_COMPILE_OBJECT}")
  message(STATUS "Using gfilt stl decryptor")
endif(USE_GFILT)

#
#  For now, on gcc 4.3.2, add the -Wno-deprecated flag
#
if (GCC_NUMERIC_VERSION GREATER 40299) 
  set(CXX_WARNING_SUPRESSION_FLAGS "-Wno-deprecated -Wno-parentheses"
    CACHE STRING "Warning supression flags for this compiler")
endif (GCC_NUMERIC_VERSION GREATER 40299)

#
#  Detect certain old platforms and reduce optimization levels accordingly
#
set(RELOPTLEVEL 3)

#
#  Reduce on FC4 (gah)
#
if(EXISTS /etc/issue)
  file(READ /etc/issue ETC_ISSUE)
  if(${ETC_ISSUE} MATCHES "Fedora Core release 4")
    message(STATUS "You're on FC4, a buggy platform.  Reducing compiler optimizations.  Your code will run more slowly.")
    set(RELOPTLEVEL 2)
  endif(${ETC_ISSUE} MATCHES "Fedora Core release 4")
endif(EXISTS /etc/issue)

#
# These are defaults that we need to force into the cache the first time, but not
# afterwards.  Notice the block around METAPROJECT_CONFIGURED.
#
if(NOT METAPROJECT_CONFIGURED)
  message(STATUS "Setting default compiler flags and build type.")

  #
  #  Check if it is defined...   if somebody has specified it on the 
  #  cmake commmand line, e.g. 
  #  cmake -DCMAKE_BUILD_TYPE:STRING=Debug, we need to use that value
  #
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel" FORCE)
  endif (NOT CMAKE_BUILD_TYPE)

  set(CMAKE_CXX_FLAGS "${WARNING_FLAGS} ${CMAKE_CXX_FLAGS} ${CXX_WARNING_SUPRESSION_FLAGS}")
  set(CMAKE_CXX_FLAGS "${WARNING_FLAGS} ${CMAKE_CXX_FLAGS}" CACHE STRING 
    "Flags used by the compiler during all build types" FORCE)
  
  set(CMAKE_CXX_FLAGS_RELEASE "-O${RELOPTLEVEL} -Wno-unused-variable -DNDEBUG -DI3_OPTIMIZE")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING
    "Flags used by compiler during release builds" FORCE)

  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O${RELOPTLEVEL} -Wno-unused-variable -g -DNDEBUG -DI3_OPTIMIZE")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE STRING
    "Flags used by compiler during release builds" FORCE)

  set(METAPROJECT_CONFIGURED TRUE CACHE INTERNAL "Metaproject configured")
endif(NOT METAPROJECT_CONFIGURED)
  
