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
#include <icetray/I3Int.h>
#include <icetray/I3Module.h>
#include <icetray/I3Frame.h>

// creates sequential ints and sends them downstream
class IntGenerator : public I3Module
{
  int i;

public:

  IntGenerator(const I3Context& context) : I3Module(context)
  {      
    log_trace("%s", __PRETTY_FUNCTION__);
    AddOutBox("OutBox");
    i = 0;
  }

  void Process()
  {
    I3FramePtr frame(new I3Frame(I3Frame::Physics));
    boost::shared_ptr<I3Int> data(new I3Int(++i));
    frame->Put("myint", data);
    PushFrame(frame,"OutBox");
  }
};

I3_MODULE(IntGenerator);

