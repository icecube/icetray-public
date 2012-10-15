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

IF(NOT HEALPIX_VERSION)
        if(IS_DIRECTORY ${I3_PORTS}/include/Healpix-2.20a )
                set(HEALPIX_VERSION "2.20a")
        endif(IS_DIRECTORY ${I3_PORTS}/include/Healpix-2.20a )
ENDIF(NOT HEALPIX_VERSION)

tooldef(healpix-cxx 
    include/
    healpix-cxx/healpix_map.h
    lib/
    NONE
    healpix_cxx 
    cxxsupport
    )
