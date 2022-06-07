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
