#
# Top-level CMake file for a parasitic metaproject build
#
# Jakob van Santen (UW-Madison) 2013-12-02

# Use this to add projects to a pre-built meta-project where read-only copies
# of the source tree and build directory exist at a known and constant path, e.g.
#
# cmake ../src -DHOST_BUILD=/cvmfs/icecube.wisc.edu/py2-v1/RHEL_6_x86_64/metaprojects/simulation/trunk
#
# Tarballs created from this build will be extremely light-weight, containing only
# the new projects and symlinks to the parasitized metaproject.

cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)

set(HOST_BUILD "NONE" CACHE STRING "Path to host build directory")
if(HOST_BUILD STREQUAL "NONE;,")
  message(FATAL_ERROR "You must specify a host build directory")
endif(HOST_BUILD STREQUAL "NONE;,")

# Recover source and build directories for pre-built metaprojects
set(HOST_I3_BUILD ${HOST_BUILD})
load_cache(${HOST_BUILD} READ_WITH_PREFIX HOST_ icetray_SOURCE_DIR)
get_filename_component(HOST_I3_SRC ${HOST_icetray_SOURCE_DIR} PATH)

set(I3_SRC ${CMAKE_SOURCE_DIR})
set(I3_BUILD ${CMAKE_BINARY_DIR})

# pull SVN revision from cache
load_cache(${HOST_BUILD} READ_WITH_PREFIX "" SVN_REVISION SVN_URL META_PROJECT)
message(STATUS ${SVN_REVISION})
message(STATUS ${SVN_URL})
set(HAVE_META_PROJECT TRUE)
set(HAVE_SVN_REVISION TRUE)

set(CMAKE_MODULE_PATH
  ${EXTRA_CMAKE_MODULE_PATH} 
  ${CMAKE_SOURCE_DIR}/cmake 
  ${CMAKE_SOURCE_DIR}/cmake/tools 
  ${CMAKE_SOURCE_DIR}/cmake/utility)

# dummy testing target
add_custom_target(i3test COMMAND echo "We don't actually have tests.")
add_custom_target(pybindings)

include(utility)
set(CMAKE_SOURCE_DIR ${HOST_I3_SRC})
include(config)
set(CMAKE_SOURCE_DIR ${I3_SRC})
include(project)

# use pre-generated I3_UBER_HEADER
set(I3_UBER_HEADER ${HOST_I3_BUILD}${CMAKE_FILES_DIRECTORY}/I3.h)
include_directories(${HOST_I3_SRC}/cmake/tool-patches/common)

# keep existing external links when installing
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#
# A bare-bones version of use_projects() that knows how to use
# pre-built libraries
#
macro(use_projects THIS_TARGET)
  parse_arguments(${THIS_TARGET}_USE_PROJECTS
    "PROJECTS"
    ""
    ${ARGN}
    )
  foreach(USED_PROJECT ${${THIS_TARGET}_USE_PROJECTS_PROJECTS})
    if(EXISTS ${HOST_I3_SRC}/${USED_PROJECT}/CMakeLists.txt)
      # pre-compiled project; just find libraries
      include_directories(${HOST_I3_SRC}/${USED_PROJECT}/public)
      find_library(${${USED_PROJECT}_LIBRARIES} NAMES lib${USED_PROJECT} PATHS ${HOST_BUILD}/lib)
      target_link_libraries(${THIS_TARGET} ${${USED_PROJECT}_LIBRARIES})
    elseif(EXISTS ${I3_SRC}/${USED_PROJECT}/CMakeLists.txt)
      # parasite project; add dependencies
      include_directories(${CMAKE_SOURCE_DIR}/${USED_PROJECT}/public)
      target_link_libraries(${THIS_TARGET} ${USED_PROJECT})
    else()
      message(FATAL_ERROR "Attempt to use nonexistent project '${USED_PROJECT}'")
    endif(EXISTS ${HOST_I3_SRC}/${USED_PROJECT}/CMakeLists.txt)
  endforeach(USED_PROJECT ${${THIS_TARGET}_USE_PROJECTS_PROJECTS})
endmacro(use_projects THIS_TARGET)

# use tools from the previous cache
macro(use_tool TARGET TOOL_)
  string(TOUPPER ${TOOL_} TOOL)
  load_cache(${HOST_BUILD} READ_WITH_PREFIX ""
    ${TOOL}_FOUND
    ${TOOL}_CONFIG_ERROR
    ${TOOL}_INCLUDE_DIR
    ${TOOL}_INCLUDE_DIRS
    ${TOOL}_LIBRARIES
    ${TOOL}_LINK_FLAGS)
  if(NOT ${TOOL}_FOUND)
    message(FATAL_ERROR "Attempt to use tool '${TOOL_}' which wasn't found")
  endif(NOT ${TOOL}_FOUND)

  if(${TOOL}_CONFIG_ERROR)
    message(FATAL_ERROR "There was a problem configuring tool ${TOOL} which is required by target ${TARGET}.  This is probably available via the 'ports' system.  Try the command 'port search ${TOOL_}' and 'port install' what you find.")
  endif(${TOOL}_CONFIG_ERROR)

  include_directories(${${TOOL}_INCLUDE_DIRS})
  if(NOT ${TOOL}_INCLUDE_DIRS)
    include_directories(${${TOOL}_INCLUDE_DIR})
  endif(NOT ${TOOL}_INCLUDE_DIRS)
  target_link_libraries(${TARGET} ${${TOOL}_LIBRARIES})

  if(${TOOL}_LINK_FLAGS)
    get_target_property(tmp_LINK_FLAGS ${TARGET} LINK_FLAGS)
    if(NOT ${tmp_LINK_FLAGS})
      set(tmp_LINK_FLAGS "")
    endif(NOT ${tmp_LINK_FLAGS})
    set_target_properties(${TARGET}
      PROPERTIES
      LINK_FLAGS "${tmp_LINK_FLAGS} ${${TOOL}_LINK_FLAGS}"
    )
  endif(${TOOL}_LINK_FLAGS)
endmacro(use_tool TARGET TOOL)

macro(use_tools THIS_USE_TOOLS_TARGET)
  #
  #  Use each of the COMMON_TOOLS
  #
  parse_arguments(${THIS_USE_TOOLS_TARGET}_USE_TOOLS
    "TOOLS"
    ""
    ${ARGN}
    )
  unique(${THIS_USE_TOOLS_TARGET}_ALL_TOOLS "${${THIS_USE_TOOLS_TARGET}_USE_TOOLS_TOOLS}")
  set(THIS_USE_TOOLS_NEW_COMPILE_FLAGS "")
  foreach(USED_TOOL ${${THIS_USE_TOOLS_TARGET}_ALL_TOOLS})
    use_tool(${THIS_USE_TOOLS_TARGET} ${USED_TOOL})
  endforeach(USED_TOOL ${${THIS_USE_TOOLS_TARGET}_ALL_TOOLS})

endmacro(use_tools)

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
    get_filename_component(pname ${proj} NAME_WE)
    if(NOT (pname STREQUAL "cmake"))
      set(SUBDIRS ${SUBDIRS} ${proj})
    endif(NOT (pname STREQUAL "cmake"))
  endforeach(d ${cmake_projects})
else()
  message(STATUS "Using project list manually specified by I3_PROJECTS")
  set(SUBDIRS ${I3_PROJECTS})
endif()


message(STATUS "Generating env-shell.sh")
configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/env-shell.sh.in
  ${CMAKE_CURRENT_BINARY_DIR}/env-shell.sh
  @ONLY)
execute_process(COMMAND chmod 755 ${CMAKE_BINARY_DIR}/env-shell.sh)

colormsg("")
colormsg(_HIBLUE_ "Configuring parasitic projects:")
colormsg("")

foreach(subdir ${SUBDIRS})
  get_filename_component(pname ${subdir} NAME_WE)
  set(I3_PROJECTS "${I3_PROJECTS}" ${pname})
  message(STATUS "+ ${pname}")
  add_subdirectory(${CMAKE_SOURCE_DIR}/${pname})
  exec_program(test ARGS -h ${CMAKE_BINARY_DIR}/${pname}/resources -a -e ${CMAKE_BINARY_DIR}/${pname}/resources || ln -snf ${CMAKE_SOURCE_DIR}/${pname}/resources ${CMAKE_BINARY_DIR}/${pname}/resources 
    OUTPUT_VARIABLE DEV_NULL)
endforeach(subdir ${SUBDIRS})

# create a symbolic link (and also install it as such)
macro(create_symlink _filepath _sympath)
  get_filename_component(_symname ${_sympath} NAME)
  get_filename_component(_installdir ${_sympath} PATH)
  if (NOT EXISTS ${CMAKE_BINARY_DIR}/${_installdir})
    exec_program(mkdir ARGS -p ${CMAKE_BINARY_DIR}/${_installdir} OUTPUT_VARIABLE DEV_NULL)
    install(CODE "exec_program(mkdir ARGS -p ${CMAKE_INSTALL_PREFIX}/${_installdir} OUTPUT_VARIABLE DEV_NULL)")
  endif(NOT EXISTS ${CMAKE_BINARY_DIR}/${_installdir})
  exec_program(ln ARGS -snf ${_filepath} ${CMAKE_BINARY_DIR}/${_installdir}/${_symname} OUTPUT_VARIABLE DEV_NULL)
  install(CODE "exec_program(ln ARGS -sf ${_filepath} ${CMAKE_INSTALL_PREFIX}/${_installdir}/${_symname} OUTPUT_VARIABLE DEV_NULL)")
endmacro(create_symlink)

message(STATUS "Syminking resources from host projects")
file(GLOB host_resources ${HOST_I3_SRC}/*/resources)
foreach(fpath ${host_resources})
  get_filename_component(pdir ${fpath} PATH)
  get_filename_component(pname ${pdir} NAME)
  create_symlink(${pdir}/resources ${pname}/resources)
endforeach(fpath ${SUBDIRS})

file(GLOB host_python_packages ${HOST_I3_BUILD}/lib/icecube/*)
message(STATUS "Syminking Python packages from ${HOST_I3_BUILD}/lib/icecube/")
foreach(fpath ${host_python_packages})
  get_filename_component(pname ${fpath} NAME)
  create_symlink(${fpath} lib/icecube/${pname})
endforeach(fpath ${host_python_packages})

file(GLOB host_libraries ${HOST_I3_BUILD}/lib/*)
list(REMOVE_ITEM host_libraries ${HOST_I3_BUILD}/lib/icecube)
message(STATUS "Symlinking libraries from ${HOST_I3_BUILD}/lib/")
foreach(fpath ${host_libraries})
  get_filename_component(pname ${fpath} NAME)
  create_symlink(${fpath} lib/${pname})
endforeach(fpath ${host_libraries})

# lightweight tarball
configure_file(${CMAKE_SOURCE_DIR}/cmake/tarball-env-shell.sh.in
  ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/tarball-env-shell.sh
  @ONLY)

install(PROGRAMS ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/tarball-env-shell.sh
  DESTINATION .
  RENAME env-shell.sh
  )

add_custom_target(tarball-install
  COMMAND ${CMAKE_MAKE_PROGRAM} install
  COMMENT "Tarball install"
  )
add_custom_target(tarball-finish
  COMMAND echo Creating ${CMAKE_INSTALL_PREFIX}.tar.gz
  COMMAND tar czf ${CMAKE_INSTALL_PREFIX}.tar.gz ${CMAKE_INSTALL_PREFIX}
  COMMAND echo Checksumming ${CMAKE_INSTALL_PREFIX}.tar.gz
  COMMAND ${MD5SUM_TARBALL_COMMAND}
  COMMENT "Finishing tarball of ${CMAKE_INSTALL_PREFIX}"
  DEPENDS tarball-install
  )

add_custom_target(tarball DEPENDS tarball-finish)
