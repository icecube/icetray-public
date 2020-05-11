#
# Top-level CMake file for a parasitic metaproject build
#
# Jakob van Santen (UW-Madison) 2013-12-02
#
# Use this to add projects to a pre-built meta-project where read-only copies
# of the source tree and build directory exist at a known and constant path, e.g.
# in cvmfs. After checking out the cmake project and any others you would like to
# add, create a CMakeLists.txt containing the line:
#
# include(cmake/toplevel-parasite.cmake NO_POLICY_SCOPE)
#
# then create a build directory and run e.g.
#
# cmake ../src -DMETAPROJECT=simulation/trunk
#
# In environments like the one provided by /cvmfs/icecube.wisc.edu/setup.sh,
# the base metaproject specification will be expanded to the full path to a
# build for the selected toolset and current architecture. In other
# environments, the full path to an existing build directory may be used.
#
# Tarballs created from this build will be extremely light-weight, containing only
# the new projects and symlinks to the parasitized base metaproject.

cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)

set(CMAKE_MODULE_PATH
  ${EXTRA_CMAKE_MODULE_PATH} 
  ${CMAKE_SOURCE_DIR}/cmake 
  ${CMAKE_SOURCE_DIR}/cmake/tools 
  ${CMAKE_SOURCE_DIR}/cmake/utility)
include(utility)

colormsg("")
colormsg(_HIBLUE_ "Configuring parasitic metaproject")
colormsg("")

set(METAPROJECT "" CACHE STRING "Path to host build directory")
if(EXISTS ${METAPROJECT}/env-shell.sh)
  # user supplied full path
  get_filename_component(HOST_I3_BUILD ${METAPROJECT} ABSOLUTE)
elseif(DEFINED ENV{SROOT} AND EXISTS $ENV{SROOT}/metaprojects)
  # cvmfs.icecube.wisc.edu-stype $SROOT/metaprojects exists
  if(NOT METAPROJECT STREQUAL "")
    get_filename_component(_metaproject_family ${METAPROJECT} PATH)
    if(_metaproject_family STREQUAL "")
      set(_metaproject_family ${METAPROJECT})
    endif(_metaproject_family STREQUAL "")
  endif()
    
  # endif(NOT METAPROJECT STREQUAL "")
  if(EXISTS $ENV{SROOT}/metaprojects/${METAPROJECT}/env-shell.sh)
    # specified metaproject/version exists
    set(HOST_I3_BUILD $ENV{SROOT}/metaprojects/${METAPROJECT})
  elseif(NOT ${_metaproject_family} STREQUAL "" AND EXISTS $ENV{SROOT}/metaprojects/${_metaproject_family})
    colormsg(HIRED "No metaproject \"${METAPROJECT}\" in $ENV{SROOT}/metaprojects")
    colormsg(RED "Available versions of ${_metaproject_family}:")
    file(GLOB _metaproject_versions RELATIVE $ENV{SROOT}/metaprojects/${_metaproject_family} $ENV{SROOT}/metaprojects/${_metaproject_family}/*/env-shell.sh)
    foreach(_envshell ${_metaproject_versions})
      get_filename_component(_version ${_envshell} PATH)
      colormsg(YELLOW ${_version})
    endforeach(_envshell ${_metaproject_versions})
  else()
    colormsg(HIRED "No metaproject \"${METAPROJECT}\" in $ENV{SROOT}/metaprojects")
    colormsg(RED "Available metaprojects:")
    file(GLOB _metaproject_versions RELATIVE $ENV{SROOT}/metaprojects $ENV{SROOT}/metaprojects/*/*/env-shell.sh)
    foreach(_envshell ${_metaproject_versions})
      get_filename_component(_version ${_envshell} PATH)
      colormsg(YELLOW ${_version})
    endforeach(_envshell ${_metaproject_versions})
  endif(EXISTS $ENV{SROOT}/metaprojects/${METAPROJECT}/env-shell.sh)
endif(EXISTS ${METAPROJECT}/env-shell.sh)
if(NOT DEFINED HOST_I3_BUILD)
  message(FATAL_ERROR "You must specify a base metaproject/version with e.g. -DMETAPROJECT=simulation/trunk. Alternatively, you may specify the full path to a build directory of a pre-built metaproject")
endif()
set(NICKNAME "" CACHE STRING "Nickname for this build. This will be included in the tarball name")

# Recover source director from the pre-built metaproject
load_cache(${HOST_I3_BUILD} READ_WITH_PREFIX HOST_ icetray_SOURCE_DIR)
get_filename_component(HOST_I3_SRC ${HOST_icetray_SOURCE_DIR} PATH)

set(I3_SRC ${CMAKE_SOURCE_DIR})
set(I3_BUILD ${CMAKE_BINARY_DIR})
set(SPHINX_DIR "${CMAKE_BINARY_DIR}/sphinx_src")

# pull SVN revision from cache
load_cache(${HOST_I3_BUILD} READ_WITH_PREFIX "" SVN_REVISION SVN_URL META_PROJECT Python_VERSION)
set(HAVE_META_PROJECT TRUE)
set(HAVE_SVN_REVISION TRUE)
# use a more idiomatic name for the tarball
if(DEFINED ENV{OS_ARCH})
  set(CMAKE_INSTALL_PREFIX ${META_PROJECT})
  if(NOT NICKNAME STREQUAL "")
    set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}.${NICKNAME})
  endif(NOT NICKNAME STREQUAL "")
  if(META_PROJECT MATCHES "trunk$")
    set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}.r${SVN_REVISION})
  endif(META_PROJECT MATCHES "trunk$")
  set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}.$ENV{OS_ARCH} CACHE STRING "Install prefix.  Also name of tarball.")
endif(DEFINED ENV{OS_ARCH})
message(STATUS "Base metaproject: ${HOST_I3_BUILD}")
message(STATUS "Built from:       ${SVN_URL} r${SVN_REVISION}")
message(STATUS "Install prefix:   ${CMAKE_INSTALL_PREFIX}")

# dummy testing target
add_custom_target(test-bins COMMAND echo "We don't actually have tests.")
add_custom_target(pybindings)

set(CMAKE_SOURCE_DIR ${HOST_I3_SRC})
include(config)
set(CMAKE_SOURCE_DIR ${I3_SRC})
include(project)

# use pre-generated I3_UBER_HEADER
set(I3_UBER_HEADER ${HOST_I3_BUILD}${CMAKE_FILES_DIRECTORY}/I3.h)
include_directories(${HOST_I3_SRC}/cmake/tool-patches/common)
# add patches for ancient boost versions if we've got 'em
load_cache(${HOST_I3_BUILD} READ_WITH_PREFIX "" BOOST_VERSION)
if(IS_DIRECTORY ${HOST_I3_SRC}/cmake/tool-patches/boost-${BOOST_VERSION})
	include_directories(${HOST_I3_SRC}/cmake/tool-patches/boost-${BOOST_VERSION})
endif(IS_DIRECTORY ${HOST_I3_SRC}/cmake/tool-patches/boost-${BOOST_VERSION})

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
      find_library(${USED_PROJECT}_LIBRARIES NAMES lib${USED_PROJECT}${CMAKE_SHARED_LIBRARY_SUFFIX} PATHS ${HOST_I3_BUILD}/lib NO_DEFAULT_PATH)
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

#
# A bare-bones version of use_pybindings() 
#
macro(use_pybindings THIS_TARGET)
  parse_arguments(${THIS_TARGET}_USE_PROJECTS
    "PROJECTS"
    ""
    ${ARGN}
    )
  foreach(USED_PROJECT ${${THIS_TARGET}_USE_PROJECTS_PROJECTS})
    if(EXISTS ${HOST_I3_SRC}/${USED_PROJECT}/CMakeLists.txt)
      # pre-compiled project; do nothing
    elseif(EXISTS ${I3_SRC}/${USED_PROJECT}/CMakeLists.txt)
      # parasite project; add dependencies
      add_dependencies(${THIS_TARGET} ${USED_PROJECT}-pybindings)
    else()
      message(FATAL_ERROR "Attempt to use nonexistent project '${USED_PROJECT}'")
    endif(EXISTS ${HOST_I3_SRC}/${USED_PROJECT}/CMakeLists.txt)
  endforeach(USED_PROJECT ${${THIS_TARGET}_USE_PROJECTS_PROJECTS})
endmacro(use_pybindings THIS_TARGET)

#
#  Greedily load info about all known tools
#
include(all_tools)
# Some tools don't have their own file
list(APPEND ALL_TOOLS scipy numpy)
list(REMOVE_DUPLICATES ALL_TOOLS)
foreach(TOOL_ ${ALL_TOOLS})
  string(TOUPPER ${TOOL_} TOOL)
  load_cache(${HOST_I3_BUILD} READ_WITH_PREFIX ""
    ${TOOL}_FOUND
    ${TOOL}_CONFIG_ERROR
    ${TOOL}_INCLUDE_DIR
    ${TOOL}_INCLUDE_DIRS
    ${TOOL}_LIBRARIES
    ${TOOL}_LINK_FLAGS)  
endforeach(TOOL_ ${ALL_TOOLS})

# Set compatibility shims for Python version etc. info
set(Python_EXECUTABLE ${Python_EXECUTABLE})
set(Python_VERSION ${Python_VERSION})
set(Python_LIBRARIES ${Python_LIBRARIES})
set(Python_INCLUDE_DIR ${Python_INCLUDE_DIRS})

# use tools from the previous cache
macro(use_tool TARGET TOOL_)
  string(TOUPPER ${TOOL_} TOOL)
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
  exec_program(mkdir ARGS -p ${CMAKE_BINARY_DIR}/${_installdir} OUTPUT_VARIABLE DEV_NULL)
  install(CODE "exec_program(mkdir ARGS -p ${CMAKE_INSTALL_PREFIX}/${_installdir} OUTPUT_VARIABLE DEV_NULL)")
  exec_program(ln ARGS -snf ${_filepath} ${CMAKE_BINARY_DIR}/${_installdir}/${_symname} OUTPUT_VARIABLE DEV_NULL)
  install(CODE "exec_program(ln ARGS -sf ${_filepath} ${CMAKE_INSTALL_PREFIX}/${_installdir}/${_symname} OUTPUT_VARIABLE DEV_NULL)")
endmacro(create_symlink)

message(STATUS "Symlinking resources from host projects")
file(GLOB host_resources ${HOST_I3_SRC}/*/resources)
foreach(fpath ${host_resources})
  get_filename_component(pdir ${fpath} PATH)
  get_filename_component(pname ${pdir} NAME)

  ## if rpath and spath match, the project is *not* parasitic, and we don't want to overwrite
  ## the 'resources/' symlink
  get_filename_component(rpath ${CMAKE_BINARY_DIR}/${pname}/resources REALPATH)
  get_filename_component(spath ${CMAKE_SOURCE_DIR}/${pname}/resources REALPATH)
  if(NOT (EXISTS ${spath} AND (${spath} MATCHES ${rpath})))
    create_symlink(${pdir}/resources ${pname}/resources)
  else()
    colormsg(YELLOW "Skipping symlink of ${pname}, as it is not parasitic")
  endif()
endforeach(fpath ${SUBDIRS})

file(GLOB host_python_packages ${HOST_I3_BUILD}/lib/icecube/*)
message(STATUS "Symlinking Python packages from ${HOST_I3_BUILD}/lib/icecube/")
foreach(fpath ${host_python_packages})
  get_filename_component(pname ${fpath} NAME)
  create_symlink(${fpath} lib/icecube/${pname})
endforeach(fpath ${host_python_packages})

file(GLOB host_binaries ${HOST_I3_BUILD}/bin/*)
message(STATUS "Symlinking binaries from ${HOST_I3_BUILD}/bin/")
foreach(fpath ${host_binaries})
  get_filename_component(pname ${fpath} NAME)
  create_symlink(${fpath} bin/${pname})
endforeach(fpath ${host_binaries})

file(GLOB host_libraries ${HOST_I3_BUILD}/lib/*)
list(REMOVE_ITEM host_libraries ${HOST_I3_BUILD}/lib/icecube)
message(STATUS "Symlinking libraries from ${HOST_I3_BUILD}/lib/")
foreach(fpath ${host_libraries})
  get_filename_component(pname ${fpath} NAME)
  create_symlink(${fpath} lib/${pname})
endforeach(fpath ${host_libraries})

find_program(MD5SUM_PROGRAM md5sum)
find_program(MD5SUM_PROGRAM md5)
if(MD5SUM_PROGRAM)
  set(MD5SUM_TARBALL_COMMAND ${MD5SUM_PROGRAM} ${CMAKE_INSTALL_PREFIX}.tar.gz > ${CMAKE_INSTALL_PREFIX}.md5sum)
else(MD5SUM_PROGRAM)
  set(MD5SUM_TARBALL_COMMAND /bin/echo Skipping md5sum, as md5sum command was not found.)
endif(MD5SUM_PROGRAM)

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

#
#  Environment checking targets
#
configure_file(${CMAKE_SOURCE_DIR}/cmake/env-check.sh.in
  ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/env-check.sh
  @ONLY)

add_custom_target(env-check ALL 
  COMMAND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/env-check.sh
  COMMENT "Checking build against environment")
