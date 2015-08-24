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
  AddOutBox("OutBox");
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
  PushFrame(frame, "OutBox");
}

void Copy::Finish()
{
}

