/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
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

#include <icetray/I3Module.h>
#include <icetray/I3Frame.h>
#include <boost/foreach.hpp>
#include <vector>
#include <string>

//
//  This module just does a "get" on each of the list of keys it is
//  given, on each of the streams it is given.  This is only useful
//  (as far as I know) for some specific framework testing situations.
//
class Get : public I3Module
{
  std::vector<std::string> keys_;
  std::vector<I3Frame::Stream> streams_;
  bool get_all_;

 public:

  Get(const I3Context& context);

  void Configure();

private:

  void impl(I3FramePtr frame);

};

I3_MODULE(Get);

Get::Get(const I3Context& context) : I3Module(context), get_all_(false)
{
  AddParameter("Keys", "Keys to Get<I3FrameObject> on.  If not specified, get all.",
	       keys_);
  AddParameter("Streams", "vector of I3Frame::Streams to do the Gets on.  If not specified, do it on Geometry, Calibration, DetectorStatus, and Physics",
	       streams_);
}

void 
Get::Configure()
{
  GetParameter("Keys", keys_);
  if (keys_.size() == 0)
    {
      log_info("no Keys passed to Get module...  will Get all frame objects");
      get_all_ = true;
    }
  GetParameter("Streams", streams_);
  if (streams_.size() == 0)
    {
      streams_.push_back(I3Frame::Stream('G'));
      streams_.push_back(I3Frame::Stream('C'));
      streams_.push_back(I3Frame::Stream('D'));
      streams_.push_back(I3Frame::Stream('P'));
    }
  BOOST_FOREACH(const I3Frame::Stream& stream, streams_)
    {
      Register(stream, &Get::impl);
    }
}

void 
Get::impl(I3FramePtr frame)
{
  if (get_all_)
    keys_ = frame->keys();

  BOOST_FOREACH(const std::string& key, keys_)
    {
      I3FrameObjectConstPtr fop = frame->Get<I3FrameObjectConstPtr>(key);
      if(!fop && frame->Has(key))
	log_fatal("Came across an unregistered class.");
      log_info("Got %s", key.c_str());
    }
  PushFrame(frame, "OutBox");
}

