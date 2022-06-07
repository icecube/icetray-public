/**
 *  $Id$
 *
 *  Copyright (C) 2016  Claudio Kopper <ckopper@icecube.wisc.edu>
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

class CreateBlobs : public I3ConditionalModule
{
  bool dropMemoryData_;

 public:
  CreateBlobs(const I3Context& context)  : I3ConditionalModule(context)
  {
    AddParameter("DropMemoryData", "Drop the in-memory shared_ptr after creating the blob.", false);
  }

  virtual ~CreateBlobs() { }

  void Configure()
  {
    GetParameter("DropMemoryData", dropMemoryData_);
  }

  void Process(){
    I3FramePtr frame = PopFrame();
    if (!frame)
      log_fatal("CreateBlobs is not a driving module!");

    // Just force creation of all frame item blobs.
    // To the user this should look like a no-op.
    frame->create_blobs(dropMemoryData_);

    PushFrame(frame,"OutBox");
  }

 private:

  CreateBlobs(const CreateBlobs&);
  CreateBlobs& operator=(const CreateBlobs&);
};

I3_MODULE(CreateBlobs);
