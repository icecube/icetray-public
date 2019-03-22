#
# Tarball target
#
option(INSTALL_HEADERS "install header files when making tarball" OFF)
option(INSTALL_TOOL_LIBS "install libraries when making tarball" ON)

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

# keep existing external links when installing
if(NOT INSTALL_TOOL_LIBS)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif(NOT INSTALL_TOOL_LIBS)

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
