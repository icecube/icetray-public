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
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/constants.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include <icetray/counter64.hpp>

#include <map>
#include <set>
#include <string>
#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#ifdef ROOT_HAS_NETX
#include <TXNetFile.h>
#endif

#ifdef I3_WITH_DCAP
#include <dcap.h>
#endif

#ifdef I3_WITH_LIBARCHIVE
#include <archive.h>
#include <archive_entry.h>
#endif

using boost::algorithm::ends_with;

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

#ifdef ROOT_HAS_NETX
struct xrootd_source 
{
  typedef char char_type;
  typedef boost::iostreams::source_tag category;

  std::streamsize bytes_read;
  std::string name;
  boost::shared_ptr<TXNetFile> netfile;

  xrootd_source(const std::string& s)
    : bytes_read(0),
      name(s + "?filetype=raw"),
      netfile(new TXNetFile(name.c_str()))
  { }

  std::streamsize read(char* s, std::streamsize n)
  {
    bool b = netfile->ReadBuffer(s, n);
    if (b == kTRUE) // error
      {
	//
	// FIXME:  how can we tell if this is regular EOF or something
	//         really bad?
	//
	//	log_error("error reading from xrootd");
	return -1;
      }
    std::streamsize newsize = netfile->GetBytesRead();
    std::streamsize r = newsize-bytes_read;
    bytes_read = newsize;
    return r;
  }
};
#endif

#ifdef I3_WITH_DCAP
struct dcap_source 
{
  typedef char char_type;
  struct category 
    : boost::iostreams::source_tag,
      boost::iostreams::closable_tag { };

  std::string name;
  int fd;

  static const std::string prefix;

  dcap_source(const std::string& s)
  { 
    dc_setClientActive();

    std::string dcapfile = "";
    if (s.compare(prefix.size(), 3, "///") == 0) { // file name given as dcap:///pnfs/path/to/file
      dcapfile = s.substr(prefix.size());
    } else if (s.compare(prefix.size(), 2, "//") == 0) { // file name given as dcap://site.org/path/to/file
      dcapfile = s;
    } else {
      log_error("Cannot open file '%s' on dCache.", s.c_str());
      fd = -1;
      return;
    }

    fd = dc_open(dcapfile.c_str(), O_RDONLY);
  }

  std::streamsize read(char* s, std::streamsize n)
  {
    dc_errno = 0;
    std::streamsize nread = dc_read(fd, s, n);

    if (nread == 0) return -1;   // boost::iostreams indicates eof by return value -1
    if (nread < 0) {
      log_error("Error reading file.");
      return -1;
    }

    return nread;
  }

  void close() {
    dc_close(fd);
  }

};

const std::string dcap_source::prefix = "dcap:";

int dc_errno;
#endif

#ifdef I3_WITH_LIBARCHIVE

#if ARCHIVE_VERSION_NUMBER < 3000000
// Forwards compatibility macros
#define archive_read_free archive_read_finish
#define archive_read_support_filter_all archive_read_support_compression_all 
#define archive_read_support_filter_gzip archive_read_support_compression_gzip
#define archive_read_support_filter_bzip2 archive_read_support_compression_bzip2
#define archive_read_support_filter_lzma archive_read_support_compression_lzma
#define archive_read_support_filter_xz archive_read_support_compression_xz
#endif

struct archive_filter {
	typedef char char_type;
	typedef boost::iostreams::multichar_input_filter_tag category;

	struct client_data {
		void *source;
		char_type buffer[4096];
		ssize_t capacity;
	};
	
	typedef enum { GZIP, BZIP2, LZMA, XZ, NONE } compression_type;

	boost::shared_ptr<struct archive> reader_;
	struct archive_entry *current_entry_;
	client_data source_info_;
	bool header_read_, raw_archive_;
	ssize_t bytes_read_;

	static void archive_destructor(struct archive *ar)
	{
		if (!ar) return;
		
		archive_read_free(ar);
	}
	
	archive_filter(const std::string& filename) :
	    reader_(archive_read_new(), archive_destructor),
	    header_read_(false), raw_archive_(false), bytes_read_(0)
	{
		archive_read_support_format_all(reader_.get());
		archive_read_support_format_raw(reader_.get());
	
		compression_type comp = guess_compression(filename);
		
		switch (comp) {
			case GZIP:
				if (archive_read_support_filter_gzip(reader_.get()) == ARCHIVE_WARN)
					log_debug("(archive_filter) no built-in gzip decompression.");
				break;
			case BZIP2:
				if (archive_read_support_filter_bzip2(reader_.get()) == ARCHIVE_WARN)
					log_debug("(archive_filter) no built-in bzip2 decompression.");
				break;
			case LZMA:
				if (archive_read_support_filter_lzma(reader_.get()) == ARCHIVE_WARN)
					log_debug("(archive_filter) no built-in lzma decompression.");
				break;
			case XZ:
				if (archive_read_support_filter_xz(reader_.get()) == ARCHIVE_WARN)
					log_debug("(archive_filter) no built-in xz decompression.");
				break;
			default:
				archive_read_support_filter_all(reader_.get());
				log_debug("(archive_filter) The compression scheme of file '%s' "
				    "is not obvious from the name. Attempting autodetection...",
				    filename.c_str());
				break;
		}
		
		source_info_.source = NULL;
		source_info_.capacity = 4096;
	}
	
	template<typename Source>
	std::streamsize read(Source& src, char_type* s, std::streamsize n)
	{
		if (source_info_.source != &src) {
			source_info_.source = (void*)&src;
			/* TODO: add a callback for skipping over entries. */
			int err = archive_read_open(reader_.get(), &source_info_, NULL, &read_stream<Source>, NULL);
			if (err != ARCHIVE_OK)
				log_fatal("archive_read_open() failed: %s", archive_error_string(reader_.get()));
			raw_archive_ = false;
			log_trace("(archive_filter) opened new source");
		}
		
		while (!header_read_ && !raw_archive_) {
			if (archive_read_next_header(reader_.get(), &current_entry_) == ARCHIVE_OK) {
				std::string fname(archive_entry_pathname(current_entry_));
				/*
				 * NB: libarchive exposes raw data as an archive with a
				 * single entry whose pathname is "data".
				 */
				if (fname == "data") {
					log_trace("(archive_filter) Unrecognized archive format!"
					    " Falling back to raw mode.");
					raw_archive_ = true;
					continue;
				}
				
				if (!ends_with(fname,".i3")) {
					log_trace("(archive_filter) skipping file '%s' (not an I3 file)",
					    fname.c_str());
					continue;
				} else {
					log_trace("(archive_filter) reading file '%s'", fname.c_str());
					header_read_ = true;
					bytes_read_ = 0;
				}
			} else {
				return -1;
			}
		}

		ssize_t nread = archive_read_data(reader_.get(), s, n);
		bytes_read_ += nread;
	
		if (nread <= 0)
			return -1; /* boost::iostreams-style EOF */
		
		if (!raw_archive_ && bytes_read_ >= archive_entry_size(current_entry_))
			header_read_ = false;
		
		return nread;
	}
	
	/* A callback for libarchive. */
	template <typename Source>
	static ssize_t read_stream(struct archive *a, void *client_data_blob, const void **buff)
	{
		client_data *data = (client_data*)client_data_blob;
		Source &source = *((Source*)data->source);
		/* FIXME: for now we always read in 4096-byte chunks. Better ideas? */
		ssize_t nread = boost::iostreams::read(source, data->buffer, data->capacity);
		*buff = data->buffer;
		
		if (nread < 0)
			return 0; /* libarchive-style EOF */
		else
			return nread;
	}
	
	static compression_type guess_compression(const std::string &filename)
	{
		compression_type comp = NONE;
		if (ends_with(filename,".gz"))
			comp = GZIP;
		else if (ends_with(filename,".bz2"))
			comp = BZIP2;
		else if (ends_with(filename,".lzma"))
			comp = LZMA;
		else if (ends_with(filename,".xz"))
			comp = XZ;
			
		return comp;
	}
};

#endif


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

#ifdef I3_WITH_LIBARCHIVE
	/*
	 * If it's not obviously an I3 file, treat it as a
	 * gzipped/bzipped/lzma'd/xz'd/uncompressed
	 * gnutar/pax/ustar/cpio/shar/iso9660 archive
	 * containing I3 files.
	 */
      if (!ends_with(filename,".i3"))
		ifs.push(archive_filter(filename));
#else
      if (ends_with(filename,".gz"))
	{
	  ifs.push(io::gzip_decompressor());
	  log_trace("Input file ends in .gz.  Using gzip decompressor.");
	}
      else if (ends_with(filename,".bz2"))
	{
	  ifs.push(io::bzip2_decompressor());
	  log_trace("Input file ends in .bz2.  Using bzip2 decompressor.");
	}
      else
	{
	  log_trace("Input file doesn't end in .gz or .bz2.  Not decompressing.");
	}
#endif

      if (filename.find("root://") == 0)
	{
#ifdef ROOT_HAS_NETX
	  xrootd_source src(filename);
	  ifs.push(src);
#else
	  log_fatal("IceTray was compiled without xrootd support, unable to open file %s",
		    filename.c_str());
#endif
	} 
      else if (filename.find("dcap://") == 0) {
#ifdef I3_WITH_DCAP
	dcap_source src(filename);
	ifs.push(src);
#else
	  log_fatal("IceTray was compiled without dcap support, unable to open file %s",
		    filename.c_str());
#endif
      } else if (filename.find("socket://") == 0) {
        std::string host, port("1313");
	host = filename.substr(strlen("socket://"));
	if (host.rfind(':') != std::string::npos) {
          port = host.substr(host.rfind(':')+1);
          host.resize(host.rfind(':'));
        }
	
        struct addrinfo hints, *res;
        int error, s;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = PF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        error = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
        if (error)
          log_fatal("Host resolution error (%s:%s): %s", host.c_str(),
             port.c_str(), gai_strerror(error));
        s = -1;
        for (/* struct addrinfo *r = res */; res != NULL; res = res->ai_next) {
          s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
          if (s < 0)
            continue;

          if (connect(s, res->ai_addr, res->ai_addrlen) < 0) {
            close(s);
            s = -1;
            continue;
          }

          break;
        }
        if (res != NULL)
          freeaddrinfo(res);

        if (s < 0)
          log_fatal("Could not connect to %s:%s: %s", host.c_str(), port.c_str(), strerror(errno));
        log_info("Connect to %s:%s opened successfully", host.c_str(), port.c_str());
#if BOOST_VERSION < 104400
        boost::iostreams::file_descriptor_source fs(s, true);
#else
        boost::iostreams::file_descriptor_source fs(s, boost::iostreams::close_handle);
#endif
	ifs.push(fs);
      } else {
#ifdef I3_WITH_MMAPED_FILE_SOURCE
	io::mapped_file_source fs(filename);
#else
	io::file_source fs(filename);
#endif
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

      if (ends_with(filename,".gz"))
	{
	  ofs.push(io::gzip_compressor(compression_level));
	  log_trace("Input file ends in .gz.  Using gzip decompressor.");
	}
      else if (ends_with(filename,".bz2"))
	{
	  ofs.push(io::bzip2_compressor(compression_level));
	  log_trace("Input file ends in .bz2.  Using bzip2 decompressor.");
	}
      else
	{
	  log_trace("Input file doesn't end in .gz or .bz2.  Not decompressing.");
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

