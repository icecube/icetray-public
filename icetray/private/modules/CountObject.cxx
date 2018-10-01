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

