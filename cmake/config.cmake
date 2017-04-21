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

## set some cmake flags - see cmake_variables(7)
set(BUILD_SHARED_LIBS SHARED)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
## this is our flag, but in the above style
set(DOXYGEN_OUTPUT_PATH ${CMAKE_BINARY_DIR}/docs/doxygen)

#
#  Check build sanity
#
## ensure that we're not cmake'ing under an env-shell from another workspace
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

## default the option SYSTEM_PACKAGES to ON
option(SYSTEM_PACKAGES "Use tools provided by the operating system" ON)
## import I3_PORTS from the environment into the cmake variable I3_PORTS
set(I3_PORTS $ENV{I3_PORTS} CACHE STRING "Path to your icecube ports installation")

#
# Find CVMFS software
# (designed for py2-v2 port-less software)
#
if($ENV{SROOT} MATCHES "^/cvmfs/icecube")
  if($ENV{SROOTBASE} MATCHES "py2-v2$")
    set(SYSTEM_PACKAGES TRUE)
  endif()
  set(CMAKE_PREFIX_PATH $ENV{SROOT})
  set(USE_CVMFS TRUE CACHE BOOL "Are we using CVMFS?")
  set(CVMFS_SROOTBASE "$ENV{SROOTBASE}" CACHE STRING "CVMFS toolset path" )
endif()

## use a default if I3_PORTS isn't set, and report its value
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
STRING(REGEX REPLACE "^.*(g(cc|[+][+])|clang|Apple LLVM)[ -][Vv]ers(ion|ión|io|ão) ([^ ]+) .*"
                     "\\1-\\4" COMPILER_ID_TAG ${COMPILER_ID_TAG})

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
find_program(SVN_EXECUTABLE svn
             PATHS ENV PATH /usr/bin
             NO_DEFAULT_PATH)
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

## set the uber header. this file is included via command line for
## every compiled file.
set(I3_UBER_HEADER ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/I3.h)
configure_file(${CMAKE_SOURCE_DIR}/cmake/I3.h.in
  ${I3_UBER_HEADER}
  @ONLY)

colormsg("")
colormsg(_HIBLUE_ "Setting compiler, compile drivers, and linker")
colormsg("")

#
#  distcc - distributed compilation
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
#  ccache - local object file caching. can speed compilation times
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
#  gfilt - filter STL error messages into something understandable
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
# include-what-you-use
#
if(USE_IWYU AND (CMAKE_BUILD_TYPE MATCHES "Debug"))
    find_program(IWYU_PROGRAM NAMES "iwyu" "include-what-you-use" HINTS ENV PATH)
    if(IWYU_PROGRAM)
        message(STATUS "iwyu found at ${IWYU_PROGRAM}")
    else()
        message(STATUS "iwyu not found.")
        set(USE_IWYU OFF)
    endif()
else()
set(USE_IWYU OFF)
endif()

#
# Get compiler information
#

#
#  GCC_VERSION and GCC_NUMERIC_VERSION is e.g. 40302 (for 4.3.2)
#
# execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
#   COMMAND tr -d \\n
#   OUTPUT_VARIABLE GCC_VERSION)
# numeric_version(${GCC_VERSION} "gcc")
# set(GCC_NUMERIC_VERSION ${GCC_NUMERIC_VERSION} CACHE INTEGER "Numeric gcc version")


#
# Ban old gcc versions
#
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  numeric_version(${CMAKE_CXX_COMPILER_VERSION} "gcc")
  if(GCC_NUMERIC_VERSION LESS 40801)
    message("***")
    message("*** You're using a gcc version less than 4.8.1. This is no longer supported.")
    message("*** Upgrade your complier, or set the CC and CXX environment variables appropriately.")
    if(HOSTNAME MATCHES "cobalt")
      message("***")
      message("*** cobalt users can use py2_v2 and \"scl enable devtoolset-2 bash\" and rebuilding")
      message("*** If you need further help, ask in \#software on Slack")
    endif()
    deprecation_warning(20160905 "Unsupported gcc version.")
  endif()
endif()

#
# Ban old clang versions. This will probably break for non-Apple clang
# on Apple. :(
#
if((CMAKE_CXX_COMPILER_ID MATCHES "AppleClang") OR
    (APPLE AND (CMAKE_CXX_COMPILER_ID MATCHES "Clang") AND (CMAKE_CXX_COMPILER MATCHES "/Applications/Xcode")))
  numeric_version(${CMAKE_CXX_COMPILER_VERSION} "clang")
  if(CLANG_NUMERIC_VERSION LESS 60000)
    message("***")
    message("*** You're using an Xcode version less than 6. This is no longer supported.")
    message("*** Upgrade your copy of Xcode, or set the CC and CXX environment variables appropriately.")
    deprecation_warning(20160905 "Unsupported Xcode version.")
  endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  numeric_version(${CMAKE_CXX_COMPILER_VERSION} "clang")
  if(CLANG_NUMERIC_VERSION LESS 30400)
    message("***")
    message("*** You're using a clang version less than 3.4. This is no longer supported.")
    message("*** Upgrade your complier, or set the CC and CXX environment variables appropriately.")
    deprecation_warning(20160905 "Unsupported clang version.")
  endif()
endif()

#
# Ban old Intel icc versions
#
if(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
  numeric_version(${CMAKE_CXX_COMPILER_VERSION} "intel")
  if(INTEL_NUMERIC_VERSION LESS 150000)
    message("***")
    message("*** You're using an icc version less than 15. This is no longer supported.")
    message("*** Upgrade your complier, or set the CC and CXX environment variables appropriately.")
    deprecation_warning(20160905 "Unsupported icc version.")
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
#  Set warning suppression flags for our compilers
#  This should work for gcc/clang/intel. Note: we test for the warning, and
#  assume it hass a corresponding '-Wno-*' flag.
#
include(CheckCXXCompilerFlag)
if (NOT CXX_WARNING_SUPPRESSION_FLAGS)
    foreach(f -Wdeprecated -Wunused-variable -Wunused-local-typedef -Wunused-local-typedefs -Wpotentially-evaluated-expression)
    string(REPLACE "-" "_" FLAG ${f})
    check_cxx_compiler_flag(${f} CXX_HAS${FLAG})
    if (CXX_HAS${FLAG})
      string(REGEX REPLACE "^-W" "-Wno-" f ${f})
      list(APPEND CXX_WARNING_SUPPRESSION_FLAGS ${f})
    endif()
  endforeach()
  string(REPLACE ";" " " CXX_WARNING_SUPPRESSION_FLAGS "${CXX_WARNING_SUPPRESSION_FLAGS}")
  set(CXX_WARNING_SUPPRESSION_FLAGS ${CXX_WARNING_SUPPRESSION_FLAGS}
    CACHE STRING "Warning suppression flags for this compiler")
endif()

#
# New versions of clang changed the default template depth
# to 128. This is not enough for boost::python. Change it to
# a somewhat higher value.
#
# This affects clang as shipped by Apple in OSX 10.9-DP3:
# "Apple LLVM version 5.0 (clang-500.1.65) (based on LLVM 3.3svn)"
#
if (CMAKE_COMPILER_IS_CLANG)
  set(CMAKE_CXX_FLAGS "-ftemplate-depth-256 -fcolor-diagnostics ${CMAKE_CXX_FLAGS}")
endif (CMAKE_COMPILER_IS_CLANG)

#
# The following is needed on OSX to enable "system includes" in tools.cmake 
if (CMAKE_COMPILER_IS_CLANG)
  set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
endif (CMAKE_COMPILER_IS_CLANG)

#
# Set the compiler flag for C++14
#
if(CMAKE_COMPILER_IS_CLANG AND
    (CLANG_NUMERIC_VERSION LESS "30500"))
  set(CXX14_FLAG "-std=c++1y")
else()
  set(CXX14_FLAG "-std=c++14")
endif()
set(CXX14_FLAG "-std=c++11")

#
# libraries everybody links to
#
if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
	# FreeBSD keeps libdl stuff in libc
	link_libraries(m stdc++)
else (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
	link_libraries(m dl) # stdc++)
endif (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")

#
#  Detect certain old platforms and reduce optimization levels accordingly
#
set(RELOPTLEVEL 3)

#
#  Ban any Fedora Core release
#
if(EXISTS /etc/issue)
  file(READ /etc/issue ETC_ISSUE)
  if(${ETC_ISSUE} MATCHES "Fedora Core")
    message(FATAL_ERROR "You're running a Fedora Core release, a known old and buggy platform. Upgrade your OS.")
  endif()
endif()

#
# Ban RHEL5. Should also ban other RH5 OSes (SL, CentOS, etc)
#
if(${ETC_ISSUE} MATCHES "Red Hat Enterprise Linux Client release 5.5 (Tikanga)")
  message(FATAL_ERROR "You're on Red Enterprise Linux 5, an old, buggy, and unsupported platform. Upgrade your OS.")
endif()

message(STATUS "Setting default compiler flags and build type.")

#
#  Check if it is defined...   if somebody has specified it on the
#  cmake commmand line, e.g.
#  cmake -DCMAKE_BUILD_TYPE=Debug, we need to use that value
#
if ((NOT CMAKE_BUILD_TYPE) OR (CMAKE_BUILD_TYPE MATCHES "^None"))
  if(META_PROJECT MATCHES "trunk$")
    set(CMAKE_BUILD_TYPE "RelWithAssert")
  else()
    set(CMAKE_BUILD_TYPE "Release")
  endif()
endif()
set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel" FORCE)

## turning this flag on enables outputting a JSON file that is used
## for clang's libtooling. cache/force is needed
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export a JSON 'database' of compilation commands for each source file. Useful for clang's libtooling" FORCE)

#
# set flags common to all build types
#
set(CMAKE_CXX_FLAGS "-pipe ${CXX14_FLAG} ${CXX_WARNING_FLAGS} ${CMAKE_CXX_FLAGS} ${CXX_WARNING_SUPPRESSION_FLAGS}")
string(REGEX REPLACE "[ ]+" " " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING
  "Flags used by the compiler during all build types")

set(CMAKE_C_FLAGS "-pipe -std=gnu89 ${C_WARNING_FLAGS} ${CMAKE_C_FLAGS}")
string(REPLACE "-Wno-non-virtual-dtor" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING
  "Flags used by the compiler during all build types")

## set RELEASE flags
set(CMAKE_CXX_FLAGS_RELEASE "-O${RELOPTLEVEL} -DNDEBUG -DI3_COMPILE_OUT_VERBOSE_LOGGING")
set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING
  "Flags used by compiler during release builds")

## set Release With Debug Info flags
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O${RELOPTLEVEL} -g -DNDEBUG -DI3_COMPILE_OUT_VERBOSE_LOGGING")
set(CMAKE_C_FLAGS_RELWITHDEBINFO   "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE STRING
  "Flags used by compiler during release builds")

## RelWithAssert flags
set(CMAKE_CXX_FLAGS_RELWITHASSERT "-O${RELOPTLEVEL} -DI3_COMPILE_OUT_VERBOSE_LOGGING" CACHE STRING
  "Flags used by compiler during Release+Assert builds")
set(CMAKE_C_FLAGS_RELLWITHASSERT "-O${RELOPTLEVEL}" CACHE STRING
  "Flags used by compiler during Release+Assert builds")
set(CMAKE_EXE_LINKER_FLAGS_RELWITHASSERT "${CMAKE_EXE_LINKER_FLAGS_RELEASE}" CACHE STRING
  "Flags used for linking binaries during Release+Assert builds.")
set(CMAKE_SHARED_LINKER_FLAGS_RELWITHASSERT "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}" CACHE STRING
  "Flags used for linking shared libraries during Release+Assert builds.")
mark_as_advanced(
  CMAKE_CXX_FLAGS_RELWITHASSERT
  CMAKE_C_FLAGS_RELWITHASSERT
  CMAKE_EXE_LINKER_FLAGS_RELWITHASSERT
  CMAKE_SHARED_LINKER_FLAGS_RELWITHASSERT)
# Update the documentation string of CMAKE_BUILD_TYPE for GUIs
set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
  "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel RelWithAssert." FORCE)

## coverage flags
set(CMAKE_CXX_FLAGS_COVERAGE "-g -O0 -fprofile-arcs -ftest-coverage" CACHE STRING
  "Flags used by the C++ compiler during coverage builds.")
set(CMAKE_C_FLAGS_COVERAGE "-g -O0 -fprofile-arcs -ftest-coverage" CACHE STRING
  "Flags used by the C compiler during coverage builds.")
set(CMAKE_EXE_LINKER_FLAGS_COVERAGE
  "-g -O0 -fprofile-arcs -ftest-coverage" CACHE STRING
  "Flags used for linking binaries during coverage builds.")
set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE
  "-g -O0 -fprofile-arcs -ftest-coverage" CACHE STRING
  "Flags used by the shared libraries linker during coverage builds.")
mark_as_advanced(
  CMAKE_CXX_FLAGS_COVERAGE
  CMAKE_C_FLAGS_COVERAGE
  CMAKE_EXE_LINKER_FLAGS_COVERAGE
  CMAKE_SHARED_LINKER_FLAGS_COVERAGE)
# Update the documentation string of CMAKE_BUILD_TYPE for GUIs
set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
  "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel RelWithAssert Coverage." FORCE)

## optimization remarks
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  if(NOT REMARK_TYPE)
    set(REMARK_TYPE ".*")
  endif()
set(CMAKE_CXX_FLAGS_REMARKS "${CMAKE_CXX_FLAGS_RELEASE} -gline-tables-only -gcolumn-info -Rpass='${REMARK_TYPE}' -Rpass-missed='${REMARK_TYPE}' -Rpass-analysis='${REMARK_TYPE}'" CACHE STRING
  "Flags used by the C compiler to output remarks about how it's optimizing")
set(CMAKE_C_FLAGS_REMARKS ${CMAKE_CXX_FLAGS_REMARKS} CACHE STRING
  "Flags used by the C++ compiler to output remarks about how it's optimizing")
mark_as_advanced(
  CMAKE_CXX_FLAGS_REMARKS
  CMAKE_C_FLAGS_REMARKS)
# Update the documentation string of CMAKE_BUILD_TYPE for GUIs
set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
  "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel RelWithAssert Coverage Remarks." FORCE)
else()
  message(FATAL_ERROR "Sorry, the 'Remarks' build-type is Clang specific.")
endif()

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
  foreach(f ${l} ${l2})
    string(REPLACE "-" "_" FLAG ${f})
    check_c_compiler_flag("${f}" "HAS_FLAG${FLAG}")
    if(NOT ${HAS_FLAG${FLAG}})
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
  foreach(f ${l} ${l2})
    string(REPLACE "-" "_" FLAG ${f})
    check_cxx_compiler_flag("${f}" "HAS_FLAG${FLAG}")
    if(NOT ${HAS_FLAG${FLAG}})
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
