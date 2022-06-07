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
#include <icetray/serialization.h>
#include <icetray/I3PhysicsTimer.h>
#include <icetray/I3Units.h>
#include <icetray/I3Frame.h>

template <typename Archive>
void
I3RUsage::serialize(Archive &ar, unsigned version)
{
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("system", systemtime);
  ar & make_nvp("user", usertime);
  ar & make_nvp("wallclock", wallclocktime);
}

I3_SERIALIZABLE(I3RUsage);

using namespace std;

I3PhysicsTimer::I3PhysicsTimer(I3FramePtr frame, const string& module_name)
  : frame_(frame), module_name_(module_name)
{
  getrusage(RUSAGE_SELF, &start_rusage_);
  gettimeofday(&start_wallclock_, 0);
}

double
I3PhysicsTimer::delta(const struct timeval &starttime, const struct timeval& stoptime)
{
  double start = (double) starttime.tv_sec * I3Units::second + (double)starttime.tv_usec * I3Units::microsecond;
//  log_trace("start %lu %lu", starttime.tv_sec, starttime.tv_usec);

  double stop  = (double) stoptime.tv_sec * I3Units::second + (double)stoptime.tv_usec * I3Units::microsecond; 
//  log_trace("end %lu %lu", stoptime.tv_sec, stoptime.tv_usec);
  log_trace("asdouble %lf %lf", start, stop);

  return stop - start;
}

I3PhysicsTimer::~I3PhysicsTimer()
{
  getrusage (RUSAGE_SELF, &end_rusage_);
  gettimeofday (&end_wallclock_, 0);

  I3RUsagePtr rup(new I3RUsage);
  rup->systemtime = delta (start_rusage_.ru_stime, end_rusage_.ru_stime);
  rup->usertime = delta (start_rusage_.ru_utime, end_rusage_.ru_utime);

  rup->wallclocktime = delta (start_wallclock_, end_wallclock_);

  frame_->Put(module_name_ + "_rusage", rup);
  log_trace("Elapsed: %lf", rup->wallclocktime);
}
