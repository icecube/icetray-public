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
#include <boost/regex.hpp>

#include <icetray/I3Tray.h>
#include <icetray/I3TrayInfo.h>
#include <icetray/I3TrayInfoService.h>
#include <icetray/Utility.h>
#include <icetray/modules/Rename.h>

using namespace std;

I3_MODULE(Rename);

Rename::Rename(const I3Context& ctx) : 
  I3ConditionalModule(ctx)
{
  AddParameter("Keys", 
	       "Rename objects in frame by name: [From1, To1, From2, To2 ... FromN, ToN]", 
	       rename_keys_);
}

void Rename::Configure()
{
  GetParameter("Keys", rename_keys_);
  if (rename_keys_.size() % 2)
    log_fatal("Rename takes an even number of Keys:\n"
	      "From_1, To_2, From_2, To_2, ... From_N, To_N");
}

void Rename::Process()
{
  I3FramePtr frame = PopFrame();
  do_rename(frame);
  PushFrame(frame);
}

void Rename::Finish()
{
}

void Rename::do_rename(I3FramePtr frame)
{
  for (vector<string>::const_iterator iter = rename_keys_.begin();
       iter != rename_keys_.end();
       iter++, iter++)
    {
      const std::string& from = *iter;
      const std::string& to = *(iter+1);
      
      if (!frame->Has(from))
	continue;
      if (frame->Has(to))
	log_fatal("Can't rename frame item '%s' to '%s' since the destination is full",
		  from.c_str(),  to.c_str());
      frame->Rename(from, to);
    }
}
