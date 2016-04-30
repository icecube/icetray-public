#
#  cmake/tools/minuit2.cmake
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

set(MINUIT2_FOUND FALSE)

# search for ROOT's Minuit2
if(ROOT_FOUND)
    if(${ROOT_VERSION} GREATER 5.29)
        TOOLDEF(minuit2
            ${ROOTSYS}/include
            Minuit2/MnConfig.h
            ${ROOTSYS}/lib
            NONE # bin is n/a, placeholder
            Minuit2
            )
    else(${ROOT_VERSION} GREATER 5.29)
        message(STATUS "+ minuit2 disabled: ROOT version less 5.30 detected")
    endif(${ROOT_VERSION} GREATER 5.29)
endif(ROOT_FOUND)

# search for the system installed Minuit2
if(NOT MINUIT2_FOUND)
    unset(minuit2_INCLUDE_DIR)
    TOOLDEF(minuit2
        include/Minuit2
        Minuit2/MnConfig.h
        lib/Minuit2
        NONE # bin is n/a, placeholder
        Minuit2
    )
endif(NOT MINUIT2_FOUND)
