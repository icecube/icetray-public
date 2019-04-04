#
#  Copyright (C) 2011
#  Claudio Kopper <claudio.kopper@icecube.wisc.edu>
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
colormsg(HICYAN "genie")

if($ENV{SROOTBASE} MATCHES "py3-v4$")
  colormsg(YELLOW "GENIE v3 is incomplete under py3-v4, and is therefore banned.")
  colormsg(YELLOW "A forthcoming update will rectify this")
  return()
endif()
  
find_program (GENIE_CONFIG genie-config)

if (${GENIE_CONFIG} MATCHES ".*NOTFOUND$")
  message(STATUS "Looking for Genie genie-config program -- not found")
  set(GENIE_CONFIG_ERROR TRUE)
else ()
  message(STATUS "Looking for Genie genie-config program -- found")

  execute_process(
    COMMAND bash -c "${GENIE_CONFIG} --libs"
    OUTPUT_VARIABLE GENIE_CONFIG_OUTPUT OUTPUT_STRIP_TRAILING_WHITESPACE
    )

  string(REGEX REPLACE "-l([^ ]*)" "${CMAKE_SHARED_LIBRARY_PREFIX}\\1${CMAKE_SHARED_LIBRARY_SUFFIX}" GENIE_LIBRARIES ${GENIE_CONFIG_OUTPUT})
  string(REGEX REPLACE "-L([^ ]*)" "" GENIE_LIBRARIES ${GENIE_LIBRARIES})
  separate_arguments(GENIE_LIBRARIES)

  string(REGEX REPLACE "-L([^ ]*)" "\\1" GENIE_LIB_DIR ${GENIE_CONFIG_OUTPUT})
  string(REGEX REPLACE "-l([^ ]*)" "" GENIE_LIB_DIR ${GENIE_LIB_DIR})
  string(STRIP ${GENIE_LIB_DIR} GENIE_LIB_DIR)

  set(GENIE_INC_DIR $ENV{GENIE}/include/GENIE)

  find_path(GENIE_INCLUDE_DIR NAMES Conventions/GVersion.h PATHS ${GENIE_INC_DIR} ${TOOL_SYSTEM_PATH})
  if(${GENIE_INCLUDE_DIR} MATCHES ".*NOTFOUND$")
    ## we assume that we've got GENIE v3 here. we shouldn't
    set(GENIE_VERSION "3.0.0")
    set(GENIE_GVERSION_H "Framework/Conventions/GVersion.h")
    find_path(GENIE_INCLUDE_DIR NAMES Framework/Conventions/GVersion.h PATHS ${GENIE_INC_DIR} ${TOOL_SYSTEM_PATH})
  else()
    set(GENIE_VERSION "2.8.6")
    set(GENIE_GVERSION_H "Conventions/GVersion.h")
  endif()

  TOOLDEF (genie
    ${GENIE_INC_DIR}
    ${GENIE_GVERSION_H}
    ${GENIE_LIB_DIR}
    NONE  # bin is n/a, placeholder
    ${GENIE_LIBRARIES}
    )
endif ()
