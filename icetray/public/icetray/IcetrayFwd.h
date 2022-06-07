/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *  
 *  SPDX-License-Identifier: BSD-2-Clause
 *  
 */

#ifndef ICETRAYFWD_H_INCLUDED
#define ICETRAYFWD_H_INCLUDED

#if defined(__APPLE__) && defined(__CINT__) && (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ > 1085)
// Workaround for CINT problems with stdint.h. Affects OS X >= 10.9; see
// https://root.cern.ch/phpBB3/viewtopic.php?f=5&t=17360
typedef char __signed;
#include <sys/_types/_int8_t.h>
#include <sys/_types/_int16_t.h>
#include <sys/_types/_int32_t.h>
#include <sys/_types/_int64_t.h>
#else
#include <stdint.h> //int64_t, etc
#endif

#include <icetray/I3Logging.h>
#include <icetray/I3PointerTypedefs.h>

class I3Module;  I3_POINTER_TYPEDEFS(I3Module);
class I3Context;  I3_POINTER_TYPEDEFS(I3Context);
class I3ServiceFactory; I3_POINTER_TYPEDEFS(I3ServiceFactory);
class I3Configuration; I3_POINTER_TYPEDEFS(I3Configuration);
class I3Frame; I3_POINTER_TYPEDEFS(I3Frame);

#endif 

