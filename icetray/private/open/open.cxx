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
#include <string>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/constants.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/algorithm/string.hpp>

#include <icetray/I3Logging.h>
#include <icetray/counter64.hpp>

#include "http_source.hpp"
#include "socket_source.hpp"

#ifdef I3_WITH_ZSTD
#include "zstd_filter.hpp"
#endif

#ifdef I3_WITH_LIBARCHIVE
#include "archive_filter.hpp"
#endif

using boost::algorithm::ends_with;
using namespace std;

// archive file formats that need to be processed directly with libarchive
const auto ARCHIVE_FILES = std::vector<std::string>({
    ".tar", ".tar.gz", ".tgz", ".tar.bz2", ".tbz2", ".tar.zst", ".tzst",
    ".pax.gz", ".pax.bz2", ".pax.zstd", ".cpio.gz", ".cpio.bz2", ".cpio.zstd"
});

namespace I3 {
  namespace dataio {

    namespace io = boost::iostreams;

    void open(io::filtering_istream& ifs, const std::string& filename)
    {
      if (!ifs.empty())
        ifs.pop();
      ifs.reset();
      if (!ifs.empty())
        log_fatal("ifs isn't empty!");

      if (any_of(ARCHIVE_FILES.begin(), ARCHIVE_FILES.end(), [filename](const std::string& suffix){return ends_with(filename, suffix);})) {
#ifdef I3_WITH_LIBARCHIVE
        ifs.push(archive_filter(filename));
        log_trace("Input file is an archive. Using libarchive.");
#else
        log_fatal("Input file is an archive, however libarchive is not found.");
#endif
      } else {
        if (ends_with(filename,".gz")){
          ifs.push(io::gzip_decompressor());
          log_trace("Input file ends in .gz.  Using gzip decompressor.");
        }else if (ends_with(filename,".bz2")){
          ifs.push(io::bzip2_decompressor());
          log_trace("Input file ends in .bz2.  Using bzip decompressor.");
        }
        else if (ends_with(filename,".zst")){
#ifdef I3_WITH_ZSTD
          ifs.push(zstd_decompressor());
          log_trace("Input file ends in .zst. Using zstd decompressor.");
#else
          log_fatal("Input file ends in .zst, however zstd is not found.");
#endif
        }

#ifdef I3_WITH_LIBARCHIVE
        /*
         * If it's not obviously an I3 file, treat it as a
         * gzipped/bzipped/lzma'd/xz'd/uncompressed
         * gnutar/pax/ustar/cpio/shar/iso9660 archive
         * containing I3 files.
         */
        else if (!ends_with(filename,".i3")) {
          ifs.push(archive_filter(filename));
          log_trace("Adding libarchive filter");
        }
#endif
        else{
          log_trace("Not using a decompressor.");
        }
      }

      if (filename.find("socket://") == 0) {
        boost::iostreams::file_descriptor_source fs = create_socket_source(filename);
        ifs.push(fs);
      } else if (filename.find("http://") == 0) {
        ifs.push(http_source(filename));
      } else {
        boost::iostreams::file_source fs(filename);
        if (!fs.is_open())
        log_fatal("problems opening file '%s' for reading.  Check permissions, paths.",
		    filename.c_str());
        ifs.push(fs);
      }

      log_debug("Opened file %s", filename.c_str());
    }

    void open(io::filtering_ostream& ofs,
	      const std::string& filename,
	      int compression_level,
	      std::ios::openmode mode)
    {
      if (!ofs.empty())
        ofs.pop();
      ofs.reset();

      if (ends_with(filename,".gz")){
        if(compression_level<=0)
          compression_level=6;
        ofs.push(io::gzip_compressor(compression_level));
        log_trace("Output file ends in .gz.  Using gzip compressor.");
      }else if (ends_with(filename,".bz2")){
        if(compression_level<=0)
          compression_level=6;
        ofs.push(io::bzip2_compressor(compression_level));
        log_trace("Output file ends in .bz2.  Using bzip2 compressor.");
      }else if (ends_with(filename,".zst")){
#ifdef I3_WITH_ZSTD	
        if(compression_level<=0)
          compression_level=4;
        ofs.push(zstd_compressor(compression_level));
        log_trace("Output file ends in .zst. Using zstd compressor.");
#else
        log_fatal("Output file ends in .zst, but libzstd-dev isn't installed.");
#endif
      }else{
        log_trace("Output file doesn't end in .gz or .bz2.  Not compressing.");
      }
      ofs.push(io::counter64());

      io::file_sink fs(filename, mode);
      if (!fs.is_open())
        log_fatal("Fatal error opening output file '%s'.  Check permissions, paths, etc.",
		  filename.c_str());
      ofs.push(fs);
    }

  } // namespace dataio
}  //  namespace I3
