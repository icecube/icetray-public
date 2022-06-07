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
#include <dataio/I3MultiWriter.h>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>

#include <icetray/counter64.hpp>
#include <icetray/open.h>

using boost::algorithm::to_lower;

namespace io = boost::iostreams;
namespace dataio = I3::dataio;

I3_MODULE(I3MultiWriter);

I3MultiWriter::I3MultiWriter(const I3Context& ctx) 
  : I3WriterBase(ctx), size_limit_(0), sync_seen_(false), file_counter_(0)
{ 
  AddParameter("SizeLimit",
	       "Soft Size limit in bytes.  Files will typically exceed this limit by the size of one half of one frame.",
	       size_limit_);
  AddParameter("SyncStream",
	       "Frame type to wait for to split files. New files will always begin with a frame of this type. Useful for frames from which events need to inherit (e.g. DAQ frames).",
	       I3Frame::DAQ);
  std::vector<I3Frame::Stream> default_metadata;
  default_metadata.push_back(I3Frame::Geometry);
  default_metadata.push_back(I3Frame::Calibration);
  default_metadata.push_back(I3Frame::DetectorStatus);
  AddParameter("MetadataStreams",
	       "Frame types to cache and write at the beginning of all new files (e.g. GCD frames). If a frame is not in Streams, even if specified here, it will never be written.",
	       default_metadata);
}

I3MultiWriter::~I3MultiWriter() { }

void
I3MultiWriter::Configure_()
{
  log_trace("%s", __PRETTY_FUNCTION__);
  I3ConditionalModule::Configure_();

  GetParameter("SizeLimit", size_limit_);
  if (size_limit_ == 0)
    log_fatal("SizeLimit (%llu) must be > 0", (unsigned long long)size_limit_);
  GetParameter("SyncStream", sync_stream_);
  GetParameter("MetadataStreams", metadata_streams_);

  log_trace("path_=%s", path_.c_str());
  NewFile();
}

void 
I3MultiWriter::NewFile()
{
  log_trace("%s", __PRETTY_FUNCTION__);

  boost::format f(path_);
  try {
    f % (file_counter_++);
  } catch (const std::exception& e) {
    log_error("Exception caught: %s", e.what());
    log_error("Does your Filename contain a printf-style specifier where the number should go, e.g., '%s'?",
	      "myfile-%04u.i3.gz");
    throw;
  }
  std::string current_path = f.str();

  current_filename_ = file_stager_->GetWriteablePath(current_path);
  log_info("Starting new file '%s'", current_filename_->c_str());
  dataio::open(filterstream_, *current_filename_, gzip_compression_level_);

  BOOST_FOREACH(I3FramePtr frame, metadata_cache_)
	frame->save(filterstream_, skip_keys_);
}

void
I3MultiWriter::Process()
{
  I3FramePtr frame;

  log_trace("%s", __PRETTY_FUNCTION__);

  uint64_t bytes_written;

  // Need to flush to evaluate file size
  filterstream_.flush();
  io::counter64* ctr = filterstream_.component<io::counter64>(filterstream_.size() - 2);
  if (!ctr) log_fatal("couldnt get counter from stream");
  bytes_written = ctr->characters();

  log_trace("%llu bytes: %s", (unsigned long long)bytes_written, current_filename_->c_str());

  frame = PeekFrame();
  if (frame == NULL)
    return;

  if (frame->GetStop() == sync_stream_)
    sync_seen_ = true;

  if (bytes_written > size_limit_ && (frame->GetStop() == sync_stream_ ||
    !sync_seen_))
      NewFile();

  if (std::find(metadata_streams_.begin(), metadata_streams_.end(),
    frame->GetStop()) != metadata_streams_.end() &&
    (streams_.size() == 0 || std::find(streams_.begin(), streams_.end(),
      frame->GetStop()) != streams_.end())) {
	// If this is a frame type we need to cache for file-start metadata,
	// insert it into the cache in a way that preserves the ordering of the
	// frames as they were first inserted into the stream.
	
	// Duplicate frame so it can't be modified by following modules
	I3FramePtr cache_copy(new I3Frame(frame->GetStop()));
	cache_copy->merge(*frame);

	size_t i;
	for (i = 0; i < metadata_cache_.size(); i++) {
		if (metadata_cache_[i]->GetStop() == frame->GetStop()) {
			metadata_cache_[i] = cache_copy;
			break;
		}
	}
	if (i == metadata_cache_.size())
		metadata_cache_.push_back(cache_copy);
  }

  I3WriterBase::Process();
}

void
I3MultiWriter::Finish()
{
  log_trace("%s", __PRETTY_FUNCTION__);

  io::counter64* ctr = filterstream_.component<io::counter64>(filterstream_.size() - 2);
  if (!ctr) log_fatal("couldnt get counter from stream");
  uint64_t lastfile_bytes = ctr->characters();

  filterstream_.reset();

  log_trace("lastfile bytes=%llu", (unsigned long long)lastfile_bytes);

  if (lastfile_bytes == 0)
    {
      log_trace("unlinking %s", current_filename_->c_str());
      unlink(current_filename_->c_str());
    }
  I3WriterBase::Finish();
}

