/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Dr. Torsten Schmidt <hightech-consulting@gmx.de>
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
  AddParameter("Streams",
	       "A list of frame types which this function should fire on. "
	       "By default runs only on DAQ and physics frames",
	       std::vector<I3Frame::Stream>{I3Frame::DAQ,I3Frame::Physics});
}

Keep::~Keep(){}

void Keep::Configure()
{
  GetParameter("Keys", keysParam_);
  set<string> tmp(keysParam_.begin(), keysParam_.end());
  keys_.swap(tmp);
  
  std::vector<I3Frame::Stream> svec;
  GetParameter("Streams", svec);
  streams_ = std::set<I3Frame::Stream>(svec.begin(), svec.end());
}


void Keep::Process()
{
  log_trace("%s", "Processing");
  I3FramePtr frame = PopFrame();

  if(!streams_.count(frame->GetStop()))
    // Don't remove anything from this frame!
    PushFrame(frame);
  else {
    // Supposed to run here
    I3FramePtr newFrame(new I3Frame(frame->GetStop()));
    newFrame->drop_blobs(frame->drop_blobs());
    
    for(set<string>::const_iterator iter = keys_.begin(); iter != keys_.end(); ++iter){
      I3Frame::typename_iterator jter = frame->typename_find(*iter);
      if(jter != frame->typename_end()) newFrame->take(*frame, *iter);
    }
  
    PushFrame(newFrame, "OutBox"); 
  }
}
