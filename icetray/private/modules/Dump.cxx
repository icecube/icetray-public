/**
 *  $Id$
 *  
 *  Copyright (C) 2007,8,9   Troy D. Straszheim  <troy@icecube.umd.edu>
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
  {}

  virtual ~Dump() { }

  void Process(){
    I3FramePtr frame = PopFrame();
    if (!frame)
      log_fatal("Dump is not a driving module!");

    frameCount_++;
    cout << "------------------------- This is frame number " << frameCount_ << " -------------------------\n" << *frame;
    PushFrame(frame,"OutBox");
  }

 private:

  uint64_t frameCount_;
  
  Dump(const Dump&);
  Dump& operator=(const Dump&);
};

I3_MODULE(Dump);

