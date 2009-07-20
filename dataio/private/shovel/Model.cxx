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
#include <boost/optional.hpp>
using boost::optional;

#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/ref.hpp>

#include <shovel/Model.h>
#include <shovel/View.h>
#include <icetray/serialization.h>
#include <icetray/Utility.h>
#include <dataio/I3File.h>
#include <vector>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

extern int errno;

using namespace std;
//using namespace boost::lambda;
//using boost::lambda::bind;
using namespace boost;
using boost::ref;

#include <boost/foreach.hpp>


Model::Model(View& view) : view_(view)
{
  log_trace("%s", __PRETTY_FUNCTION__);
  x_index_=0;
  y_index_=0;
}

namespace {
  void 
  fboo(double d)
  {
    View::Instance().scan_progress(d);
  }
}

int
Model::open_file(const std::string& filename, 
		 boost::optional<vector<I3Frame::Stream> > skipstreams,
		 boost::optional<unsigned> nframes
		 )
{
  View::Instance().start_scan_progress(filename);

  log_trace("%s", __PRETTY_FUNCTION__);

  if (i3file_.open_file(filename, fboo, skipstreams, nframes, false))
    log_fatal("error opening file %s", filename.c_str());
  
  View::Instance().end_scan_progress();

  log_trace("i3file opened");
  std::vector<I3File::FrameInfo> vfi;
  vfi = i3file_.frames();

  for(unsigned i=0; i<vfi.size(); i++)
    {
      frame_infos_.push_back(make_pair(vfi[i], i));
      if (vfi[i].stream != I3Frame::TrayInfo)
	frame_infos_other_.push_back(make_pair(vfi[i], i));
    }

  return 0;
}

unsigned
Model::totalframes()
{
  return frame_infos_.size();
}

void
Model::move_first()
{
  x_index_ = 0;
  notify();
}

void
Model::move_last()
{
  x_index_ = frame_infos_.size()-1;

  notify();
}

void
Model::move_x(int delta)
{
  int newx = x_index_ + delta;
  x_index_ = newx < 0 ? 0 : newx;
  x_index_ = x_index_ >= frame_infos_.size() ? frame_infos_.size() -1 : x_index_;
  //  log_trace("post move x is %u", x_index_);
  notify();
}

void 
Model::do_goto_frame()
{
  optional<unsigned> result = view_.dialog<unsigned>("  Goto frame number: ");
  if (result)
    goto_frame(*result);
}

void
Model::save_xml()
{
  optional<string> result = view_.get_file("Save xml to file: ");
  if (!result)
    return;

  std::ofstream ofs(result->c_str());

  unsigned fileindex = frame_infos_[x_index_].second;
  I3FramePtr frame = i3file_.get_raw_frame(fileindex);

  string xml = frame->as_xml(y_keystring_);

  ofs <<  prettify_xml(xml);
  ofs.close();
}

void
Model::write_frame()
{
  optional<string> result = view_.get_file("Write frame to file: ");
  if (!result)
    return;

  std::ofstream ofs(result->c_str(), ios::binary | ios::app);

  unsigned fileindex = frame_infos_[x_index_].second;
  I3FramePtr fp = i3file_.get_raw_frame(fileindex);
  fp->save(ofs, vector<string>());
  ofs.close();
}

void 
Model::goto_frame(unsigned frame)
{
  //  x_index_ = frame - 1;
  move_first();
  move_x(frame - 1);
  notify();
}

void 
Model::move_y(int delta)
{
  int newy = y_index_ + delta;
  y_index_ = newy < 0 ? 0 : newy;
  y_index_ = y_index_ >= y_max_ ? y_max_ - 1 : y_index_;
  notify();
}

I3FramePtr
Model::get_frame(unsigned index)
{
  return i3file_.get_raw_frame(frame_infos_[index].second);
}

void
Model::show_xml()
{
  I3FramePtr frame = get_frame(x_index_);
  if (frame->size() == 0)
    return;

  string xml;
  try {
    xml = frame->as_xml(y_keystring_);
  } catch (const std::exception& e) {
    xml = std::string("***\n*** ") + e.what() + " caught while deserializing\n***\n"
      + "*** Object is unreadable in an unanticipated way, \n"
      + "*** please retain the .i3 file and report this error\n"
      + "***\n"
      ;
    view_.page(xml);
    return;
  }
  try {
    string pretty = y_keystring_ + " [type: " + frame->type_name(y_keystring_)
      + "]\n\n" + prettify_xml(xml);
    view_.page(pretty);
  } catch (const std::bad_alloc& e) {
    log_trace("Pretty xml output not possible, going to plain xml");
    view_.page(xml);
  }
}

void
Model::pretty_print()
{
  I3FramePtr frame = get_frame(x_index_);
  if (frame->size() == 0)
    return;

  I3TrayInfoConstPtr ticp = frame->Get<I3TrayInfoConstPtr>(y_keystring_);
  
  ostringstream oss;

  if (!ticp)
    oss << "\n\n\nCan only pretty-print I3TrayInfo currently.\n\n\n";
  else
    oss << *ticp;
  view_.page(oss.str());
}

void 
Model::notify()
{
  I3FramePtr frame = get_frame(x_index_);
  y_max_ = frame->size();

  if (y_index_ >= y_max_)
    y_index_ = y_max_-1;
  if (frame->size() == 0)
    y_index_ = 0;

  // you have to get the sorted keys out here
  vector<string> keys = frame->keys();
  log_trace("Got frame with %zu keys", keys.size());
  for (unsigned i=0; i < keys.size(); i++)
    {
      log_debug("key: %s", keys[i].c_str());
    }

  y_keystring_ = frame->size() > 0 ? keys[y_index_] : std::string();

  view_.display_frame(frame, x_index_, y_index_);
}

vector<I3Frame::Stream> 
Model::streams(unsigned start_index, unsigned length)
{
  //  log_trace("Get streams for %u, len %u", start_index, length);
  assert(start_index + length <= frame_infos_.size());
  vector<I3Frame::Stream> ret;

  for (unsigned i=0; i<length; i++)
    {
      ret.push_back(frame_infos_[start_index + i].first.stream);
      //      log_trace("push %c", ret[i].value);
    }

  
  return ret;
}

void
Model::toggle_infoframes()
{
  frame_infos_.swap(frame_infos_other_);
  if (x_index_ >= frame_infos_.size())
    x_index_ = frame_infos_.size() - 1;
  notify();
}
