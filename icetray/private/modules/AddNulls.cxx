/**
 *  $Id$
 *  
 *  Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
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

