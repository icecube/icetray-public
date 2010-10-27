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

IF(NOT GSL_VERSION)
	if(IS_DIRECTORY ${I3_PORTS}/include/gsl-1.14)
		set(GSL_VERSION "1.14")
	else(IS_DIRECTORY ${I3_PORTS}/include/gsl-1.14)
		set(GSL_VERSION "1.8")
	endif(IS_DIRECTORY ${I3_PORTS}/include/gsl-1.14)
ENDIF(NOT GSL_VERSION)

tooldef(gsl 
    include/gsl-${GSL_VERSION}
    gsl/gsl_rng.h
    lib/gsl-${GSL_VERSION}
    NONE
    gsl gslcblas
    )
