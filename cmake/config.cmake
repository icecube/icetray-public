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
colormsg("")
colormsg(_HIBLUE_ "IceCube Configuration starting")
colormsg("")

set(BUILD_SHARED_LIBS SHARED)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(DOXYGEN_OUTPUT_PATH ${CMAKE_BINARY_DIR}/docs/doxygen)

#
#  Check build sanity
#
if(DEFINED ENV{I3_BUILD})
  if(NOT "$ENV{I3_BUILD}" STREQUAL "${I3_BUILD}")
    message("***")
    message("*** You appear to be trying to configure cmake from within an environment with a different workspace")
    message("*** I3_BUILD = $ENV{I3_BUILD}")
    message("*** CMAKE_BINARY_DIR = ${I3_BUILD}")
    message("***")
    message("*** Retry from a clean environment (exit your env-shell.sh)")
    message("***")
    message(FATAL_ERROR "Environment corrupt")
  else()
    message(STATUS "I3_BUILD appears to be sane.")
  endif()
else()
  #message(STATUS "I3_BUILD not yet set in environment.  ok.")
endif()

if(DEFINED ENV{I3_SRC})
  if(NOT "$ENV{I3_SRC}" STREQUAL "${I3_SRC}")
    message("***")
    message("*** You appear to be trying to configure cmake from within an environment with a different workspace")
    message("*** I3_SRC = $ENV{I3_SRC}")
    message("*** CMAKE_SOURCE_DIR = ${I3_SRC}")
    message("***")
    message("*** Retry from a clean environment (exit your env-shell.sh)")
    message("***")
    message(FATAL_ERROR "Environment corrupt")
  else()
    message(STATUS "I3_SRC appears to be sane.")
  endif()
else()
  #message(STATUS "I3_SRC not yet set in environment.  ok.")
endif()

set(I3_PORTS $ENV{I3_PORTS} CACHE STRING "Path to your icecube ports installation" FORCE)

if(SYSTEM_PACKAGES)
  if(IS_DIRECTORY $ENV{I3_PORTS})
    boost_report_value(I3_PORTS)
  endif(IS_DIRECTORY $ENV{I3_PORTS})
else(SYSTEM_PACKAGES)
  if("$ENV{I3_PORTS}" STREQUAL "")
    message(STATUS "I3_PORTS not set, maybe not a problem.  Trying default of /opt/i3/ports.")
    set(ENV{I3_PORTS} "/opt/i3/ports")
  endif("$ENV{I3_PORTS}" STREQUAL "")

  if(NOT IS_DIRECTORY $ENV{I3_PORTS})
    message(FATAL_ERROR "I3_PORTS ($ENV{I3_PORTS}) is unset or doesn't point to a directory. If this is intentional, please set -DSYSTEM_PACKAGES=True")
  else(NOT IS_DIRECTORY $ENV{I3_PORTS})
    boost_report_value(I3_PORTS)
  endif(NOT IS_DIRECTORY $ENV{I3_PORTS})
endif(SYSTEM_PACKAGES)

if(DEFINED ENV{I3_TESTDATA})
    set(I3_TESTDATA $ENV{I3_TESTDATA} CACHE STRING "Path to your icetray test-data" FORCE)
elseif(DEFINED ENV{I3_PORTS})
    set(I3_TESTDATA $ENV{I3_PORTS}/test-data CACHE STRING "Path to your icetray test-data" FORCE)
else()
    colormsg(YELLOW "*** Neither I3_PORTS nor I3_TESTDATA set.")
    colormsg(YELLOW "*** Make sure to define at least one of them before calling 'make test' or manually running tests.")
    set(I3_TESTDATA "" CACHE STRING "Path to your icetray test-data: currently empty, define it if you wish to run unit tests and/or test scripts." FORCE)
endif(DEFINED ENV{I3_TESTDATA})

#
# Create various info/debug files
#

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
#  COMPILER_ID_TAG
#
execute_process(COMMAND ${CMAKE_CXX_COMPILER} -v ERROR_VARIABLE COMPILER_ID_TAG)
set(COMPILER_ID_TAG "REGEXPS IN CMAKE SUCK\n${COMPILER_ID_TAG}")
STRING(REGEX REPLACE ".*[ 
\\t]([a-zA-Z]+)[ -][Vv]ersion ([^ ]+).*" "\\1-\\2" COMPILER_ID_TAG ${COMPILER_ID_TAG})

#
# Get system info
#
set(OSTYPE ${CMAKE_SYSTEM_NAME})
boost_report_value(OSTYPE)

set(OSVERSION ${CMAKE_SYSTEM_VERSION})
boost_report_value(OSVERSION)

set(ARCH ${CMAKE_SYSTEM_PROCESSOR})
boost_report_value(ARCH)

set(BUILDNAME "${OSTYPE}-${OSVERSION}/${ARCH}/${COMPILER_ID_TAG}" CACHE INTERNAL "buildname")
boost_report_value(BUILDNAME)

set(TOOLSET "${COMPILER_ID_TAG}/${ARCH}/${CMAKE_BUILD_TYPE}" CACHE INTERNAL "toolset")
boost_report_value(TOOLSET)

execute_process(COMMAND hostname
  COMMAND tr -d \\n
  OUTPUT_VARIABLE HOSTNAME)
boost_report_value(HOSTNAME)
set(SITE ${HOSTNAME})

#
# Show cmake path and version
#
boost_report_pretty("CMake path" CMAKE_COMMAND)
if(NOT CMAKE_VERSION)
  set(CMAKE_VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION})
endif(NOT CMAKE_VERSION)
math(EXPR CMAKE_VERSION_INT "${CMAKE_MAJOR_VERSION} * 10000 + ${CMAKE_MINOR_VERSION} * 100 + ${CMAKE_PATCH_VERSION}")
boost_report_pretty("CMake version" CMAKE_VERSION)

#
# Find the svn program
#
find_program(SVN_EXECUTABLE svn)

#
# Get SVN_REVISION from svn info
#
option(USE_SVN_REVISION_FLAGS "Add compiled-in svn revision information." ON)

if(NOT HAVE_SVN_REVISION)
  set(HAVE_SVN_REVISION TRUE CACHE INTERNAL "flag")
  execute_process(COMMAND ${SVN_EXECUTABLE} info ${CMAKE_SOURCE_DIR}
    COMMAND awk "/^Revision:/ { printf $2 }"
    OUTPUT_VARIABLE SVN_REVISION
    ERROR_VARIABLE SVN_REVISION_ERROR)

  if (SVN_REVISION STREQUAL "")
    set(SVN_REVISION_ERROR TRUE)
  endif (SVN_REVISION STREQUAL "")

  if (SVN_REVISION_ERROR OR NOT SVN_EXECUTABLE)
    colormsg(YELLOW "*** Unable to determine svn revision")
    colormsg(YELLOW "*** Are you using a non-English locale?")
    set(SVN_REVISION "0")
  endif (SVN_REVISION_ERROR OR NOT SVN_EXECUTABLE)

  if(NOT USE_SVN_REVISION_FLAGS)
    colormsg(YELLOW "*** Not compiling in svn revision information.  Hope you aren't running simprod")
    set(SVN_REVISION "0")
  endif(NOT USE_SVN_REVISION_FLAGS)

  boost_report_value(SVN_REVISION)
  set(SVN_REVISION ${SVN_REVISION} CACHE INTERNAL "svn revision")

endif(NOT HAVE_SVN_REVISION)

#
# Get SVN_URL from svn info
#
if(NOT HAVE_SVN_URL)
  set(HAVE_SVN_URL TRUE CACHE INTERNAL "flag")
  execute_process(COMMAND ${SVN_EXECUTABLE} info ${CMAKE_SOURCE_DIR}
    COMMAND awk "/^URL:/ { printf $2 }"
    OUTPUT_VARIABLE SVN_URL)

  if(NOT SVN_EXECUTABLE)
    set(SVN_URL "Unknown")
  endif(NOT SVN_EXECUTABLE)

  if (NOT SVN_URL)
    set(SVN_URL "Unknown")
  endif(NOT SVN_URL)
  boost_report_value(SVN_URL)

  set(SVN_URL ${SVN_URL} CACHE INTERNAL "svn url")
endif(NOT HAVE_SVN_URL)

#
#  get META_PROJECT (used by dart) from SVN_URL
#
if(NOT HAVE_META_PROJECT)
  set(HAVE_META_PROJECT TRUE CACHE INTERNAL "flag")
  string(REGEX REPLACE "http://(code.icecube.wisc|icecode.umd|129.2.43.208).edu/svn/meta-projects/" ""
    META_PROJECT ${SVN_URL})
  string(REGEX REPLACE "/" "." META_PROJECT ${META_PROJECT})
  string(STRIP "\n" ${META_PROJECT})
  boost_report_value(META_PROJECT)
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
    set(CMAKE_INSTALL_PREFIX ${META_PROJECT}.r${SVN_REVISION}.${OSTYPE}-${ARCH}.${COMPILER_ID_TAG} CACHE STRING "Install prefix.  Also name of tarball." FORCE)
  else (NOT "${META_PROJECT}" STREQUAL "Unknown")
    set(CMAKE_INSTALL_PREFIX ${OSTYPE}-${ARCH}.${COMPILER_ID_TAG} CACHE STRING "Install prefix.  Also name of tarball." FORCE)
  endif (NOT "${META_PROJECT}" STREQUAL "Unknown")
endif(CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")

set(I3_UBER_HEADER ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/I3.h)
configure_file(${CMAKE_SOURCE_DIR}/cmake/I3.h.in
  ${I3_UBER_HEADER}
  @ONLY)

colormsg("")
colormsg(_HIBLUE_ "Setting compiler, compile drivers, and linker")
colormsg("")

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
      set(CMAKE_C_COMPILE_OBJECT "${CCACHE_PROGRAM} ${CMAKE_C_COMPILE_OBJECT}")
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
if (USE_CCACHE OR CMAKE_COMPILER_IS_CLANG)
  option(USE_GFILT "Use gfilt STL error decryptor" OFF)
else()
  option(USE_GFILT "Use gfilt STL error decryptor" ON)
endif()

if(USE_GFILT)
  set(CMAKE_CXX_COMPILE_OBJECT "${GFILT_PROGRAM} ${CMAKE_CXX_COMPILE_OBJECT}")
  message(STATUS "Using gfilt stl decryptor")
endif(USE_GFILT)

#
# Get compiler information
#

#
#  GCC_VERSION and GCC_NUMERIC_VERSION is e.g. 40302 (for 4.3.2)
#
execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
  COMMAND tr -d \\n
  OUTPUT_VARIABLE GCC_VERSION)
numeric_version(${GCC_VERSION} "gcc")
set(GCC_NUMERIC_VERSION ${GCC_NUMERIC_VERSION} CACHE INTEGER "Numeric gcc version" FORCE)

#
# Ban old gcc versions
#
execute_process(COMMAND "date" "+%s" OUTPUT_VARIABLE NOW)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  if(GCC_NUMERIC_VERSION LESS 40300)
    message("***")
    message("*** You're using a gcc version less than 4.3. This is no longer supported.")
    message("*** Upgrade your complier, or set the CC and CXX environment variables appropriately.")
    message("***")
    if(NOW LESS 1429488000)
      message("*** This will become a fatal error on April 20, 2015.")
      message("***")
      message(WARNING "Unsupported gcc version.")
    else()
      message(FATAL_ERROR "Unsupported gcc version.")
    endif()
  endif()
endif()

#
# Set compiler warning flags.  Unfortunately cmake doesn't do this on its own
#
if(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
  set(CMAKE_COMPILER_IS_INTEL TRUE)
  set(CXX_WARNING_FLAGS "-w1 -Wno-non-virtual-dtor")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_COMPILER_IS_CLANG TRUE)
  set(CXX_WARNING_FLAGS "-Qunused-arguments" "-Wall" "-Wno-non-virtual-dtor")
else()
  set(CXX_WARNING_FLAGS "-Wall -Wno-non-virtual-dtor")
endif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
string(REPLACE ";" " " CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS}")

if(CMAKE_C_COMPILER_ID MATCHES "Intel")
  set(C_WARNING_FLAGS "-w1")
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
  set(C_WARNING_FLAGS "-Qunused-arguments" "-Wall")
else()
  set(C_WARNING_FLAGS "-Wall")
endif(CMAKE_C_COMPILER_ID MATCHES "Intel")
string(REPLACE ";" " " C_WARNING_FLAGS "${C_WARNING_FLAGS}")

#
#  For now, on gcc 4.3.2, add the -Wno-deprecated flag
#  But be wary of -Wno-unused-local-typedefs, which is a gcc 4.8 flag
#
if (GCC_NUMERIC_VERSION GREATER 40799)
  set(CXX_WARNING_SUPRESSION_FLAGS "-Wno-deprecated -Wno-unused-local-typedef"
    CACHE STRING "Warning supression flags for this compiler")
elseif (GCC_NUMERIC_VERSION GREATER 40299)
  set(CXX_WARNING_SUPRESSION_FLAGS "-Wno-deprecated"
    CACHE STRING "Warning supression flags for this compiler")
endif (GCC_NUMERIC_VERSION GREATER 40799)

if (CMAKE_COMPILER_IS_CLANG)
  set(CXX_WARNING_SUPRESSION_FLAGS "-Wno-deprecated -Wno-unused-local-typedef"
    CACHE STRING "Warning supression flags for this compiler")
endif (CMAKE_COMPILER_IS_CLANG)

#
# New versions of clang changed the default template depth
# to 128. This is not enough for boost::python. Change it to
# a somewhat higher value.
#
# This affects clang as shipped by Apple in OSX 10.9-DP3:
# "Apple LLVM version 5.0 (clang-500.1.65) (based on LLVM 3.3svn)"
#
if (CMAKE_COMPILER_IS_CLANG)
  set(CMAKE_CXX_FLAGS "-ftemplate-depth-256")
endif (CMAKE_COMPILER_IS_CLANG)

#
# The following is needed on OSX to enable "system includes" in tools.cmake 
if (CMAKE_COMPILER_IS_CLANG)
  set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
endif (CMAKE_COMPILER_IS_CLANG)

#
# libraries everybody links to
#
if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
	# FreeBSD keeps libdl stuff in libc
	link_libraries(m stdc++)
else (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
	link_libraries(m dl stdc++)
endif (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")

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
# Reduce on RHEL5
#
if(${ETC_ISSUE} MATCHES "Red Hat Enterprise Linux Client release 5.5 (Tikanga)")
  message(STATUS "You're on Red Enterprise Linux 5, an old, buggy, and unsupported platform. Your code will run poorly.")
  set(RELOPTLEVEL 2)
endif(${ETC_ISSUE} MATCHES "Red Hat Enterprise Linux Client release 5.5 (Tikanga)")


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
    if(NOT META_PROJECT MATCHES "trunk$")
      set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel" FORCE)
    else(NOT META_PROJECT MATCHES "trunk$")
      set(CMAKE_BUILD_TYPE "RelWithAssert" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel" FORCE)
    endif(NOT META_PROJECT MATCHES "trunk$")
  endif (NOT CMAKE_BUILD_TYPE)

  set(CMAKE_CXX_FLAGS "${CXX_WARNING_FLAGS} ${CMAKE_CXX_FLAGS} ${CXX_WARNING_SUPRESSION_FLAGS}")
  string(REGEX REPLACE "[ ]+" " " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING
    "Flags used by the compiler during all build types" FORCE)

  set(CMAKE_C_FLAGS "${C_WARNING_FLAGS} ${CMAKE_C_FLAGS}")
  string(REPLACE "-Wno-non-virtual-dtor" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING
    "Flags used by the compiler during all build types" FORCE)

  set(CMAKE_CXX_FLAGS_RELEASE "-O${RELOPTLEVEL} -Wno-unused-variable -DNDEBUG -DI3_COMPILE_OUT_VERBOSE_LOGGING")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING
    "Flags used by compiler during release builds" FORCE)

  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O${RELOPTLEVEL} -Wno-unused-variable -g -DNDEBUG -DI3_COMPILE_OUT_VERBOSE_LOGGING")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE STRING
    "Flags used by compiler during release builds" FORCE)

  ## RelWithAssert flags
  set(CMAKE_CXX_FLAGS_RELWITHASSERT "-O${RELOPTLEVEL} -Wno-unused-variable -DI3_COMPILE_OUT_VERBOSE_LOGGING" CACHE STRING
    "Flags used by compiler during Release+Assert builds" FORCE)
  set(CMAKE_C_FLAGS_RELLWITHASSERT "-O${RELOPTLEVEL} -Wno-unused-variable" CACHE STRING
    "Flags used by compiler during Release+Assert builds" FORCE)
  set(CMAKE_EXE_LINKER_FLAGS_RELWITHASSERT "${CMAKE_EXE_LINKER_FLAGS_RELEASE}" CACHE STRING
    "Flags used for linking binaries during Release+Assert builds." FORCE)
  set(CMAKE_SHARED_LINKER_FLAGS_RELWITHASSERT "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}" CACHE STRING
    "Flags used for linking shared libraries during Release+Assert builds." FORCE)
  mark_as_advanced(
    CMAKE_CXX_FLAGS_RELWITHASSERT
    CMAKE_C_FLAGS_RELWITHASSERT
    CMAKE_EXE_LINKER_FLAGS_RELWITHASSERT
    CMAKE_SHARED_LINKER_FLAGS_RELWITHASSERT )
  # Update the documentation string of CMAKE_BUILD_TYPE for GUIs
  set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel RelWithAssert."
    FORCE )

  ## coverage flags
  set(CMAKE_CXX_FLAGS_COVERAGE "-pipe -g -O0 -fprofile-arcs -ftest-coverage" CACHE STRING
    "Flags used by the C++ compiler during coverage builds."
    FORCE )
  set(CMAKE_C_FLAGS_COVERAGE "-pipe -g -O0 -fprofile-arcs -ftest-coverage" CACHE STRING
    "Flags used by the C compiler during coverage builds."
    FORCE )
  set(CMAKE_EXE_LINKER_FLAGS_COVERAGE
    "-pipe -g -O0 -fprofile-arcs -ftest-coverage" CACHE STRING
    "Flags used for linking binaries during coverage builds."
    FORCE )
  set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE
    "-pipe -g -O0 -fprofile-arcs -ftest-coverage" CACHE STRING
    "Flags used by the shared libraries linker during coverage builds."
    FORCE )
  mark_as_advanced(
    CMAKE_CXX_FLAGS_COVERAGE
    CMAKE_C_FLAGS_COVERAGE
    CMAKE_EXE_LINKER_FLAGS_COVERAGE
    CMAKE_SHARED_LINKER_FLAGS_COVERAGE )
  # Update the documentation string of CMAKE_BUILD_TYPE for GUIs
  set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel RelWithAssert Coverage."
    FORCE )

  ## at this point only project CMakeLists.txt's and tools/*.cmake will modify
  ## the command line. let's check what we have.
  option(CHECK_FLAGS "Check *some* of the flags passed to the compilers" OFF)
  if(CHECK_FLAGS)
    include(CheckCCompilerFlag)
    include(CheckCXXCompilerFlag)

    ## check C flags
    set(l ${CMAKE_C_FLAGS})
    string(TOUPPER ${CMAKE_BUILD_TYPE} l2)
    set(l2 ${CMAKE_C_FLAGS_${l2}})
    separate_arguments(l)
    separate_arguments(l2)
    message(STATUS "Checking C compiler flags for ${CMAKE_C_COMPILER}")
    foreach(f "-bar" ${l} ${l2})
      check_c_compiler_flag("${f}" "HAS_FLAG ${f}")
      if(NOT "HAS_FLAG ${f}")
	colormsg(HIRED  "*** Your compiler '${CMAKE_C_COMPILER}' doesn't like the flag '${f}'.")
	colormsg(HIRED  "*** This is a bug. Please file a ticket at http://code.icecube.wisc.edu/projects/icecube/newticket")
	message(WARNING "*** Unknown compiler flag, '${f}'.")
      endif()
    endforeach()
    message(STATUS "")

    ## check CXX flags
    set(l ${CMAKE_CXX_FLAGS})
    string(TOUPPER ${CMAKE_BUILD_TYPE} l2)
    set(l2 ${CMAKE_CXX_FLAGS_${l2}})
    separate_arguments(l)
    separate_arguments(l2)
    message(STATUS "Checking CXX compiler flags for ${CMAKE_CXX_COMPILER}")
    foreach(f "-foo" ${l} ${l2})
      check_CXX_compiler_flag("${f}" "HAS_FLAG ${f}")
      if(NOT "HAS_FLAG ${f}")
	colormsg(HIRED  "*** Your compiler '${CMAKE_CXX_COMPILER}' doesn't like the flag '${f}'.")
	colormsg(HIRED  "*** This is a bug. Please file a ticket at http://code.icecube.wisc.edu/projects/icecube/newticket")
	message(WARNING "*** Unknown compiler flag, '${f}'.")
      endif()
    endforeach()
  endif()

  #
  # stop binutils stupidity
  #
  if(NOT APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-as-needed" CACHE STRING "Flags used by the linker" FORCE)
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--no-as-needed" CACHE STRING "Flags used by the linker" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed" CACHE STRING "Flags used by the linker" FORCE)
  endif(NOT APPLE)

  set(METAPROJECT_CONFIGURED TRUE CACHE INTERNAL "Metaproject configured")
endif(NOT METAPROJECT_CONFIGURED)
