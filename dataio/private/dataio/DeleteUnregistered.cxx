/**
 *  $Id$
 *  
 *  Copyright (C) 2007, 2008, 2009
 *  Troy Straszheim <troy@icecube.umd.edu>
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
 
#include <icetray/I3Context.h>
#include <icetray/I3DefaultName.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Logging.h>
#include <icetray/I3Module.h>
#include <archive/archive_exception.hpp>

using namespace std;

class DeleteUnregistered : public I3Module
{
 public:

  DeleteUnregistered(const I3Context& context);

  virtual ~DeleteUnregistered();
  void Process();

 private:

  DeleteUnregistered(const DeleteUnregistered&);
  DeleteUnregistered& operator=(const DeleteUnregistered&);

  SET_LOGGER("DeleteUnregistered");
};


I3_MODULE(DeleteUnregistered);


DeleteUnregistered::DeleteUnregistered(const I3Context& context)
  : I3Module(context)
{}

DeleteUnregistered::~DeleteUnregistered() 
{ }

void DeleteUnregistered::Process()
{
  I3FramePtr frame = PopFrame();
  
  vector<string> deleteme;
  for (I3Frame::typename_iterator iter = frame->typename_begin();
       iter != frame->typename_end();
       iter++){
    I3FrameObjectConstPtr fop = frame->Get<I3FrameObjectConstPtr>(iter->first);
    if(!fop && frame->Has(iter->first))
       deleteme.push_back(iter->first);
  }
  for (unsigned i=0; i<deleteme.size(); i++)
    frame->Delete(deleteme[i]);
  PushFrame(frame); 
}


