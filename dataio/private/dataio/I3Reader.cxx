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
#include <icetray/IcetrayFwd.h>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/scoped_ptr.hpp>

#include <fstream>
#include <set>

#include <icetray/open.h>
#include <icetray/I3Frame.h>
#include <icetray/I3TrayInfo.h>
#include <icetray/I3Module.h>

class I3Reader : public I3Module
{

  unsigned nframes_;
  std::vector<std::string> filenames_;
  std::vector<std::string> skip_;
  std::vector<I3Frame::Stream> push_wo_merging_;
  bool popmeta_done_;
  bool skip_unregistered_;
  bool drop_blobs_;

  boost::iostreams::filtering_istream ifs_;

  I3FramePtr cache_, tmp_;

  std::vector<std::string>::iterator filenames_iter_;
  std::string current_filename_;

  void OpenNextFile();

 public:

  I3Reader(const I3Context&);

  void Configure();
  void Process();

  ~I3Reader();

};

namespace io = boost::iostreams;
namespace dataio = I3::dataio;

I3_MODULE(I3Reader);

I3Reader::I3Reader(const I3Context& context) : I3Module(context),
					       cache_(new I3Frame(I3Frame::Stream('!'))),
					       drop_blobs_(false), nframes_(0)
{
  string fname;
  push_wo_merging_.push_back(I3Frame::Stream('I')); // don't merge 'I' frames into others

  AddParameter("Filename", 
	       "Filename to read.  Use either this or Filenamelist, not both.", 
	       fname);

  AddParameter("FilenameList", 
	       "List of files to read, *IN SORTED ORDER*", 
	       filenames_);

  AddParameter("SkipKeys", 
	       "Don't load frame objects with these keys", 
	       skip_);

  AddParameter("DontMergeStreamTypes",
	       "Don't merge these kinds of frames into these others that go by",
	       push_wo_merging_);

  AddParameter("DropBuffers",
	       "Tell I3Frames not to cache buffers of serialized frameobject data (this saves memory "
	       "at the expense of processing speed and the ability to passthru unknown frame objects)",
	       drop_blobs_);

}

void
I3Reader::Configure()
{
  string fname;

  GetParameter("FileNameList", filenames_);
  GetParameter("FileName", fname);
  if(!filenames_.empty() && !fname.empty())
    log_fatal("Both Filename and FileNameList were specified.");

  if (filenames_.empty() && fname.empty())
    log_fatal("Neither 'Filename' nor 'FilenameList' specified");

  if (filenames_.empty())
    filenames_.push_back(fname);

  GetParameter("SkipKeys", skip_);
  filenames_iter_ = filenames_.begin();

  GetParameter("DontMergeStreamTypes",
	       push_wo_merging_);

  GetParameter("DropBuffers",
	       drop_blobs_);

  cache_->drop_blobs(drop_blobs_);

  OpenNextFile();
}

void
I3Reader::Process()
{
  if (ifs_.peek() == EOF)
    {
      if (filenames_iter_ == filenames_.end())
	{
	  RequestSuspension();
	  return;
	}
      else
	OpenNextFile();
    }

  I3FramePtr frame(new I3Frame);
  frame->drop_blobs(drop_blobs_);
  try {
	nframes_++;
	frame->load(ifs_, skip_);
  } catch (const std::exception &e) {
	log_fatal("Error reading %s at frame %d: %s!",
	    current_filename_.c_str(), nframes_, e.what());
	return;
  }
	
  I3Frame::Stream s = frame->GetStop();

  if (find(push_wo_merging_.begin(), push_wo_merging_.end(), s) == push_wo_merging_.end())
    {
      log_trace("Doing a merge");
      // sync the two up.
      cache_->purge(frame->GetStop());

      frame->merge(*cache_);
      *cache_ = *frame;
    }
  PushFrame(frame, "OutBox");
}

void
I3Reader::OpenNextFile()
{
  if (!ifs_.empty())
    ifs_.pop();
  ifs_.reset();
  assert(ifs_.empty());

  current_filename_ = *filenames_iter_;
  nframes_ = 0;
  filenames_iter_++;

  I3::dataio::open(ifs_, current_filename_);
  log_trace("Constructing with filename %s, %zu regexes", 
	    current_filename_.c_str(), skip_.size());

  log_info("Opened file %s", current_filename_.c_str());
}

I3Reader::~I3Reader() 
{ 
}

