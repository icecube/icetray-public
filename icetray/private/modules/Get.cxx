/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
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

#include <icetray/I3Module.h>
#include <icetray/I3Frame.h>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
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

Get::Get(const I3Context& context) : I3Module(context)
{
  AddOutBox("OutBox");
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
      frame->Get<I3FrameObjectConstPtr>(key);
      log_info("Got %s", key.c_str());
    }
  PushFrame(frame, "OutBox");
}

