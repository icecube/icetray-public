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

#include <icetray/modules/Copy.h>

#include <boost/regex.hpp>

using namespace std;

I3_MODULE(Copy);

Copy::Copy(const I3Context& ctx) : 
  I3Module(ctx)
{
  AddParameter("Keys", 
	       "pairs of keys [src_1, dst_1, src_2, dst_2,... src_n, dst_n]",
	       copy_keys_);
  AddOutBox("OutBox");
}

void Copy::Configure()
{
  GetParameter("Keys", copy_keys_);
  if (copy_keys_.size() % 2 != 0)
    log_fatal("odd number of params.  Need src,dst pairs.");
}

void Copy::Process()
{
  I3FramePtr frame = PopFrame();
  do_copy(frame);
  PushFrame(frame, "OutBox");
}

void Copy::Finish()
{
}

void Copy::do_copy(I3FramePtr frame)
{
  for (unsigned i=0; i<copy_keys_.size(); i+=2)
    {
      frame->Put(copy_keys_[i+1], 
		 frame->Get<I3FrameObjectConstPtr>(copy_keys_[i]));
    }
}
