/**
 *  $Id$
 *  
 *  Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
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
#include <icetray/I3ConditionalModule.h>
#include <icetray/I3Frame.h>
#include <icetray/Utility.h>

#include <map>

/**
 * @brief An icetray module which will check whether a configurable
 * set of frame elements is (or is not) present on each stream.
 */
class CountFrames : public I3ConditionalModule
{
 public:
  CountFrames(const I3Context& context);

  virtual ~CountFrames();
  
  void Configure();

  void Process();

  void Finish();
  
 private:

  std::map<I3Frame::Stream, unsigned> required_framecounts_;
  std::map<I3Frame::Stream, unsigned> actual_framecounts_;

};


#include <boost/assign/list_inserter.hpp>
using namespace boost::assign;
using namespace std;

I3_MODULE(CountFrames);

CountFrames::CountFrames(const I3Context& context) : 
  I3ConditionalModule(context)
{
  insert(required_framecounts_)
    (I3Frame::DAQ, 0)
    (I3Frame::Physics, 0)
    (I3Frame::Geometry, 0)
    (I3Frame::DetectorStatus, 0)
    (I3Frame::Calibration, 0);
    
  // initialize the actual framecounts
  actual_framecounts_ = required_framecounts_;

  for (map<I3Frame::Stream,unsigned>::const_iterator iter = required_framecounts_.begin();
       iter != required_framecounts_.end();
       iter++)
    {
      string name = iter->first.str();
      AddParameter(name,
		   "Ensure stream " + name + " occurs this many times",
		   iter->second);
    }
}

CountFrames::~CountFrames()
{
}

void 
CountFrames::Configure()
{
  for (map<I3Frame::Stream, unsigned>::iterator iter = required_framecounts_.begin();
       iter != required_framecounts_.end();
       iter++)
    {
      string name = iter->first.str();
      GetParameter(name, iter->second);
    }
}

void CountFrames::Process()
{
  I3FramePtr frame = PopFrame();

  actual_framecounts_[frame->GetStop()]++;

  PushFrame(frame,"OutBox");
}

void CountFrames::Finish()
{

  for (map<I3Frame::Stream,unsigned>::const_iterator iter = required_framecounts_.begin();
       iter != required_framecounts_.end();
       iter++)
    {
      if (actual_framecounts_[iter->first] != iter->second)
	log_fatal("On Stream %s, %u frames were required but %u were seen",
		  iter->first.str().c_str(), 
		  iter->second,
		  actual_framecounts_[iter->first]);
      log_trace("Confirmed %u frames seen on stream %s",
		iter->second, iter->first.str().c_str());
    }
}
