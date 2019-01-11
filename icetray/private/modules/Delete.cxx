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
#include <icetray/I3Tray.h>
#include <icetray/I3TrayInfo.h>
#include <icetray/I3TrayInfoService.h>
#include <icetray/Utility.h>

#include <icetray/modules/Delete.h>

#include <boost/regex.hpp>

using namespace std;

I3_MODULE(Delete);

Delete::Delete(const I3Context& ctx) : 
  I3Module(ctx)
{
  AddParameter("Keys", 
	       "Delete keys that match any of the regular expressions in this vector", 
	       delete_keys_);
  AddOutBox("OutBox");
}

void Delete::Configure()
{
  GetParameter("Keys", delete_keys_);
}

void Delete::Process()
{
  I3FramePtr frame = PopFrame();
  do_delete(frame);
  PushFrame(frame, "OutBox");
}

void Delete::Finish()
{
}

void Delete::do_delete(I3FramePtr frame)
{
  for (vector<string>::const_iterator iter = delete_keys_.begin();
       iter != delete_keys_.end();
       iter++)
    {
      frame->Delete(*iter);
    }
}
