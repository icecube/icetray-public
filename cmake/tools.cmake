#
#  $Id$
#
#  Copyright (C) 2007-9  Troy D. Straszheim  <troy@icecube.umd.edu>
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
colormsg("")
colormsg(_HIBLUE_ "Configuring tools...")
colormsg("")

include_directories(cmake/tool-patches/common)
add_custom_target(install_tool_libs)

include(tooldef)

#
#  By default, use /usr/share/fizzicks/cmake as I3_SITE_CMAKE_DIR
#
if (NOT IS_DIRECTORY $ENV{I3_SITE_CMAKE_DIR})
  set (I3_SITE_CMAKE_DIR "/usr/share/fizzicks/cmake"
    CACHE PATH "Path to site-specific cmake files")
  message(STATUS "Using default site cmake dir of ${I3_SITE_CMAKE_DIR}")
else()
  set (I3_SITE_CMAKE_DIR $ENV{I3_SITE_CMAKE_DIR}
    CACHE PATH "Path to site-specific cmake files")
  message(STATUS "Using user-configured I3_SITE_CMAKE_DIR=${I3_SITE_CMAKE_DIR}")
endif()

include(all_tools)
#
#  Load each tool, directory preferences are, in order:
#  * I3_SITE_CMAKE_DIR
#  * I3_EXTRA_TOOLS_DIR
#  * CMAKE_SOURCE_DIR/cmake/tools
#
foreach(tool ${ALL_TOOLS})

  if (EXISTS ${I3_SITE_CMAKE_DIR}/${tool}.cmake)
    message(STATUS "")
    message(STATUS "Reading ${tool} from site cmake dir")
    include(${I3_SITE_CMAKE_DIR}/${tool}.cmake)
  elseif(EXISTS "${I3_EXTRA_TOOLS_DIR}/${tool}.cmake")
    message(STATUS "")
    message(STATUS "Reading ${tool} from extra tools dir")
    include(${I3_EXTRA_TOOLS_DIR}/${tool}.cmake)
  elseif(EXISTS ${CMAKE_SOURCE_DIR}/cmake/tools/${tool}.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/tools/${tool}.cmake)
  else(EXISTS ${I3_SITE_CMAKE_DIR}/${tool}.cmake)
    message(STATUS "*** ${tool}.cmake not found in I3_SITE_CMAKE_DIR, I3_EXTRA_TOOLS_DIR, or cmake/tools")
    message(STATUS "*** This is only an error if a projects requires tool '${tool}'")
  endif()

endforeach()

macro(use_tool TARGET TOOL_)
  string(TOUPPER ${TOOL_} TOOL)
  if(NOT ${TOOL}_FOUND)
    message(FATAL_ERROR "Attempt to use tool '${TOOL_}' which wasn't found")
  endif(NOT ${TOOL}_FOUND)

  if(${TOOL}_CONFIG_ERROR)
    message(FATAL_ERROR "There was a problem configuring tool ${TOOL} which is required by target ${TARGET}.")
  endif(${TOOL}_CONFIG_ERROR)

  include_directories(SYSTEM ${${TOOL}_INCLUDE_DIRS})
  if(NOT ${TOOL}_INCLUDE_DIRS)
    include_directories(SYSTEM ${${TOOL}_INCLUDE_DIR})
  endif(NOT ${TOOL}_INCLUDE_DIRS)

  target_link_libraries(${TARGET} ${${TOOL}_LIBRARIES})
  foreach(library ${${TOOL}_LIBRARIES})
    list(APPEND ${TARGET}_LIBRARIES ${library})
  endforeach(library ${${TOOL}_LIBRARIES})

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

  string(COMPARE EQUAL ${TOOL} "ROOT" ISROOT)
  if("${ROOT_CXX_STANDARD}" AND ${ISROOT})
    colormsg(RED "     + using c++${ROOT_CXX_STANDARD} for ${TARGET}")
    set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD ${ROOT_CXX_STANDARD})
  endif()

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
  
  if(NOT ${THIS_USE_TOOLS_TARGET}_COMPILE_FLAGS)
    set(${THIS_USE_TOOLS_TARGET}_COMPILE_FLAGS "")
  else(NOT ${THIS_USE_TOOLS_TARGET}_COMPILE_FLAGS)
    list(REMOVE_DUPLICATES ${THIS_USE_TOOLS_TARGET}_COMPILE_FLAGS)
  endif(NOT ${THIS_USE_TOOLS_TARGET}_COMPILE_FLAGS)

  get_property(${THIS_USE_TOOLS_TARGET}_INCLUDE_DIRS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
  set(${THIS_USE_TOOLS_TARGET}_INCLUDE_DIRS ${${THIS_USE_TOOLS_TARGET}_INCLUDE_DIRS} CACHE INTERNAL "Directories included by library ${THIS_USE_TOOLS_TARGET}")
  
  set(${THIS_USE_TOOLS_TARGET}_COMPILE_FLAGS ${${THIS_USE_TOOLS_TARGET}_COMPILE_FLAGS} CACHE INTERNAL "Compilation flags used by library ${THIS_USE_TOOLS_TARGET}")
  
  set(${THIS_USE_TOOLS_TARGET}_LIBRARY_DEPENDS ${${THIS_USE_TOOLS_TARGET}_LIBRARIES} CACHE INTERNAL "Libraries against which ${THIS_USE_TOOLS_TARGET} links")

endmacro(use_tools)
