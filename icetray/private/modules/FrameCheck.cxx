/**
 *  $Id$
 *  
 *  Copyright (C) 2007   
 *  John Pretz  
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
  AddOutBox("OutBox");

  insert(keys_)
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
	log_fatal("Frame on stream %s doesn't contain key %s, which it must.",
		  stream.str().c_str(), key.c_str());
    }

  BOOST_FOREACH(string key, keys_[stream].hasnt)
    {
      if(frame->Has(key))
	log_fatal("Frame on stream %s contains key %s, which it must not.",
		  stream.str().c_str(), key.c_str());
    }


}
