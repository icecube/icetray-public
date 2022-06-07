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
#ifndef ICETRAY_I3PHYSICSTIMER_H_INCLUDED
#define ICETRAY_I3PHYSICSTIMER_H_INCLUDED

#include <sys/time.h>
#include <sys/resource.h>

#include <string>
#include <icetray/I3Frame.h>
#include <icetray/I3PointerTypedefs.h>

struct I3RUsage : public I3FrameObject 
{
  // in I3Units of time.
  double systemtime, usertime, wallclocktime;

  template <typename Archive>
  void 
  serialize(Archive &ar, unsigned version);
};
  
I3_POINTER_TYPEDEFS(I3RUsage);

class I3PhysicsTimer
{
  struct rusage start_rusage_, end_rusage_;
  struct timeval start_wallclock_, end_wallclock_;

  I3FramePtr frame_;
  std::string module_name_;

  double 
  delta(const struct timeval &starttime, const struct timeval& stoptime);

public:

  I3PhysicsTimer(I3FramePtr frame, const std::string& module_name);
  ~I3PhysicsTimer();

};
  


#endif
