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

if(NOT GENIE_VERSION)
  message(STATUS "Looking for Genie version")

  if(IS_DIRECTORY ${I3_PORTS}/Genie-2.6.4)
    set(GENIE_VERSION "2.6.4")
  elseif(IS_DIRECTORY ${I3_PORTS}/Genie-2.6.2)
    set(GENIE_VERSION "2.6.2")
  elseif(IS_DIRECTORY ${I3_PORTS}/Genie-2.6.0)
    set(GENIE_VERSION "2.6.0")
  else(IS_DIRECTORY ${I3_PORTS}/Genie-2.6.4)
    unset(GENIE_VERSION)
  endif(IS_DIRECTORY ${I3_PORTS}/Genie-2.6.4)

else(NOT GENIE_VERSION)
  message(STATUS "Using already configured Genie version: ${GENIE_VERSION}")
endif(NOT GENIE_VERSION)


# does Genie exist at all?
if(NOT GENIE_VERSION)
    message(STATUS "Genie not installed.")
else(NOT GENIE_VERSION)
    message(STATUS "Configuring for Genie version ${GENIE_VERSION}.")

    set(GENIE_PATH ${I3_PORTS}/Genie-${GENIE_VERSION} CACHE STRING "Genie's very own ROOTSYS equivalent.." FORCE)

    #message(STATUS "Looking for Genie genie-config program")

    find_program (GENIE_CONFIG genie-config
      PATHS ${GENIE_PATH}/bin
      NO_DEFAULT_PATH
      )

    if (${GENIE_CONFIG} MATCHES ".*NOTFOUND$")
      message(STATUS "Looking for Genie genie-config program -- not found")
      set(GENIE_CONFIG_ERROR TRUE)

    else (${GENIE_CONFIG} MATCHES ".*NOTFOUND$")
      #message(STATUS "Looking for Genie genie-config program -- found")

      #message(STATUS "Genie-config: ${GENIE_CONFIG}")

      execute_process(#COMMAND export }
        COMMAND bash -c "GENIE=${GENIE_PATH} ${GENIE_CONFIG} --libs"
        OUTPUT_VARIABLE GENIE_CONFIG_OUTPUT OUTPUT_STRIP_TRAILING_WHITESPACE
        )

      #message(STATUS "Genie-config output: ${GENIE_CONFIG_OUTPUT}")

      string(REGEX REPLACE "-l([^ ]*)" "${CMAKE_SHARED_LIBRARY_PREFIX}\\1${CMAKE_SHARED_LIBRARY_SUFFIX}" GENIE_LIBRARIES ${GENIE_CONFIG_OUTPUT})
      string(REGEX REPLACE "-L([^ ]*)" "" GENIE_LIBRARIES ${GENIE_LIBRARIES})
      #message(STATUS "Genie libraries: ${GENIE_LIBRARIES}")
      separate_arguments(GENIE_LIBRARIES)

      string(REGEX REPLACE "-L([^ ]*)" "\\1" GENIE_LIB_DIR ${GENIE_CONFIG_OUTPUT})
      string(REGEX REPLACE "-l([^ ]*)" "" GENIE_LIB_DIR ${GENIE_LIB_DIR})
      string(STRIP ${GENIE_LIB_DIR} GENIE_LIB_DIR)
      #message(STATUS "Genie lib dir: \"${GENIE_LIB_DIR}\"")

      string (REPLACE "${I3_PORTS}/" "" GENIE_RELATIVE_LIB_DIR "${GENIE_LIB_DIR}")
      #message(STATUS "Genie relative lib dir: \"${GENIE_RELATIVE_LIB_DIR}\"")


      set(GENIE_INC_DIR ${GENIE_PATH}/include/GENIE)
      string (REPLACE "${I3_PORTS}/" "" GENIE_RELATIVE_INC_DIR "${GENIE_INC_DIR}")

      #message(STATUS "Genie include dir: \"${GENIE_INC_DIR}\"")
      #message(STATUS "Genie relative include dir: \"${GENIE_RELATIVE_INC_DIR}\"")

      TOOLDEF (genie
        ${GENIE_RELATIVE_INC_DIR}
        EVGCore/EventRecord.h
        ${GENIE_RELATIVE_LIB_DIR}
        NONE  # bin is n/a, placeholder
        ${GENIE_LIBRARIES}
        )

    endif (${GENIE_CONFIG} MATCHES ".*NOTFOUND$")

endif(NOT GENIE_VERSION)
