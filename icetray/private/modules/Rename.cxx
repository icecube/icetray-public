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
