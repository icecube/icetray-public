/**
 *  $Id$
 *  
 *  Copyright (C) 2007,8,9   Troy D. Straszheim  <troy@icecube.umd.edu>
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

#include <iostream>
#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <icetray/I3ConditionalModule.h>

using namespace std;

class Dump : public I3ConditionalModule
{
 public:
  Dump(const I3Context& context)  : I3ConditionalModule(context),
				    frameCount_(0u)
  {
    AddOutBox("OutBox");
  }

  virtual ~Dump() { }

  void Configure(){ }

  void Process(){
    I3FramePtr frame = PopFrame();
    if (!frame)
      return;

    frameCount_++;
    cout << "------------------------- Frame #" << frameCount_ << " -------------------------\n" << *frame;
    PushFrame(frame,"OutBox");
  }

 private:

  uint64_t frameCount_;
  
  Dump(const Dump&);
  Dump& operator=(const Dump&);
};

I3_MODULE(Dump);

