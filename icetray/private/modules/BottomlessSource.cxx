/**
 *  $Id$
 *  
 *  Copyright (C) 2004-8
 *  The IceCube Collaboration <http://www.icecube.wisc.edu>
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

struct BottomlessSource : public I3Module
{
  BottomlessSource(const I3Context& context)  : I3Module(context)
  {
    AddParameter("Stream", "Frame type to generate", I3Frame::Physics);
    AddOutBox("OutBox");
  }

  void Configure()
  {
    GetParameter("Stream", stream_);
  }

  void Process()
  {
    log_trace("%s: %s", GetName().c_str(), __PRETTY_FUNCTION__);
    I3FramePtr frame(new I3Frame(stream_));
    PushFrame(frame);
  }

  I3Frame::Stream stream_;
};

I3_MODULE(BottomlessSource);

