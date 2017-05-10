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
#include <icetray/modules/PhysicsBuffer.h>
#include <icetray/Utility.h>

I3_MODULE(PhysicsBuffer);

using namespace std;

PhysicsBuffer::PhysicsBuffer(const I3Context& context) 
  : I3Module(context),
    desired_buffersize_(0)
{
  AddParameter("buffersize", "number frames to buffer", desired_buffersize_);
  batchpush_n_ = 13;
  AddParameter("batchpush", "batch-push every N events", batchpush_n_);
  n_ = 0;
}

void
PhysicsBuffer::Configure()
{
  GetParameter("buffersize", desired_buffersize_);
  GetParameter("batchpush", batchpush_n_);
}

// NOTE THAT THIS MODULE WONT NOTICE IF GEOMETRIES FLY PAST, IT SIMPLY
// BUFFERS Physics()...  YOU WOULD PROBABLY WANT TO WATCH FOR
// Geo/Cal/Status, AND IF THEY APPEAR, FLUSH YOUR PHYSICS BUFFER AS IN
// Finish(), BELOW
void 
PhysicsBuffer::Physics(I3FramePtr frame)
{
  n_++;
  buffer_.push_front(frame);

  log_trace("n_=%d batchpush_n_=%d", n_, batchpush_n_);
  if ((n_ % batchpush_n_) == 0)
    {
      log_trace("doing batchpush @ event %d", n_);
      while (buffer_.size() > desired_buffersize_)
	{
	  PushFrame(buffer_.back());
	  buffer_.pop_back();
	}
    }
}

void
PhysicsBuffer::Finish()
{
  // push all remaining frames
  while (buffer_.size())
    {
      PushFrame(buffer_.back());
      buffer_.pop_back();
    }
  Flush();
}
