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
#include <icetray/IcetrayFwd.h>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

#include <fstream>
#include <set>

#include <icetray/open.h>
#include <icetray/I3Frame.h>
#include <icetray/I3TrayInfo.h>
#include <icetray/I3Module.h>

#include "dataio/I3FileStager.h"

class I3Reader : public I3Module
{

  unsigned nframes_;
  bool drop_blobs_;
  std::vector<std::string> filenames_;
  std::vector<std::string> skip_;
  I3FileStagerPtr file_stager_;
  I3::dataio::shared_filehandle current_filename_;

  boost::iostreams::filtering_istream ifs_;

  I3FramePtr tmp_;

  std::vector<std::string>::iterator filenames_iter_;

  void OpenNextFile();

 public:

  I3Reader(const I3Context&);

  void Configure();
  void Process();

  ~I3Reader();
  SET_LOGGER("I3Reader");

};

namespace io = boost::iostreams;
namespace dataio = I3::dataio;

I3_MODULE(I3Reader);

I3Reader::I3Reader(const I3Context& context) : I3Module(context),
					       nframes_(0),
					       drop_blobs_(false)
{
  std::string fname;

  AddParameter("Filename", 
	       "Filename to read.  Use either this or Filenamelist, not both.", 
	       fname);

  AddParameter("FilenameList", 
	       "List of files to read, *IN SORTED ORDER*", 
	       std::vector<std::string>());

  AddParameter("SkipKeys", 
	       "Don't load frame objects with these keys", 
	       skip_);

  AddParameter("DropBuffers",
	       "Tell I3Frames not to cache buffers of serialized frameobject data (this saves memory "
	       "at the expense of processing speed and the ability to passthru unknown frame objects)",
	       drop_blobs_);

  AddOutBox("OutBox");
}

void
I3Reader::Configure()
{
  std::string fname;

  GetParameter("FileNameList", filenames_);
  GetParameter("FileName", fname);
  if(!filenames_.empty() && !fname.empty())
    log_fatal("Both Filename and FileNameList were specified.");

  if (filenames_.empty() && fname.empty())
    log_fatal("Neither 'Filename' nor 'FilenameList' specified");

  if (filenames_.empty())
    filenames_.push_back(fname);

  GetParameter("SkipKeys", skip_);

  GetParameter("DropBuffers",
	       drop_blobs_);
  
  file_stager_ = context_.Get<I3FileStagerPtr>();
  if (!file_stager_)
     file_stager_ = I3TrivialFileStager::create();
  BOOST_FOREACH(const std::string &filename, filenames_)
    file_stager_->WillReadLater(filename);

  filenames_iter_ = filenames_.begin();
  OpenNextFile();
}

void
I3Reader::Process()
{
  while (ifs_.peek() == EOF)
    {
      if (filenames_iter_ == filenames_.end())
	{
	  RequestSuspension();
    current_filename_.reset();
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
	    current_filename_->c_str(), nframes_, e.what());
	return;
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

  current_filename_.reset();
  current_filename_ = file_stager_->GetReadablePath(*filenames_iter_);
  nframes_ = 0;
  filenames_iter_++;

  I3::dataio::open(ifs_, *current_filename_);
  log_trace("Constructing with filename %s, %zu regexes", 
	    current_filename_->c_str(), skip_.size());

  log_info("Opened file %s", current_filename_->c_str());
}

I3Reader::~I3Reader() 
{ 
}

