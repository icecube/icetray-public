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
#ifndef I3_WRITERBASE_H_INCLUDED
#define I3_WRITERBASE_H_INCLUDED

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include <boost/iostreams/filtering_stream.hpp>

#include "icetray/I3ConditionalModule.h"
#include "dataio/I3FileStager.h"

class I3WriterBase : public I3ConditionalModule
{
protected:
  bool configWritten_;
  int frameCounter_;
  std::vector<std::string> skip_keys_;
  std::vector<I3Frame::Stream> streams_;
  std::vector<I3Frame::Stream> dropOrphanStreams_;
  std::vector<I3FramePtr> orphanarium_;

  void WriteConfig(I3FramePtr ptr);
  
  boost::iostreams::filtering_ostream filterstream_;
  std::string path_;
  I3FileStagerPtr file_stager_;
  I3::dataio::shared_filehandle current_filename_;

  int gzip_compression_level_;

public:

  I3WriterBase(const I3Context& ctx);

  virtual ~I3WriterBase() { }

  void Configure();
  void Finish();

  void Process();
};

#endif
