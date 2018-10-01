/* 
   DANGER:  caching results

   pick keeps the address of the previous frame it saw

   it is a frameobject filter

   tray branches
   first time, object is not there

   second time, it already passsed a module on a diferent banch that put the frameobject there

   get incorrect result 
*/

/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  John Pretz
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
#include <icetray/I3IcePick.h>
#include <icetray/I3Bool.h>

I3IcePick::I3IcePick(const I3Context& context) :
  context_(context),
  cache_(false)
{
  AddParameter("CacheResults",
	       "For each frame evaluated, write the result to the frame, and "
	       "check to see if that result exists before evaluating again.",
	       cache_);
}

I3IcePick::~I3IcePick()
{
}

void
I3IcePick::ConfigureInterface()
{
  GetParameter("CacheResults",cache_);
  cachename_ = GetConfiguration().InstanceName() + "_cache";
  Configure();
}

void
I3IcePick::Configure()
{
}

bool
I3IcePick::SelectFrameInterface(I3Frame& frame)
{
  if(cache_)
    {
      I3BoolConstPtr done = frame.Get<I3BoolConstPtr>(cachename_);
      if(!done)
	{
	  I3BoolPtr answer(new I3Bool(SelectFrame(frame)));
	  frame.Put(cachename_,answer);
	  if(answer->value) 
	    ++npassed_;
	  else 
	    ++nfailed_;
	  return answer->value;
	}
      else
	{
	  return done->value;
	}
    }
  else
    {
      bool answer = SelectFrame(frame);
      if(answer) 
	++npassed_;
      else 
	++nfailed_;
      return answer;
    }
}

I3Configuration& 
I3IcePick::GetConfiguration()
{
    return context_.Get<I3Configuration>();
}

const I3Configuration& 
I3IcePick::GetConfiguration() const
{
    return context_.Get<I3Configuration>();
}
