/**
 *  $Id$
 *  
 *  Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
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
#include <icetray/I3Module.h>
#include <icetray/I3Frame.h>
#include <icetray/Utility.h>

#include <map>

/**
 * @brief An icetray module which will check whether a configurable
 * set of frame elements is (or is not) present on each stream.
 */
class CountFrames : public I3Module
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
  I3Module(context)
{
  AddOutBox("OutBox");

  insert(required_framecounts_)
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
