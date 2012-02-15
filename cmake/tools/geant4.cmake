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
