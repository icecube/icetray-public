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

#include <boost/foreach.hpp>

#include <dataio/I3File.h>
#include <icetray/serialization.h>
#include <icetray/Utility.h>

#include <vector>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

extern int errno;

using namespace std;

typedef I3File::FrameInfo FrameInfo;

class I3FileImpl
{
  std::vector<I3File::FrameInfo> frame_infos_;

  std::ifstream ifs_;

  unsigned x_index_;
  unsigned y_index_;
  unsigned y_max_;
  string y_keystring_;

  vector<string> skipkeys_;

  map<unsigned, I3FramePtr> frame_cache_;

  static void noop(double) { }

public:
 
  I3FileImpl();

  int open_file(const std::string& filename,
		boost::function<void(double)> cb = noop,
		boost::optional<vector<I3Frame::Stream> > skipstreams = boost::optional<vector<I3Frame::Stream> >(),
		boost::optional<unsigned> nframes = boost::optional<unsigned>(),
		bool verbose = true);

  void close();

  I3FramePtr get_frame(unsigned);
  I3FramePtr get_raw_frame(unsigned);

  void move_first();
  void move_last();

  void move_x(int delta);
  void move_y(int delta);
  void do_goto_frame();
  void goto_frame(unsigned frameno);

  I3Frame::Stream stream(unsigned index);
  std::vector<I3Frame::Stream> streams(unsigned start_index, unsigned length) const;
  std::vector<I3Frame::Stream> streams() const;
  std::vector<FrameInfo> frames() const; 
  size_t size() { return frame_infos_.size(); }
  void set_skipkeys(const vector<string>&);
};

I3FileImpl::I3FileImpl() : 
  x_index_(0), 
  y_index_(0),
  y_max_(0)
{ }

void
I3FileImpl::set_skipkeys(const vector<string>& sk)
{
  skipkeys_ = sk;
}

void
I3FileImpl::close()
{
  ifs_.close();
  frame_infos_.clear();
  skipkeys_.clear();
}

int
I3FileImpl::open_file(const std::string& filename, boost::function<void(double)> cb,
		      boost::optional<vector<I3Frame::Stream> >skipstreams,
		      boost::optional<unsigned> nframes_opt,
		      bool verbose)
{
  log_trace("I3FileImpl::open_file(%s)", filename.c_str());
  if (filename.find(".gz") == filename.length() - 3)
    log_fatal("can't read zipped files... please decompress.");

  ifs_.clear();
  ifs_.open(filename.c_str(), ios::binary);
  if (!ifs_)
    {
      cerr << "Can't open file " << filename << " for reading: " << strerror(errno) << "(" << errno << ")\n";
      return 1;
    }

  // get length of file:
  ifs_.seekg (0, ios::end);
  uint64_t length = ifs_.tellg();
  ifs_.seekg (0, ios::beg);
  if (verbose)
    {
      cout << "Scanning " << filename << " (" << length << " bytes)\n";
      cout.flush();
    }

  unsigned counter = 0, pct = 0;

  unsigned nframes = nframes_opt ? *nframes_opt : std::numeric_limits<unsigned>::max();

  cb(pct);

  std::map<I3Frame::Stream, unsigned> stream_cache;

  bool first = true;

  while (ifs_.peek() != EOF && counter < nframes)
    {
      FrameInfo frame_info;
      frame_info.pos = ifs_.tellg();
      
      unsigned newpct = (int) ((double)frame_info.pos/(double)length * 100.0);
      if (newpct != pct)
	{
	  log_trace("send new percentage complete %u", (unsigned)pct);
	  pct = newpct;
	  cb(pct);
	}

      if (ifs_.peek() == EOF)
	break;

      I3FramePtr frame(new I3Frame);
      try {
	frame->load(ifs_, vector<string>());
      } catch (const std::exception& e) {
	log_info("caught exception %s at frame %zu", 
		 e.what(), frame_infos_.size());
	break;
      }

      if (skipstreams)
	{
	  bool skipit = false;
	  BOOST_FOREACH(const I3Frame::Stream& s, *skipstreams)
	    {
	      if (frame->GetStop() == s)
		skipit = true;
	    }
	  if (skipit) 
	    continue;
	}

      stream_cache[frame->GetStop()] = counter;
      log_trace("Frame %c at %u (%u total)", frame->GetStop().id(), counter, stream_cache.size());

      frame_info.stream = frame->GetStop();
      frame_info.other_streams = stream_cache;
      frame_infos_.push_back(frame_info);
      counter++;
    }
  return 0;
}


void
I3FileImpl::move_first()
{
  x_index_ = 0;
}

void
I3FileImpl::move_last()
{
  x_index_ = frame_infos_.size()-1;
}

void
I3FileImpl::move_x(int delta)
{
  int newx = x_index_ + delta;
  x_index_ = newx < 0 ? 0 : newx;
  x_index_ = x_index_ >= frame_infos_.size() ? frame_infos_.size() -1 : x_index_;
  //  log_trace("post move x is %u", x_index_);
}

void 
I3FileImpl::do_goto_frame()
{
  //  optional<unsigned> result = view_.dialog<unsigned>(" Goto frame number: ");
  //  if (result)
  //    goto_frame(*result);
}

void 
I3FileImpl::goto_frame(unsigned frame)
{
  //  x_index_ = frame - 1;
  move_first();
  move_x(frame - 1);
}

void 
I3FileImpl::move_y(int delta)
{
  int newy = y_index_ + delta;
  y_index_ = newy < 0 ? 0 : newy;
  y_index_ = y_index_ >= y_max_ ? y_max_ - 1 : y_index_;
}

I3FramePtr
I3FileImpl::get_raw_frame(unsigned index)
{
  log_debug("getting raw frame %u", index);
  ifs_.seekg(frame_infos_[index].pos);
  ifs_.clear();
  assert(!ifs_.fail());
  assert(!ifs_.bad());
  assert(!ifs_.eof());
  assert(ifs_.good());

  I3FramePtr frame(new I3Frame);
  frame->load(ifs_, skipkeys_);
  
  return frame;
}

I3FramePtr
I3FileImpl::get_frame(unsigned index)
{
  I3FramePtr frame = get_raw_frame(index);
  I3Frame cache;
  
  const FrameInfo& fi = frame_infos_[index];

  log_trace("other streams size = %u", fi.other_streams.size());
  for (FrameInfo::stream_map_t::const_iterator iter = fi.other_streams.begin();
       iter != fi.other_streams.end();
       iter++)
    {
      log_trace("prev frame %u on stream %c", iter->second, iter->first.id());
      if (iter->first == frame->GetStop())
	{
	  assert(iter->second == index); 
	  continue;
	}
      I3FramePtr otherframe = get_raw_frame(iter->second);
      frame->merge(*otherframe);
    }

  return frame;
}

vector<I3File::FrameInfo>
I3FileImpl::frames() const
{
  return frame_infos_;
}

vector<I3Frame::Stream> 
I3FileImpl::streams(unsigned start_index, unsigned length) const
{
  assert(start_index + length <= frame_infos_.size());
  vector<I3Frame::Stream> ret;

  for (unsigned i=0; i<length; i++)
    {
      ret.push_back(frame_infos_[start_index + i].stream);
    }

  return ret;
}

vector<I3Frame::Stream> 
I3FileImpl::streams() const
{
  vector<I3Frame::Stream> ret;

  for (unsigned i=0; i<frame_infos_.size(); i++)
    {
      ret.push_back(frame_infos_[i].stream);
    }
  return ret;
}

I3Frame::Stream
I3FileImpl::stream(unsigned index)
{
  return frame_infos_[index].stream;
}


//
// Interface
//

I3File::I3File()
  : impl_(new I3FileImpl)
{ }

I3File::~I3File() 
{ }

int 
I3File::open_file(const std::string& filename,
		  boost::function<void(double)> cb,
		  boost::optional<vector<I3Frame::Stream> > skipstreams,
		  boost::optional<unsigned> nframes,
		  bool verbose)
{
  return impl_->open_file(filename, cb, skipstreams, nframes, verbose);
}

void 
I3File::close() { impl_->close(); }

I3FramePtr I3File::get_frame(unsigned u) { return impl_->get_frame(u); }
I3FramePtr I3File::get_raw_frame(unsigned u) { return impl_->get_raw_frame(u); }

void I3File::move_first() { impl_->move_first(); }
void I3File::move_last() { impl_->move_last(); }

void I3File::move_x(int delta) { impl_->move_x(delta); }
void I3File::move_y(int delta) { impl_->move_y(delta); }
void I3File::do_goto_frame() { impl_->do_goto_frame(); }
void I3File::goto_frame(unsigned frameno) { impl_->goto_frame(frameno); }

I3Frame::Stream 
I3File::stream(unsigned index) { return impl_->stream(index); }

std::vector<I3Frame::Stream> I3File::streams(unsigned start_index, unsigned length) const
{
  return impl_->streams(start_index,length);
}

std::vector<I3Frame::Stream> 
I3File::streams() const
{
  return impl_->streams();
}

std::vector<I3File::FrameInfo>
I3File::frames()
{
  return impl_->frames();
}

size_t 
I3File::size() 
{ 
  return impl_->size();
}

void 
I3File::set_skipkeys(const vector<string>& vs)
{
  impl_->set_skipkeys(vs);
}

