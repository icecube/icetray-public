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
message(STATUS "Configuring tools...")

include_directories(cmake/tool-patches/common)
add_custom_target(install_tool_libs)

include(tooldef)

#
#  These guys are in subdirectory 'tools'
#
set(ALL_TOOLS pthread root I3Boost python
    rdmc expat gsl sprng sla
    mysql bdb log4cplus photonics ptd
    ZThread omniORB fox jni ncurses
    cdk I3Qt4 cfitsio
    )
  
foreach(tool ${ALL_TOOLS})
  find_package(${tool})
endforeach(tool ${ALL_TOOLS})

macro(use_tool TARGET TOOL_)
  string(TOUPPER ${TOOL_} TOOL)
  if(NOT ${TOOL}_FOUND)
    message(FATAL_ERROR "Attempt to use tool '${TOOL_}' which wasn't found")
  endif(NOT ${TOOL}_FOUND)

  if(${TOOL}_CONFIG_ERROR)
    message(FATAL_ERROR "There was a problem configuring tool ${TOOL} which is required by target ${TARGET}.  This is probably available via the 'ports' system.  Try the command 'port search ${TOOL_}' and 'port install' what you find.")
  endif(${TOOL}_CONFIG_ERROR)

  include_directories(${${TOOL}_INCLUDE_DIR})
  target_link_libraries(${TARGET} ${${TOOL}_LIBRARIES})
  
  if(${TOOL}_LINK_FLAGS)
    get_target_property(tmp_LINK_FLAGS ${TARGET} LINK_FLAGS)
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
  
