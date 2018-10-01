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

#include <icetray/Utility.h>
#include <icetray/I3Frame.h>
#include <icetray/I3ConditionalModule.h>

#include <string>
#include <vector>

class AddNulls : public I3ConditionalModule
{
  std::vector<std::string> keys_;
  
public:

  AddNulls (const I3Context& context);
  void Configure();
  void Physics(I3FramePtr frame);

};

using namespace std;

AddNulls::AddNulls(const I3Context& context) : I3ConditionalModule(context)
{
  AddParameter("where", "vector of keys:  null pointer gets added to each", keys_);
}

void
AddNulls::Configure()
{
  GetParameter("where", keys_);
}

void 
AddNulls::Physics(I3FramePtr frame)
{
  for (vector<string>::const_iterator iter = keys_.begin();
       iter != keys_.end(); 
       iter++)
    {
      frame->Put(*iter, I3FrameObjectPtr());
    }
  PushFrame(frame);
}
// Test, test, 1, 2, 3. Is this thing on?

I3_MODULE(AddNulls);

