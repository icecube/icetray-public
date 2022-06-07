/**
 *  $Id$
 *  
 *  Copyright (C) 2007   
 *  John Pretz  
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
#include "icetray/modules/FrameCheck.h"
#include "icetray/Utility.h"
#include "icetray/I3Frame.h"

#include <boost/assign/list_inserter.hpp>
#include <boost/foreach.hpp>

using namespace boost::assign;

using namespace std;

I3_MODULE(FrameCheck);

FrameCheck::FrameCheck(const I3Context& context) : 
  I3Module(context)
{
  insert(keys_)
    (I3Frame::DAQ, predicates())
    (I3Frame::Physics, predicates())
    (I3Frame::Geometry, predicates())
    (I3Frame::DetectorStatus, predicates())
    (I3Frame::Calibration, predicates());
    
  string ensure = "Ensure_";
  string has = "_Has";
  string hasnt = "_Hasnt";
      
  for (streammap_t::const_iterator iter = keys_.begin();
       iter != keys_.end();
       iter++)
    {
      string name = iter->first.str();
      AddParameter(ensure + name + has,
		   "Ensure stream " + name + " has these keys",
		   vector<string>());

      AddParameter(ensure + name + hasnt,
		   "Ensure stream " + name + " does not have these keys",
		   vector<string>());
    }
}

FrameCheck::~FrameCheck()
{
}

void 
FrameCheck::Configure()
{
  string ensure = "Ensure_";
  string has = "_Has";
  string hasnt = "_Hasnt";
      
  for (streammap_t::iterator iter = keys_.begin();
       iter != keys_.end();
       iter++)
    {
      string name = iter->first.str();
      GetParameter(ensure + name + has,
		   iter->second.has);

      GetParameter(ensure + name + hasnt,
		   iter->second.hasnt);

    }
}

void FrameCheck::Physics(I3FramePtr frame)
{
  CheckFrameForKeys(frame, I3Frame::Physics);
  PushFrame(frame,"OutBox");
}

void FrameCheck::DAQ(I3FramePtr frame)
{
  CheckFrameForKeys(frame, I3Frame::DAQ);
  PushFrame(frame,"OutBox");
}

void FrameCheck::DetectorStatus(I3FramePtr frame)
{
  CheckFrameForKeys(frame, I3Frame::DetectorStatus);
  PushFrame(frame,"OutBox");
}

void FrameCheck::Geometry(I3FramePtr frame)
{
  CheckFrameForKeys(frame, I3Frame::Geometry);
  PushFrame(frame,"OutBox");
}

void FrameCheck::Calibration(I3FramePtr frame)
{
  CheckFrameForKeys(frame, I3Frame::Calibration);
  PushFrame(frame,"OutBox");
}

void FrameCheck::CheckFrameForKeys(I3FramePtr frame,
				   const I3Frame::Stream& stream)
{

  BOOST_FOREACH(string key, keys_[stream].has)
    {
      if(!frame->Has(key))
	log_fatal("%s: Frame on stream %s doesn't contain key %s, which it must.",
		  GetName().c_str(), stream.str().c_str(), key.c_str());
    }

  BOOST_FOREACH(string key, keys_[stream].hasnt)
    {
      if(frame->Has(key))
	log_fatal("%s: Frame on stream %s contains key %s, which it must not.",
		  GetName().c_str(), stream.str().c_str(), key.c_str());
    }


}
