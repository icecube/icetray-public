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
#include "icetray/modules/CountObject.h"
#include "icetray/Utility.h"
#include "icetray/I3Frame.h"

#include <boost/assign/list_inserter.hpp>
#include <boost/foreach.hpp>

using namespace boost::assign;

using namespace std;

I3_MODULE(CountObject);

CountObject::CountObject(const I3Context& context) : 
  I3Module(context),
  where_(),
  expected_(0),
  seen_(0)
{
  AddParameter("where", "where to count frame objects", where_);
  AddParameter("expected", "expected count of objects at this location", expected_);
}

void 
CountObject::Configure()
{
  GetParameter("where", where_);
  GetParameter("expected", expected_);
}

void CountObject::Physics(I3FramePtr frame)
{
  if (frame->Has(where_)) seen_++;
  PushFrame(frame);
}

void CountObject::DetectorStatus(I3FramePtr frame)
{
  if (frame->Has(where_)) seen_++;
  PushFrame(frame);
}

void CountObject::Geometry(I3FramePtr frame)
{
  if (frame->Has(where_)) seen_++;
  PushFrame(frame);
}

void CountObject::Calibration(I3FramePtr frame)
{
  if (frame->Has(where_)) seen_++;
  PushFrame(frame);
}

void
CountObject::Finish()
{
  if (expected_ != seen_)
    log_fatal("Module %s ecounting objects at slot '%s' expected to see %u but saw only %u",
	      GetName().c_str(), where_.c_str(), expected_, seen_);
}

CountObject::~CountObject()
{
}

