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

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

#include <icetray/I3ConditionalModule.h>
#include <icetray/I3TrayHeaders.h>


/**
 *  Deletes things from frame.  Has special privileges granted by I3Frame.
 */
class Delete : public I3ConditionalModule
{
 private:

  Delete();
  Delete(const Delete&);
  Delete& operator=(const Delete&);

  std::vector<std::string> delete_keys_;
  std::vector<std::string> delete_key_starts_;

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
	       "Delete objects with these names or...", 
	       delete_keys_);
  AddParameter("KeyStarts",
               "...objects with names that start with any of these strings",
               delete_key_starts_);
  AddOutBox("OutBox");
}

void Delete::Configure()
{
  GetParameter("Keys", delete_keys_);
  GetParameter("KeyStarts", delete_key_starts_);
}

void Delete::Process()
{
  I3FramePtr frame = PopFrame();

  for (vector<string>::const_iterator iter = delete_keys_.begin();
       iter != delete_keys_.end();
       iter++)
    {
      if (!iter->empty())
        {
          frame->Delete(*iter);
        }
    }
  
  for (vector<string>::const_iterator iter = delete_key_starts_.begin();
       iter != delete_key_starts_.end();
       iter++)
    {
      if (!iter->empty())
        {
          I3Frame::typename_iterator jter = frame->typename_begin();
          while (jter != frame->typename_end())
            {
              if (boost::starts_with(jter->first, *iter))
                {
                  frame->Delete((jter++)->first);
                }
              else
                {
                  ++jter;
                }
            }
        }
    }

  PushFrame(frame, "OutBox");
}

void Delete::Finish()
{
}

