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

tooldef(healpix-cxx
    include
    healpix_cxx/healpix_map.h
    lib
    NONE
    healpix_cxx 
)

if(NOT HEALPIX-CXX_FOUND)
    execute_process(COMMAND lsb_release -si OUTPUT_VARIABLE DISTRO ERROR_QUIET)
    if(DISTRO MATCHES "Fedora")
        tooldef(healpix-cxx
	    include
	    healpix_cxx/healpix_map.h
	    lib
	    NONE
	    healpix_cxx 
    )
endif(DISTRO MATCHES "Fedora")
