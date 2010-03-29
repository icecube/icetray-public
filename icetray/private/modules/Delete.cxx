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

#include <fstream>
#include <string>
#include <set>
#include <icetray/I3ConditionalModule.h>
#include <icetray/I3TrayHeaders.h>
#include <icetray/I3Logging.h>

#include <icetray/I3Tray.h>
#include <icetray/I3TrayInfo.h>
#include <icetray/I3TrayInfoService.h>
#include <icetray/Utility.h>

#include <boost/regex.hpp>

/**
 *  Deletes things from frame.  Has special privileges granted by I3Frame.
 */
class Delete : public I3ConditionalModule
{
  Delete();
  Delete(const Delete&);

  Delete& operator=(const Delete&);

  std::vector<std::string> delete_keys_;

  public:

  Delete(const I3Context& ctx);

  virtual ~Delete() { }

  void Configure();

  void Process();

  void Finish();

  SET_LOGGER("Delete");
};


using namespace std;

I3_MODULE(Delete);

Delete::Delete(const I3Context& ctx) : 
  I3ConditionalModule(ctx)
{
  AddParameter("Keys", 
	       "Delete keys that match any of the regular expressions in this vector", 
	       delete_keys_);
  AddOutBox("OutBox");
}

void Delete::Configure()
{
  GetParameter("Keys", delete_keys_);
}

void Delete::Process()
{
  I3FramePtr frame = PopFrame();

  for (vector<string>::const_iterator iter = delete_keys_.begin();
       iter != delete_keys_.end();
       iter++)
    {
      frame->Delete(*iter);
    }

  PushFrame(frame, "OutBox");
}

void Delete::Finish()
{
}

