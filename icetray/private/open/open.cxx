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

      if (ends_with(filename,".gz")){
        ifs.push(io::gzip_decompressor());
        log_trace("Input file ends in .gz.  Using gzip decompressor.");
      }else if (ends_with(filename,".bz2")){
        ifs.push(io::bzip2_decompressor());
      }
      
#ifdef I3_WITH_LIBARCHIVE
	/*
	 * If it's not obviously an I3 file, treat it as a
	 * gzipped/bzipped/lzma'd/xz'd/uncompressed
	 * gnutar/pax/ustar/cpio/shar/iso9660 archive
	 * containing I3 files.
	 */
      else if (!ends_with(filename,".i3"))
		ifs.push(archive_filter(filename));
#endif

      else if (ends_with(filename,".zst")){
#ifdef I3_WITH_ZSTD
        ifs.push(zstd_decompressor());
        log_trace("Input file ends in .zst. Using zstd decompressor.");
#else
        log_fatal("Input file ends in .zst, however zstd is not found.");
#endif	
      }
      else{
        log_trace("Not decompressing.");
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
