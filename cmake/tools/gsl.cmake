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
  foreach(v 1.8 1.14 1.15)
    if(IS_DIRECTORY ${I3_PORTS}/include/gsl-${v})
      set(GSL_VERSION "${v}")
      set(GSL_SUBDIR "gsl-${v}")
    endif(IS_DIRECTORY ${I3_PORTS}/include/gsl-${v})
  endforeach(v 1.8 1.14 1.15)
ENDIF(NOT GSL_VERSION)

if(BLAS_FOUND)
  tooldef(gsl 
    include/${GSL_SUBDIR}
    gsl/gsl_rng.h
    lib/gsl-${GSL_VERSION}
    NONE
    gsl)
  list(APPEND GSL_LIBRARIES ${BLAS_LIBRARIES})
else(BLAS_FOUND)
  tooldef(gsl 
    include/${GSL_SUBDIR}
    gsl/gsl_rng.h
    lib/${GSL_SUBDIR}
    NONE
    gsl gslcblas)
endif(BLAS_FOUND)
