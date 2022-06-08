#
#  $Id$
#  
#  Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#  
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
#  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
#  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
#  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
#  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#  SUCH DAMAGE.
#  
#  SPDX-License-Identifier: BSD-2-Clause
#  
#  

tooldef(healpix-cxx
    include
    healpix_cxx/healpix_map.h
    lib
    NONE
    healpix_cxx 
)

if(HEALPIX-CXX_FOUND)
    if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
        list(APPEND HEALPIX-CXX_LIBRARIES "-fopenmp")
    endif()
endif()

if(NOT HEALPIX-CXX_FOUND)
    execute_process(COMMAND lsb_release -si OUTPUT_VARIABLE DISTRO ERROR_QUIET)
    if(DISTRO MATCHES "Fedora")
        tooldef(healpix-cxx
	    include
	    healpix/healpix_map.h
	    lib
	    NONE
	    healpix_cxx)
        if(HEALPIX-CXX_FOUND)
	    set(HEALPIX_CXX_ON_FEDORA True)
            list(APPEND HEALPIX-CXX_LIBRARIES "${CFITSIO_LIBRARIES}" "-lhealpix_cxxsupport" "-lhealpix_fft")
	endif()
    endif(DISTRO MATCHES "Fedora")
endif(NOT HEALPIX-CXX_FOUND)
