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
cmake_minimum_required(VERSION 2.8.5 FATAL_ERROR)
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

## set cmake policies
if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()
if(POLICY CMP0046)
    cmake_policy(SET CMP0046 OLD)
endif()
if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
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

set(I3_SRC ${CMAKE_SOURCE_DIR})
set(I3_BUILD ${CMAKE_BINARY_DIR})

include(${CMAKE_SOURCE_DIR}/CMakeLists.optional.txt OPTIONAL)

set(CMAKE_MODULE_PATH
  ${EXTRA_CMAKE_MODULE_PATH} 
  ${CMAKE_SOURCE_DIR}/cmake 
  ${CMAKE_SOURCE_DIR}/cmake/tools 
  ${CMAKE_SOURCE_DIR}/cmake/utility)

include(utility)
include(config)

## enable_testing() must be called before add_test() which happens in project.cmake
if(DEFINED ENV{I3_TESTDATA})
    set(I3_TESTDATA $ENV{I3_TESTDATA} CACHE STRING "Path to your icetray test-data")
elseif(DEFINED ENV{I3_PORTS})
    set(I3_TESTDATA $ENV{I3_PORTS}/test-data CACHE STRING "Path to your icetray test-data")
else()
    colormsg(YELLOW "*** Neither I3_PORTS nor I3_TESTDATA set.")
    colormsg(YELLOW "*** Make sure to define at least one of them before calling 'make test' or manually running tests.")
    set(I3_TESTDATA "" CACHE STRING "Path to your icetray test-data: currently empty, define it if you wish to run unit tests and/or test scripts.")
endif(DEFINED ENV{I3_TESTDATA})

if(I3_TESTDATA)
  add_custom_target(rsync
    COMMAND test -n "${I3_TESTDATA}"
    COMMAND mkdir -p "${I3_TESTDATA}"
    COMMAND rsync -vrlpt --delete code.icecube.wisc.edu::Offline/test-data/ ${I3_TESTDATA}/
    COMMENT "Rsyncing test-data to I3_TESTDATA"
    )
  ### ctest testing
  enable_testing()
else()
  add_custom_target(rsync
    COMMENT "I3_TESTDATA is not set.  Set it, 'make rebuild_cache' and try again.")
  add_custom_target(test
    COMMENT "I3_TESTDATA is not set.  Set it, 'make rebuild_cache' and try again.")
endif()

file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/inspect)

include(tools)
include(system_report)
include(project)

add_custom_target(test-bins)
add_custom_target(pybindings)

## these doxygen settings need to happen before configuring projects
set(INSPECT_ALL_HTML ${CMAKE_BINARY_DIR}/doxygen/inspect/index.html)
set(SPHINX_DIR "${CMAKE_BINARY_DIR}/sphinx_src")
add_custom_target(doxygen)

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

#
# Tarball target
#
find_program(MD5SUM_PROGRAM md5sum)
find_program(MD5SUM_PROGRAM md5)
if(MD5SUM_PROGRAM)
  set(MD5SUM_TARBALL_COMMAND ${MD5SUM_PROGRAM} ${CMAKE_INSTALL_PREFIX}.tar.gz > ${CMAKE_INSTALL_PREFIX}.md5sum)
else(MD5SUM_PROGRAM)
  set(MD5SUM_TARBALL_COMMAND /bin/echo Skipping md5sum, as md5sum command was not found.)
endif(MD5SUM_PROGRAM)

add_custom_target(tarball-start
  COMMAND /bin/echo Building installation directory ${CMAKE_INSTALL_PREFIX}...
  COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/etc
  COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/lib
  COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/bin
  COMMENT "Tarball start"
  )  

add_custom_target(tarball-install
  COMMAND ${CMAKE_MAKE_PROGRAM} install
  COMMENT "Tarball install"
  )
add_dependencies(tarball-install tarball-start)
add_custom_target(tarball-install-tools
  COMMAND ${CMAKE_MAKE_PROGRAM} install_tool_libs
  COMMAND ${MAKE_TARBALL_ROOTSYS_SH}
  COMMENT "Tarball install (tools)"
  )
add_dependencies(tarball-install-tools tarball-install)
add_custom_target(tarball-install-sh
  COMMAND ${MAKE_TARBALL_SH}
  COMMENT "Tarball install (env-shell)"
  )
add_dependencies(tarball-install-sh tarball-install)
if(USE_CVMFS OR (NOT INSTALL_TOOL_LIBS))
else()
  add_dependencies(tarball-install-sh tarball-install-tools)
endif()

add_custom_target(tarball-install-lite)
add_dependencies(tarball-install-lite tarball-install-sh)

# ensure that only relative paths end up in the tarball
get_filename_component(_tarball_basename ${CMAKE_INSTALL_PREFIX} NAME)
add_custom_target(tarball-finish
  COMMAND echo Creating ${CMAKE_INSTALL_PREFIX}.tar.gz
  COMMAND tar czf ${_tarball_basename}.tar.gz ${_tarball_basename}
  COMMAND echo Checksumming ${CMAKE_INSTALL_PREFIX}.tar.gz
  COMMAND ${MD5SUM_TARBALL_COMMAND}
  COMMENT "Finishing tarball of ${CMAKE_INSTALL_PREFIX}"
  )
add_dependencies(tarball-finish tarball-install-sh)

add_custom_target(tarball)
add_dependencies(tarball tarball-finish)

# this is outside because it contains the test macros, which 
# might be noops
#include(testing)

option(INSTALL_HEADERS "install header files when making tarball" OFF)
option(INSTALL_TOOL_LIBS "install libraries from I3_PORTS when making tarball" ON)

# if not shipping I3_PORTS, keep existing external links when installing
if(NOT INSTALL_TOOL_LIBS)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif(NOT INSTALL_TOOL_LIBS)

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

#
#  extra icetray-inspect docs goodness.
#
find_program(XSLTPROC_BIN xsltproc DOC "Location of the XSLT processor")

#
#  Temp directory for documentation
#
file(MAKE_DIRECTORY "${DOXYGEN_OUTPUT_PATH}/.tagfiles")

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

#i3_add_testing_targets()

colormsg("")
colormsg(_HIBLUE_ "Configuring projects:")
colormsg("")

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
add_custom_target(inspect-docs)
add_dependencies(inspect-docs inspect)

add_custom_target(doxygen-docs)
add_dependencies(doxygen-docs doxygen)

add_custom_target(html-docs)
add_dependencies(html-docs html)

add_custom_target(docs)
add_dependencies(inspect-docs doxygen-docs)
add_dependencies(html-docs inspect-docs)
add_dependencies(docs html-docs)

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
# make_tarball.sh
#
set(MAKE_TARBALL_SH ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/make_tarball.sh
  CACHE INTERNAL
  "Generated file run by 'tarball' target that in turn runs 'tarball_hook.sh'"
  )

configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/make_tarball.sh.in
  ${MAKE_TARBALL_SH}
  @ONLY
  )
execute_process(COMMAND chmod 755 ${MAKE_TARBALL_SH})

set(MAKE_TARBALL_ROOTSYS_SH ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/make_tarball_rootsys.sh
  CACHE INTERNAL
  "Generated file run by 'tarball_rootsys' target that setups tarball ROOTSYS voodoo"
  )

configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/make_tarball_rootsys.sh.in
  ${MAKE_TARBALL_ROOTSYS_SH}
  @ONLY
  )
execute_process(COMMAND chmod 755 ${MAKE_TARBALL_ROOTSYS_SH})


if(EXISTS ${CMAKE_SOURCE_DIR}/tarball_hook.sh.in) 
  message(STATUS "Generating tarball_hook.sh")
  configure_file(
    ${CMAKE_SOURCE_DIR}/tarball_hook.sh.in
    ${CMAKE_BINARY_DIR}/tarball_hook.sh
    @ONLY
    )
  execute_process(COMMAND chmod 755 ${CMAKE_BINARY_DIR}/tarball_hook.sh)
endif(EXISTS ${CMAKE_SOURCE_DIR}/tarball_hook.sh.in) 

message(STATUS "Configuring 'gfilt' STL decryptor")
configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/gfilt.in
  ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/gfilt
  @ONLY
  )
set(GFILT_PROGRAM 
  ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/gfilt
  CACHE FILEPATH "Gfilt error filter path")

  
find_program(GFILT_PROGRAM gfilt PATHS ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY} NO_DEFAULT_PATH)

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

add_custom_target(retarball
  COMMAND ${CMAKE_MAKE_PROGRAM} wipe-tarball
  COMMAND ${CMAKE_MAKE_PROGRAM} tarball
  COMMENT "Re-Tarballing ${CMAKE_INSTALL_PREFIX}"
  )

add_custom_target(wipe-tarball
  COMMAND /bin/echo Wiping old tarball and installation directory ${CMAKE_INSTALL_PREFIX}...
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}
  COMMAND rm -f ${CMAKE_INSTALL_PREFIX}.tar.gz
  COMMENT "Wiping old tarball ${CMAKE_INSTALL_PREFIX}"
  )

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/docs/inspect")

string(REGEX REPLACE "s\\.V.*$" "" DEST ${META_PROJECT})
string(REGEX REPLACE "-software" "" DEST ${DEST})
string(REGEX REPLACE "\\." "_" DEST ${DEST})
string(REGEX REPLACE "_release$" "" DEST ${DEST})
add_custom_target(deploy-docs
  COMMAND rsync -va --delete ${CMAKE_BINARY_DIR}/docs/ buildmaster@dragon:/opt/docs/${DEST}/
  COMMENT Deploying docs to ${DEST}
  )


## coverage target
add_custom_target(coverage
  COMMAND if test ! -d ../output \; then mkdir ../output\; fi
  COMMAND lcov -b ../ -d . -z
  COMMAND lcov -b ../ -d . -c -i -o test_base.info
  COMMAND ./env-shell.sh ctest -j2
  COMMAND lcov -b ../ -d . -c -o test_run.info
  COMMAND lcov -b ../ -d . -a test_base.info -a test_run.info -o test_total.info
  COMMAND lcov -r test_total.info '/usr/include/*' '$ENV{I3_PORTS}/*' '*/numpy' '/usr/lib/gcc/*' -o reports.info
  COMMAND genhtml --legend -o ../output/`date +%Y-%m-%d` reports.info
  COMMAND rm -f ../output/00_LATEST \; ln -sf `ls -1tr ../output |tail -1` ../output/00_LATEST
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


