/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
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
