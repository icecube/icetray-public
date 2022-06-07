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

