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
#include <dataio/I3Writer.h>
#include <icetray/open.h>

namespace io = boost::iostreams;
namespace dataio = I3::dataio;

I3_MODULE(I3Writer);

I3Writer::I3Writer(const I3Context& ctx) 
  : I3WriterBase(ctx)
{ }

I3Writer::~I3Writer() { }

void
I3Writer::Configure_()
{
  log_trace("%s", __PRETTY_FUNCTION__);
  I3ConditionalModule::Configure_();
  current_filename_ = file_stager_->GetWriteablePath(path_);
  dataio::open(filterstream_, *current_filename_, gzip_compression_level_);
}

void
I3Writer::Finish()
{
#if BOOST_VERSION < 104400
  if (frameCounter_ == 0 &&
    (current_filename_->rfind(".bz2") == current_filename_->size()-4) &&
    streams_.size() > 0 && find(streams_.begin(), streams_.end(),
    I3Frame::TrayInfo) == streams_.end()) {
      log_warn("You have attempted to write an empty file using bzip2 "
               "compression with a Boost version prior to 1.44. A bug (fixed "
               "in Boost 1.44) would normally cause the process to exit with "
               "a bus error, To prevent this, either upgrade to a newer Boost,"
               "switch to gzip compression, or ensure that you always write at "
               "least 1 frame (for example, the TrayInfo frame). As a temporary "
               "workaround, I have written the TrayInfo frame to the output "
               "file for you.");
      // temporarily reconfigure output
      configWritten_ = false;
      std::vector<I3Frame::Stream> tempstreams(1, I3Frame::TrayInfo);
      tempstreams.swap(streams_);
      // write TrayInfo with a dummy frame
      I3WriterBase::WriteConfig(I3FramePtr(new I3Frame));
      Flush();
      // restore old configuration
      tempstreams.swap(streams_);
  }
#endif
  filterstream_.reset();
  I3WriterBase::Finish();
}
