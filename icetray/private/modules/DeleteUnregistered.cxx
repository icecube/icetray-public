/**
 *  $Id$
 *  
 *  Copyright (C) 2007, 2008, 2009
 *  Troy Straszheim <troy@icecube.umd.edu>
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
  void Configure();
  void Process();

 private:

  DeleteUnregistered(const DeleteUnregistered&);
  DeleteUnregistered& operator=(const DeleteUnregistered&);

  SET_LOGGER("DeleteUnregistered");
};


I3_MODULE(DeleteUnregistered);


DeleteUnregistered::DeleteUnregistered(const I3Context& context)
  : I3Module(context)
{
  AddOutBox("OutBox");
}

DeleteUnregistered::~DeleteUnregistered() 
{ }

void DeleteUnregistered::Configure()
{ }

void DeleteUnregistered::Process()
{
  I3FramePtr frame = PopFrame();
  
  vector<string> deleteme;
  for (I3Frame::typename_iterator iter = frame->typename_begin();
       iter != frame->typename_end();
       iter++)
    {
      try {
	I3FrameObjectConstPtr fop = frame->Get<I3FrameObjectConstPtr>(iter->first, true);
      } catch (const icecube::archive::archive_exception& e) {
	switch (e.code) {
	case icecube::archive::archive_exception::unregistered_class:
	case icecube::archive::archive_exception::unsupported_version:
	  deleteme.push_back(iter->first);
	  break;
	default:
	  throw;
	}
      }
    }
  for (unsigned i=0; i<deleteme.size(); i++)
    frame->Delete(deleteme[i]);
  PushFrame(frame, "OutBox"); 
}


