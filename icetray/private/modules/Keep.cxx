/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Dr. Torsten Schmidt <hightech-consulting@gmx.de>
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
 
// class header file
#include <icetray/modules/Keep.h>
I3_MODULE(Keep);

#include <icetray/I3Context.h>
#include <icetray/I3DefaultName.h>

// namespace declarations

using namespace std;

// implementation

Keep::Keep(const I3Context& context) 
  : I3ConditionalModule(context)
{
  keysParam_.push_back("I3Calibration");
  keysParam_.push_back("I3DetectorStatus");
  keysParam_.push_back("I3Geometry");
  keysParam_.push_back("DrivingTime");
  keysParam_.push_back("I3EventHeader");
  AddParameter("Keys", 
               "Keep frame objects with names that match any of these keys", 
               keysParam_);

  AddOutBox("OutBox");
}


Keep::~Keep()
{
}


void Keep::Configure()
{
  GetParameter("Keys", keysParam_);
  set<string> tmp(keysParam_.begin(), keysParam_.end());
  keys_.swap(tmp);
}


void Keep::Process()
{
  I3FramePtr frame = PopFrame();
  I3FramePtr newFrame(new I3Frame(frame->GetStop()));

  for(set<string>::const_iterator iter = keys_.begin(); iter != keys_.end(); ++iter)
  {
    I3Frame::typename_iterator jter = frame->typename_find(*iter);
    if(jter != frame->typename_end()) newFrame->take(*frame, *iter);
  }
 
  PushFrame(newFrame, "OutBox"); 
}
