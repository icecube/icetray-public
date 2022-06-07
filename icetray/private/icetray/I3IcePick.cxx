/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  John Pretz
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
#include <icetray/I3IcePick.h>
#include <icetray/I3Bool.h>

I3IcePick::I3IcePick(const I3Context& context) :
  I3ServiceBase(context), cache_(false)
{
  AddParameter("CacheResults",
	       "For each frame evaluated, write the result to the frame, and "
	       "check to see if that result exists before evaluating again.",
	       cache_);
//
//  Too early for harsh warnings.  Would be nice to see an orderly exit from C++ Icepicks where 
//	practical, it's likely needed in some places for a bit longer.  Also need full implementation
//	of pypicks from c++ versions.
//
//  log_warn("\n**\n"
//           "**    The IcePick system is DEPRECATED and will be removed in the next\n"
//           "**    offline-software release. All of the functionality of IcePick can\n"
//           "**    be replaced with Python functions, and drop-in replacements for many\n"
//           "**    of the former IcePicks are provided in icecube.icepick module.\n"
//           "**    If you believe that there is no suitable replacement for your favorite\n"
//           "**    IcePick, please email dataclass@icecube.wisc.edu or the appropriate\n"
//           "**    meta-project mailing list.  You can also file a bug report at\n"
//           "**    http://code.icecube.wisc.edu.\n"
//           "**\n");

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

