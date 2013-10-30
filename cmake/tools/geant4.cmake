#
#  cmake/tools/geant4.cmake
#
#  Copyright (C) 2010
#  the IceCube Collaboration <http://www.icecube.wisc.edu>
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

# first look for Geant4 >= 4.9.5 (which does not need CLHEP)
# (it installs the tool "geant4-config" to $I3_PORTS/bin)
message(STATUS "Looking for Geant4 geant4-config program")

find_program (GEANT4_CONFIG geant4-config
  PATHS ${I3_PORTS}/bin
  )
 
if (${GEANT4_CONFIG} MATCHES ".*NOTFOUND$")
    message(STATUS "Looking for Geant4 geant4-config program -- not found")
    set(GEANT4_CONFIG_TOOL_FOUND FALSE)
else (${GEANT4_CONFIG} MATCHES ".*NOTFOUND$")
    message(STATUS "Looking for Geant4 geant4-config program -- found")
    set(GEANT4_CONFIG_TOOL_FOUND TRUE)
    
    # extract the library list from geant4-config
    execute_process(COMMAND ${GEANT4_CONFIG} --libs
      OUTPUT_VARIABLE GEANT4_CONFIG_OUTPUT OUTPUT_STRIP_TRAILING_WHITESPACE
      )

    # look for the -L option
    string(REGEX MATCH "-L([^ ]*)" GEANT4_LIB_DIR ${GEANT4_CONFIG_OUTPUT})
    
    # I would like to do this, but it is not supported on cmake < 2.8.5
    # string(SUBSTRING ${GEANT4_LIB_DIR} 2 -1 GEANT4_LIB_DIR) # remove "-L"
    # so instead do this:
    string(LENGTH "${GEANT4_LIB_DIR}" GEANT4_LIB_DIR_length)
    math(EXPR GEANT4_LIB_DIR_length "${GEANT4_LIB_DIR_length}-2")
    string(SUBSTRING ${GEANT4_LIB_DIR} 2 ${GEANT4_LIB_DIR_length} GEANT4_LIB_DIR) # remove "-L"

    get_filename_component(GEANT4_LIB_DIR ${GEANT4_LIB_DIR} ABSOLUTE)
    if(I3_PORTS)
      string(REPLACE "${I3_PORTS}/" "" GEANT4_RELATIVE_LIB_DIR "${GEANT4_LIB_DIR}")
    else(I3_PORTS)
      set(GEANT4_RELATIVE_LIB_DIR ${GEANT4_LIB_DIR})
    endif(I3_PORTS)
    
    # remove the "-L" option from the argument list
    # and extract a list of libraries
    string(REGEX REPLACE "-[L]([^ ]*)" "" GEANT4_LIBRARIES ${GEANT4_CONFIG_OUTPUT})
    string(STRIP ${GEANT4_LIBRARIES} GEANT4_LIBRARIES)
    string(REGEX REPLACE "-l([^ ]*)" "${CMAKE_SHARED_LIBRARY_PREFIX}\\1${CMAKE_SHARED_LIBRARY_SUFFIX}" GEANT4_LIBRARIES ${GEANT4_LIBRARIES})
    separate_arguments(GEANT4_LIBRARIES)

    # extract the include path from geant4-config
    execute_process(COMMAND ${GEANT4_CONFIG} --cflags
      OUTPUT_VARIABLE GEANT4_CONFIG_OUTPUT OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    
    # look for the -I option
    string(REGEX MATCH "-I([^ ]*)" GEANT4_INC_DIR ${GEANT4_CONFIG_OUTPUT})

    # again, length "-1" is not supported on cmake < 2.8.5
    #string(SUBSTRING ${GEANT4_INC_DIR} 2 -1 GEANT4_INC_DIR) # remove "-I"
    string(LENGTH "${GEANT4_INC_DIR}" GEANT4_INC_DIR_length)
    math(EXPR GEANT4_INC_DIR_length "${GEANT4_INC_DIR_length}-2")
    string(SUBSTRING ${GEANT4_INC_DIR} 2 ${GEANT4_INC_DIR_length} GEANT4_INC_DIR) # remove "-I"
    
    get_filename_component(GEANT4_INC_DIR ${GEANT4_INC_DIR} ABSOLUTE)
    if(I3_PORTS)
      string(REPLACE "${I3_PORTS}/" "" GEANT4_RELATIVE_INC_DIR "${GEANT4_INC_DIR}")
    else(I3_PORTS)
      set(GEANT4_RELATIVE_INC_DIR ${GEANT4_INC_DIR})
    endif(I3_PORTS)

    # we got everything we need to define the tool properties
    tooldef (geant4
      ${GEANT4_RELATIVE_INC_DIR}
      G4RunManager.hh
      ${GEANT4_RELATIVE_LIB_DIR}
      NONE  # bin is n/a, placeholder
      ${GEANT4_LIBRARIES}
      )
      
    # (re-)define the "CLHEP" tool
    # (it's included with Geant4 now)
    unset(CLHEP_FOUND CACHE)
    unset(CLHEP_INCLUDE_DIR CACHE)
    unset(CLHEP_FOUND)
    unset(CLHEP_INCLUDE_DIR)
    
    tooldef (clhep
      ${GEANT4_RELATIVE_INC_DIR}
      CLHEP/Units/SystemOfUnits.h
      ${GEANT4_RELATIVE_LIB_DIR}
      NONE  # bin is n/a, placeholder
      )

    # no extra libraries for the geant4-internal version of clhep
    unset(CLHEP_LIBRARIES CACHE)
    unset(CLHEP_LIBRARIES)
    
endif (${GEANT4_CONFIG} MATCHES ".*NOTFOUND$")


if (NOT GEANT4_CONFIG_TOOL_FOUND)
  # there is no geant4-config tool. Try the "liblist" tool
  # installed by non-cmake Geant4 versions. (This will also
  # require CLHEP to be installed as a separate package.)

  if (CLHEP_CONFIG_ERROR)
    # we don't use the tooldef() macro, so we have to fudge pretty-printing
    colormsg("")
    colormsg(HICYAN "geant4")
  
    colormsg(CYAN "- CLHEP not found - Skipping Geant4")
    set(GEANT4_CONFIG_ERROR TRUE)
  else (CLHEP_CONFIG_ERROR)
    message(STATUS "Looking for Geant4 liblist program")

    find_program (GEANT4_LIBLIST liblist
      PATHS ${I3_PORTS}/lib
      PATH_SUFFIXES geant4_4.9.4 geant4_4.9.3
      NO_DEFAULT_PATH
      )

    if (${GEANT4_LIBLIST} MATCHES ".*NOTFOUND$")

      message(STATUS "Looking for Geant4 liblist program -- not found")
      set(GEANT4_CONFIG_ERROR TRUE)

    else (${GEANT4_LIBLIST} MATCHES ".*NOTFOUND$")

      message(STATUS "Looking for Geant4 liblist program -- found")

      get_filename_component(GEANT4_LIB_DIR ${GEANT4_LIBLIST} PATH)
      string (REPLACE "${I3_PORTS}/" "" GEANT4_RELATIVE_LIB_DIR "${GEANT4_LIB_DIR}")

      execute_process(COMMAND cat ${GEANT4_LIB_DIR}/libname.map
        COMMAND ${GEANT4_LIBLIST} -m ${GEANT4_LIB_DIR}
        OUTPUT_VARIABLE LIBLIST_OUTPUT OUTPUT_STRIP_TRAILING_WHITESPACE
        )
      string(REGEX REPLACE "-l([^ ]*)" "${CMAKE_SHARED_LIBRARY_PREFIX}\\1${CMAKE_SHARED_LIBRARY_SUFFIX}" GEANT4_LIBRARIES ${LIBLIST_OUTPUT})
      separate_arguments(GEANT4_LIBRARIES)

      find_path (GEANT4_INC_DIR NAMES G4RunManager.hh
        PATHS ${I3_PORTS}/include
        PATH_SUFFIXES geant4_4.9.4 geant4_4.9.3
        NO_DEFAULT_PATH
        )
      string (REPLACE "${I3_PORTS}/" "" GEANT4_RELATIVE_INC_DIR "${GEANT4_INC_DIR}")

      tooldef (geant4
        ${GEANT4_RELATIVE_INC_DIR}
        G4RunManager.hh
        ${GEANT4_RELATIVE_LIB_DIR}
        NONE  # bin is n/a, placeholder
        ${GEANT4_LIBRARIES}
        )

    endif (${GEANT4_LIBLIST} MATCHES ".*NOTFOUND$")

  endif (CLHEP_CONFIG_ERROR)

endif (NOT GEANT4_CONFIG_TOOL_FOUND)
