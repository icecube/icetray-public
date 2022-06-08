#
#  cmake/tools/minuit2.cmake
#
#  Copyright (C) 2010
#  the IceCube Collaboration <http://www.icecube.wisc.edu>
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



if(USE_ROOT)
    # search for ROOT's Minuit2
    TOOLDEF(minuit2
       ${ROOTSYS}/include
       Minuit2/MnConfig.h
       ${ROOTSYS}/lib
       NONE # bin is n/a, placeholder
       Minuit2
       )

    if(NOT MINUIT2_FOUND)
      TOOLDEF(minuit2
        ${ROOTSYS}/include/root
        Minuit2/MnConfig.h
        ${ROOTSYS}/lib/root
        NONE # bin is n/a, placeholder
        Minuit2
        )
    endif(NOT MINUIT2_FOUND)

    # search for the system installed Minuit2
    if(NOT MINUIT2_FOUND)
        unset(MINUIT2_INCLUDE_DIR)
        TOOLDEF(minuit2
            include/Minuit2
            Minuit2/MnConfig.h
            lib/Minuit2
            NONE # bin is n/a, placeholder
            Minuit2
        )
    endif(NOT MINUIT2_FOUND)

    if(ROOT_FOUND)
        if(${ROOT_VERSION} LESS 5.30)
            set(MINUIT2_FOUND FALSE)
            unset(MINUIT2_INCLUDE_DIR)
            message(STATUS "- Minuit2: ROOT < 5.30 not supported")
        endif(${ROOT_VERSION} LESS 5.30)
    endif(ROOT_FOUND)
else(USE_ROOT)
    message(STATUS "- Minuit2: USE_ROOT set to FALSE")
endif(USE_ROOT)
