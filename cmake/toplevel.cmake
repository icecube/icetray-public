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
cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)
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

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

#
#  this was a failed attempt to fix ticket #181.
#  
#
# execute_process(COMMAND ${CMAKE_SOURCE_DIR}/cmake/readlink.py ${CMAKE_SOURCE_DIR}
#   OUTPUT_VARIABLE I3_SRC
#   )
# message(STATUS "After resolving links I3_SRC=${I3_SRC}")
# 
# execute_process(COMMAND ${CMAKE_SOURCE_DIR}/cmake/readlink.py ${CMAKE_BINARY_DIR}
#   OUTPUT_VARIABLE I3_BUILD
#   )
# message(STATUS "After resolving links I3_BUILD=${I3_BUILD}")

set(I3_SRC ${CMAKE_SOURCE_DIR})
set(I3_BUILD ${CMAKE_BINARY_DIR})

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
  message(STATUS "I3_BUILD not yet set in environment.  ok.")
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
  message(STATUS "I3_SRC not yet set in environment.  ok.")
endif()



#
#  This is nice and all, but causes intermittent hangs on the mac
#  due to an extremely old bash.  And in some places your path gets automagically
#  prepended with /usr/kerberos/bin.
#
# execute_process(
#   COMMAND ${CMAKE_SOURCE_DIR}/cmake/envtest/parent.sh ${CMAKE_SOURCE_DIR}/cmake/envtest/child.sh
#   RESULT_VARIABLE ENVTEST_RESULT
#   ERROR_QUIET
#   )
# 
# if(ENVTEST_RESULT)
#   message(FATAL_ERROR 
#     "Problem with your shell initialization scripts.  Cannot continue,\n"
#     "as this would just result in very irritating errors later.\n"
#     "Fix your shell initialization scripts and retry.\n")
# endif(ENVTEST_RESULT)
# 

include(${CMAKE_SOURCE_DIR}/CMakeLists.optional.txt OPTIONAL)

set(CMAKE_MODULE_PATH
  ${EXTRA_CMAKE_MODULE_PATH} 
  ${CMAKE_SOURCE_DIR}/cmake 
  ${CMAKE_SOURCE_DIR}/cmake/tools 
  ${CMAKE_SOURCE_DIR}/cmake/utility)

include(utility)

include(config)
include(tools)
include(project)

add_custom_target(test
  COMMAND ${CMAKE_BINARY_DIR}/env-shell.sh ${EXECUTABLE_OUTPUT_PATH}/runtests.py
  COMMENT ">>>
>>>  Running tests via the utility 'runtests.py' in your $I3_BUILD/bin/ directory.\n
>>>  Run without arguments to see help/options.
>>>")
add_custom_target(test-bins)
add_dependencies(test test-bins)

set(INSPECT_ALL_HTML ${CMAKE_BINARY_DIR}/doxygen/inspect/index.html)

add_custom_target(doxygen)
add_custom_target(inspect)
add_dependencies(inspect icetray-inspect)

add_custom_target(pybindings)
add_dependencies(doxygen inspect)

add_custom_target(docs)
add_dependencies(docs doxygen inspect html)

#
# Tarball target
#
find_program(MD5SUM_PROGRAM md5sum)
if(MD5SUM_PROGRAM)
  set(MD5SUM_TARBALL_COMMAND md5sum ${CMAKE_INSTALL_PREFIX}.tar.gz > ${CMAKE_INSTALL_PREFIX}.md5sum)
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
  COMMAND ${CMAKE_MAKE_PROGRAM} install_tool_libs
  COMMAND ${MAKE_TARBALL_SH}
  COMMAND ${MAKE_TARBALL_ROOTSYS_SH}
  COMMENT "Tarball install"
  )
add_dependencies(tarball-install tarball-start)
add_custom_target(tarball-finish
  COMMAND echo Creating ${CMAKE_INSTALL_PREFIX}.tar.gz
  COMMAND tar czf ${CMAKE_INSTALL_PREFIX}.tar.gz ${CMAKE_INSTALL_PREFIX}
  COMMAND echo Checksumming ${CMAKE_INSTALL_PREFIX}.tar.gz
  COMMAND ${MD5SUM_TARBALL_COMMAND}
  COMMENT "Finishing tarball of ${CMAKE_INSTALL_PREFIX}"
  )
add_dependencies(tarball-finish tarball-install)

add_custom_target(tarball)
add_dependencies(tarball tarball-finish)

# this is outside because it contains the test macros, which 
# might be noops
include(testing)
include(BuildSlave)

option(INSTALL_HEADERS "install header files when making tarball" OFF)

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

configure_file(${CMAKE_SOURCE_DIR}/cmake/runtests.py.in
  ${EXECUTABLE_OUTPUT_PATH}/runtests.py
  @ONLY)

add_custom_target(env-check ALL 
  COMMAND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/env-check.sh
  COMMENT "Checking build against environment")

#
#  Temp directory for documentation
#
execute_process(COMMAND mkdir -p ${DOXYGEN_OUTPUT_PATH}/.tagfiles 
  OUTPUT_VARIABLE DEV_NULL)

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

i3_add_testing_targets()

message(STATUS "Configuring projects:")
foreach(subdir ${SUBDIRS})
  get_filename_component(pname ${subdir} NAME_WE)
  set(I3_PROJECTS "${I3_PROJECTS}" ${pname})
  message(STATUS "  ${pname}")
  add_subdirectory(${CMAKE_SOURCE_DIR}/${pname})
  exec_program(test ARGS -h ${CMAKE_BINARY_DIR}/${pname}/resources || ln -s ${CMAKE_SOURCE_DIR}/${pname}/resources ${CMAKE_BINARY_DIR}/${pname}/resources 
    OUTPUT_VARIABLE DEV_NULL)
  #
  # setup tagfiles for doxygen
  #
  exec_program(touch ARGS ${DOXYGEN_OUTPUT_PATH}/.tagfiles/${pname}.tag OUTPUT_VARIABLE DEV_NULL)
  file(WRITE ${DOXYGEN_OUTPUT_PATH}/.tagfiles/${pname}.include "# autogenerated doxy include file\n")
endforeach(subdir ${SUBDIRS})

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
  exec_program(cp ARGS ${CMAKE_BINARY_DIR}/${file} ${NOTES_DIR} OUTPUT_VARIABLE DEVNULL)
endforeach (file env-shell.sh)

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

if(NOT EXISTS ${CMAKE_BINARY_DIR}/log4cplus.conf)
  configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/log4cplus.conf.in
    ${CMAKE_BINARY_DIR}/log4cplus.conf
    )
else(NOT EXISTS ${CMAKE_BINARY_DIR}/log4cplus.conf)
  message(STATUS "Not configuring log4cplus.conf in ${CMAKE_BINARY_DIR} because the file already exists (ok).")
endif(NOT EXISTS ${CMAKE_BINARY_DIR}/log4cplus.conf)

install(FILES ${CMAKE_BINARY_DIR}/log4cplus.conf
  DESTINATION .)

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
    CACHE PATH "Installation path for dpkgable install" FORCE)
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

add_custom_target(rsync
  COMMAND mkdir -p ${I3_PORTS}
  COMMAND rsync -vrlpt --delete code.icecube.wisc.edu::Offline/test-data ${I3_PORTS}/
  COMMENT "Rsyncing test-data to I3_PORTS"
  )

execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/docs/inspect
  OUTPUT_VARIABLE DEV_NULL)

#
#  extra icetray-inspect docs goodness.
#
find_program(XSLTPROC_BIN xsltproc)

#
# subprocess.py needed by older Python
#
if(INSTALL_PYTHON_SUBPROCESS)
  configure_file(${CMAKE_SOURCE_DIR}/cmake/subprocess.py
    ${LIBRARY_OUTPUT_PATH}/subprocess.py
    COPYONLY)
endif(INSTALL_PYTHON_SUBPROCESS)

add_custom_target(deploy-docs
  COMMAND rsync -va --delete ${CMAKE_BINARY_DIR}/docs/ umdgrb.umd.edu:public_html/${META_PROJECT}/
  COMMENT Deploying docs to ${META_PROJECT}
  )

file(DOWNLOAD 
  http://www.icecube.umd.edu/~troy/.workspace_configured.html
  ${CMAKE_BINARY_DIR}/.tag
  TIMEOUT 0.5
  )
file(REMOVE ${CMAKE_BINARY_DIR}/.tag)
