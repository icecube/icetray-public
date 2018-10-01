/**
 *  $Id$
 *
 *  Copyright (C) 2016  Claudio Kopper <ckopper@icecube.wisc.edu>
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
