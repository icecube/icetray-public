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

  message(STATUS "CLHEP not found - Skipping Geant4")
  set(GEANT4_CONFIG_ERROR TRUE)

else (CLHEP_CONFIG_ERROR)

  find_path (GEANT4_LIB_DIR NAMES libG4event.so
    PATHS ${I3_PORTS}/lib
    PATH_SUFFIXES geant4_4.9.3
    NO_DEFAULT_PATH
    )
  string (REPLACE "${I3_PORTS}/" "" GEANT4_RELATIVE_LIB_DIR "${GEANT4_LIB_DIR}")
  
  file(GLOB GEANT4_LIBRARIES RELATIVE "${GEANT4_LIB_DIR}" "${GEANT4_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}G4*${CMAKE_SHARED_LIBRARY_SUFFIX}")
  separate_arguments(GEANT4_LIBRARIES)

  find_path (GEANT4_INC_DIR NAMES G4RunManager.hh
    PATHS ${I3_PORTS}/include
    PATH_SUFFIXES geant4_4.9.3
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

endif (CLHEP_CONFIG_ERROR)
