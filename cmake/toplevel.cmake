#  $Id$
#
#  Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
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
cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(icetray)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
***************************************************************************
***      In place build is a bad idea. Please build out-of-place.       ***
***   Run cmake /path/to/source from a directory != /path/to/source.    ***
***                                                                     ***
*** DON'T FORGET TO REMOVE THE CMakeCache.txt IN YOUR SOURCE DIRECTORY  ***
***                  THAT WAS JUST CREATED.                             ***
***************************************************************************
")
endif("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

#
# set cmake policies. policy definitions can be found at
# https://cmake.org/cmake/help/latest/manual/cmake-policies.7.html
# or `man cmake-policies`
#
if(POLICY CMP0002)
    cmake_policy(SET CMP0002 NEW)
endif()
if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()
if(POLICY CMP0046)
    cmake_policy(SET CMP0046 NEW)
endif()
if(POLICY CMP0053)
    cmake_policy(SET CMP0053 NEW)
endif()
if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif()
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()
if(POLICY CMP0094)
    cmake_policy(SET CMP0094 NEW)
endif()

#
# search for header, libraries and frameworks inside of the system
# SDK directory on OS X.
#
if (APPLE)
  list(APPEND CMAKE_PREFIX_PATH ${CMAKE_OSX_SYSROOT}/usr)
  set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} CACHE PATH "")

  list(APPEND CMAKE_FRAMEWORK_PATH ${CMAKE_OSX_SYSROOT}/System/Library/Frameworks)
  set(CMAKE_FRAMEWORK_PATH ${CMAKE_FRAMEWORK_PATH} CACHE PATH "")
endif (APPLE)

## set some convenience variables
set(I3_SRC ${CMAKE_SOURCE_DIR})
set(I3_BUILD ${CMAKE_BINARY_DIR})

execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/bin)

## pull in optional meta-project CMakeLists
include(${CMAKE_SOURCE_DIR}/CMakeLists.optional.txt OPTIONAL)

## expand the module path to include our own directories
set(CMAKE_MODULE_PATH
  ${EXTRA_CMAKE_MODULE_PATH}
  ${CMAKE_SOURCE_DIR}/cmake
  ${CMAKE_SOURCE_DIR}/cmake/tools
  ${CMAKE_SOURCE_DIR}/cmake/utility)

include(utility)  # load utility functions (pretty print, etc)
include(config)   # trigger the configuation meat (build types, etc)

### ctest testing
## enable_testing() must be called before add_test() which happens in project.cmake
include(testing_and_data)  # setup data locations and rsync targets
enable_testing()

include(tools)          # trigger tool/library detection
include(project)        # load the i3_*() macros

## this must be set before any call to i3_test_executable() - see "meat" below
add_custom_target(test-bins)
## this must be set before any call to i3_add_pybindings() - see "meat" below
add_custom_target(pybindings)

## these doxygen settings need to happen before configuring projects
set(INSPECT_ALL_HTML ${CMAKE_BINARY_DIR}/doxygen/inspect/index.html)
set(SPHINX_DIR "${CMAKE_BINARY_DIR}/sphinx_src")
add_custom_target(doxygen)

## manipulate some directories before configuring projects
file(MAKE_DIRECTORY "${DOXYGEN_OUTPUT_PATH}/.tagfiles")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/docs/inspect")
file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/inspect")

## look for xlstproc, required by icetray-inspect
find_program(XSLTPROC_BIN xsltproc DOC "Location of the XSLT processor")

add_custom_target(inspect
  COMMAND ${CMAKE_BINARY_DIR}/env-shell.sh
          ${EXECUTABLE_OUTPUT_PATH}/icetray-inspect
          --sphinx --sphinx-references
          --all --no-params
          --title=\"IceTracy Quick Reference\"
          -o ${SPHINX_DIR}/source/icetray_quick_reference.rst
  COMMENT "Generating rst from icetray-inspect of QuickReference"
  DEPENDS ${CMAKE_BINARY_DIR}/bin/icetray-inspect
  )

## generate a URL and target to deploy docs to
string(REGEX REPLACE "s\\.V.*$" "" DEST ${META_PROJECT})
string(REGEX REPLACE "-software" "" DEST ${DEST})
string(REGEX REPLACE "\\." "_" DEST ${DEST})
string(REGEX REPLACE "_release$" "" DEST ${DEST})
add_custom_target(deploy-docs
  COMMAND rsync -va --delete ${CMAKE_BINARY_DIR}/docs/ buildmaster@dragon:/opt/docs/${DEST}/
  COMMENT Deploying docs to ${DEST}
  )

include(tarball)

#
#  Environment checking targets
#
configure_file(${CMAKE_SOURCE_DIR}/cmake/env-check.sh.in
  ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/env-check.sh
  @ONLY)

configure_file(${CMAKE_SOURCE_DIR}/cmake/tarball-env-shell.sh.in
  ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/tarball-env-shell.sh
  @ONLY)

install(PROGRAMS ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/tarball-env-shell.sh
  DESTINATION .
  RENAME env-shell.sh
  )

add_custom_target(env-check ALL
  COMMAND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/env-check.sh
  COMMENT "Checking build against environment")

## this is the meat, where individual projects get configured
colormsg("")
colormsg(_HIBLUE_ "Configuring projects:")
colormsg("")

set(I3_PROJECTS "" CACHE STRING "List of projects to build (if empty, glob for CMakeLists.txt and use all")

if (NOT I3_PROJECTS)
  #
  #  Glob together a list of subdirectories containing a CMakeLists.txt
  #
  file(GLOB cmake_projects RELATIVE ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/*/CMakeLists.txt)
  file(GLOB hidden_projects RELATIVE ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/.*/CMakeLists.txt)

  if(hidden_projects)
    list(REMOVE_ITEM cmake_projects ${hidden_projects})
  endif(hidden_projects)

  foreach(d ${cmake_projects})
    get_filename_component(proj ${d} PATH)
    set(SUBDIRS ${SUBDIRS} ${proj})
  endforeach(d ${cmake_projects})
else()
  message(STATUS "Using project list manually specified by I3_PROJECTS")
  set(SUBDIRS ${I3_PROJECTS})
endif()

list(SORT SUBDIRS)
foreach(subdir ${SUBDIRS})
  get_filename_component(pname ${subdir} NAME_WE)
  set(I3_PROJECTS "${I3_PROJECTS}" ${pname})
  message(STATUS "+ ${pname}")
  #
  # setup tagfiles for doxygen
  #
  file(WRITE ${DOXYGEN_OUTPUT_PATH}/.tagfiles/${pname}.tag "")
  file(WRITE ${DOXYGEN_OUTPUT_PATH}/.tagfiles/${pname}.include "# autogenerated doxy include file\n")
  add_subdirectory(${CMAKE_SOURCE_DIR}/${pname})
  file(WRITE  ${DOXYGEN_OUTPUT_PATH}/.tagfiles/${pname}.include.tmp "# autogenerated doxy include file\n")
  file(APPEND ${DOXYGEN_OUTPUT_PATH}/.tagfiles/${pname}.include.tmp "TAGFILES = ")
  execute_process (COMMAND "cat" "${DOXYGEN_OUTPUT_PATH}/.tagfiles/${pname}.include"
                   COMMAND "sort" "-u"
                   COMMAND "grep" "-v" "${pname}"
                   COMMAND "grep" "-v" "^#"
                   COMMAND "xargs"
                   OUTPUT_VARIABLE foo)
  file(APPEND ${DOXYGEN_OUTPUT_PATH}/.tagfiles/${pname}.include.tmp "${foo}")
  file(RENAME "${DOXYGEN_OUTPUT_PATH}/.tagfiles/${pname}.include.tmp" "${DOXYGEN_OUTPUT_PATH}/.tagfiles/${pname}.include")
  exec_program(test ARGS -h ${CMAKE_BINARY_DIR}/${pname}/resources -a -e ${CMAKE_BINARY_DIR}/${pname}/resources || ln -snf ${CMAKE_SOURCE_DIR}/${pname}/resources ${CMAKE_BINARY_DIR}/${pname}/resources
    OUTPUT_VARIABLE DEV_NULL)
endforeach(subdir ${SUBDIRS})

## documentation targets
## Documentation is now built with docs-build

#
# env-shell.sh
#
foreach (file env-shell.sh)
  message(STATUS "Generating ${file}")
  configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/${file}.in
    ${CMAKE_CURRENT_BINARY_DIR}/${file}
    @ONLY
    )
  execute_process(COMMAND chmod 755 ${CMAKE_BINARY_DIR}/${file})
  execute_process(COMMAND cp ${CMAKE_BINARY_DIR}/${file} ${NOTES_DIR})
endforeach (file env-shell.sh)

#
# icetray-config
#
message(STATUS "Generating icetray-config")
configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/icetray-config.in
  ${CMAKE_CURRENT_BINARY_DIR}/bin/icetray-config
  @ONLY
  )
execute_process(COMMAND chmod 755 ${CMAKE_BINARY_DIR}/bin/icetray-config)
execute_process(COMMAND cp ${CMAKE_BINARY_DIR}/bin/icetray-config ${NOTES_DIR})
exec_program(${CMAKE_BINARY_DIR}/env-shell.sh ARGS /usr/bin/env > ${NOTES_DIR}/env-post_shell.txt OUTPUT_VARIABLE DEV_NULL)

#
#  dpkg configuration
#
if(DPKG_INSTALL_PREFIX)
  install(DIRECTORY ${DOXYGEN_OUTPUT_PATH} DESTINATION ${DPKG_INSTALL_PREFIX})

  set(CMAKE_INSTALL_PREFIX ${DPKG_INSTALL_PREFIX}
    CACHE PATH "Installation path for dpkgable install")
  execute_process(COMMAND sed -e s/\@CMAKE_BINARY_DIR\@/\@CMAKE_INSTALL_PREFIX\@/g\;s/\@CMAKE_SOURCE_DIR\@/\@CMAKE_INSTALL_PREFIX\@/g
    INPUT_FILE ${CMAKE_SOURCE_DIR}/cmake/env-shell.sh.in
    OUTPUT_FILE ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dpkg-env-shell.sh.in)
  configure_file(
    ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dpkg-env-shell.sh.in
    ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dpkg-env-shell.sh
    @ONLY
    )
  add_custom_target(dpkg-stage
    COMMAND ${CMAKE_MAKE_PROGRAM} doxygen
    COMMAND ${CMAKE_MAKE_PROGRAM} install
    COMMAND cp ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dpkg-env-shell.sh ${CMAKE_INSTALL_PREFIX}/env-shell.sh
    COMMAND chmod 755 ${CMAKE_INSTALL_PREFIX}/env-shell.sh
    )
endif(DPKG_INSTALL_PREFIX)

## coverage target
## there is an incompatibility between gcc-5.4/cmake-3.5/lcov-1.12 and gcc-4.8/cmake-2.8/lcov-1.10
## the target below may not work on combinations of older software. see http://code.icecube.wisc.edu/projects/icecube/ticket/1891 for more info
add_custom_target(coverage
  COMMAND if test ! -d ../output \; then mkdir ../output\; fi
  COMMAND rm -f ${CMAKE_BINARY_DIR}/CMakeFiles/*.moc ${CMAKE_BINARY_DIR}/CMakeFiles/*.h
  COMMAND ln -s ${CMAKE_BINARY_DIR}/steamshovel/*.moc ${CMAKE_BINARY_DIR}/CMakeFiles/
  COMMAND ln -s ${CMAKE_BINARY_DIR}/steamshovel/*.h   ${CMAKE_BINARY_DIR}/CMakeFiles/
  COMMAND lcov -b CMakeFiles/ -d . -z
  COMMAND lcov -b CMakeFiles/ -d . -c -i -o test_base.info
  COMMAND ./env-shell.sh ctest || true
  COMMAND lcov -b CMakeFiles/ -d . -c -o test_run.info
  COMMAND lcov -b CMakeFiles/ -d . -a test_base.info -a test_run.info -o test_total.info
  COMMAND lcov -o reports.info -r test_total.info '*/private/test/*' '/usr/include/*' '/usr/local/*' '/cvmfs/*' '*/numpy' '/usr/lib/gcc/*' ${p} '${CMAKE_BINARY_DIR}/CMakeFiles/' '${CMAKE_BINARY_DIR}/steamshovel/*'
  COMMAND genhtml --ignore-errors source --legend -o ../output/`date +%Y-%m-%d` reports.info
  COMMAND rm -f ../output/00_LATEST \; ln -sf `ls -1tr ../output |tail -1` ../output/00_LATEST
  COMMAND rm -f ${CMAKE_BINARY_DIR}/CMakeFiles/*.moc ${CMAKE_BINARY_DIR}/CMakeFiles/.h
)

## python coverage
configure_file(${CMAKE_SOURCE_DIR}/cmake/pycoverage.in
  ${CMAKE_CURRENT_BINARY_DIR}/bin/pycoverage
  @ONLY)
add_custom_target(pycoverage
  COMMAND ./env-shell.sh pycoverage -d pycoverage/`date +%Y-%m-%d`
  COMMAND rm -f pycoverage/00_LATEST \; ln -sf `ls -1tr pycoverage |tail -1` pycoverage/00_LATEST
)
add_custom_target(pycoverage_verbose
  COMMAND ./env-shell.sh pycoverage -d pycoverage/`date +%Y-%m-%d` -v
  COMMAND rm -f pycoverage/00_LATEST \; ln -sf `ls -1tr pycoverage |tail -1` pycoverage/00_LATEST
)
