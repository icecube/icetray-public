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
#include <icetray/I3Module.h>
#include <icetray/Utility.h>
#include <icetray/I3Frame.h>

#include <string>
#include <set>

class Fork : public I3Module
{
  std::vector<std::string> outboxes_;
  
public:

  Fork (const I3Context& context);
  void Configure();
  void Process();

};

I3_MODULE(Fork);

using namespace std;

Fork::Fork(const I3Context& context) : I3Module(context)
{
  AddParameter("Outboxes", "List of outboxes", outboxes_);
}

void
Fork::Configure()
{
  GetParameter("Outboxes", outboxes_);

  for (vector<string>::const_iterator iter = outboxes_.begin();
       iter != outboxes_.end(); iter++)
    {
      AddOutBox(*iter);
    }
}

void 
Fork::Process()
{
  I3FramePtr frame = PopFrame();

  for (vector<string>::const_iterator iter = outboxes_.begin();
       iter != outboxes_.end(); iter++)
    {
      I3FramePtr newframe(new I3Frame(*frame));
			  
      PushFrame(newframe, *iter);
    }
}

