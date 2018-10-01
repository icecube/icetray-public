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
#include <icetray/I3Frame.h>
#include <icetray/I3FrameObject.h>
#include <icetray/Utility.h>
#include <icetray/serialization.h>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/constants.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include <boost/regex.hpp>
#include <boost/foreach.hpp>

#include <icetray/counter64.hpp>

#include <map>
#include <set>
#include <string>
#include <iostream>
#include <fstream>

#define I3_WITH_MMAPPED_FILE_SOURCE

#ifdef I3_WITH_MMAPPED_FILE_SOURCE
namespace boost {
  namespace iostreams {
    namespace detail {
      template <>
      struct optimal_buffer_size_impl<mapped_file_source>
      {
	template <typename T>
	  static std::streamsize 
	  optimal_buffer_size(const T& t)
	{
	  return 4096;
	}
      };
    }
  }
}
#endif
using namespace boost::archive;
using namespace std;

namespace I3 {
  namespace dataio {

    namespace io = boost::iostreams;
    namespace ar = boost::archive;
    //
    //  open compressed streams
    //
    void open(io::filtering_istream& ifs, const std::string& filename)
    {
      if (!ifs.empty())
	ifs.pop();
      ifs.reset();
      if (!ifs.empty()) log_fatal("ifs isn't empty!");
	
      log_trace("Constructing with filename %s", 
      		filename.c_str());

      if (filename.rfind(".gz") == (filename.length() -3))
	{
	  ifs.push(io::gzip_decompressor());
	  log_trace("Input file ends in .gz.  Using gzip decompressor.");
	}
      else
	{
	  log_trace("Input file doesn't end in .gz.  Not decompressing.");
	}

#ifdef I3_WITH_MMAPED_FILE_SOURCE
      io::mapped_file_source fs(filename);
#else
      io::file_source fs(filename);
#endif

      if (!fs.is_open())
	log_fatal("problems opening file '%s' for reading.  Check permissions, paths.",
		  filename.c_str());

      ifs.push(fs);
      
      log_debug("Opened file %s", filename.c_str());
    }

    void open(io::filtering_ostream& ofs, 
	      const std::string& filename, 
	      unsigned compression_level,
	      std::ios::openmode mode)
    {
      if (!ofs.empty())
	ofs.pop();
      ofs.reset();

      if (filename.rfind(".gz") == (filename.length() -3))
	{
	  ofs.push(io::gzip_compressor(compression_level));
	  log_trace("Input file ends in .gz.  Using gzip decompressor.");
	}
      else
	{
	  log_trace("Input file doesn't end in .gz.  Not decompressing.");
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

