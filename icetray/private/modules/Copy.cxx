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
#include <boost/regex.hpp>

#include <fstream>
#include <string>
#include <set>

#include "icetray/I3Tray.h"
#include "icetray/I3TrayInfo.h"
#include "icetray/I3TrayInfoService.h"
#include "icetray/Utility.h"
#include "icetray/I3ConditionalModule.h"
#include "icetray/I3TrayHeaders.h"
#include "icetray/I3Logging.h"

/**
 *  Copyies things from frame.  Has special privileges granted by I3Frame.
 */
class Copy : public I3ConditionalModule
{
  Copy();
  Copy(const Copy&);

  Copy& operator=(const Copy&);

  std::vector<std::string> copy_keys_;

public:

  Copy(const I3Context& ctx);
  virtual ~Copy() { }

  void Configure();

  void Process();

  void Finish();

  SET_LOGGER("Copy");
};

using namespace std;

I3_MODULE(Copy);

Copy::Copy(const I3Context& ctx) : 
  I3ConditionalModule(ctx)
{
  AddParameter("Keys", 
	       "pairs of keys [src_1, dst_1, src_2, dst_2,... src_n, dst_n]",
	       copy_keys_);
}

void Copy::Configure()
{
  GetParameter("Keys", copy_keys_);
  if (copy_keys_.size() % 2 != 0)
    log_fatal("odd number of params.  Need src,dst pairs.");
}

void Copy::Process()
{
  I3FramePtr frame = PopFrame();
  for (unsigned i=0; i<copy_keys_.size(); i+=2)
    {
      frame->Put(copy_keys_[i+1], 
		 frame->Get<I3FrameObjectConstPtr>(copy_keys_[i]));
    }
  PushFrame(frame);
}

void Copy::Finish()
{
}

