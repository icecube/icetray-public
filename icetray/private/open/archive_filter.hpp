#ifndef ARCHIVE_FILTER_HPP
#define ARCHIVE_FILTER_HPP
/**
 *  $Id: open.cxx 2595 2016-06-01 18:26:57Z cweaver $
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
#include <boost/iostreams/filtering_stream.hpp>
#include <archive.h>
#include <archive_entry.h>

#include <boost/algorithm/string.hpp>
#include <icetray/I3Logging.h>

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
    void* source;
    char_type buffer[BOOST_IOSTREAMS_DEFAULT_DEVICE_BUFFER_SIZE];
  };

  std::string basename_;
  boost::shared_ptr<struct archive> reader_;
  struct archive_entry* current_entry_;
  client_data source_info_;
  bool header_read_;
  bool raw_archive_;
  ssize_t bytes_read_;
    
  archive_filter(const std::string& filename);

  template <typename Source>
  std::streamsize
  read(Source& src, char_type* s, std::streamsize n);
    
};

using boost::algorithm::ends_with;

namespace{
  void archive_destructor(struct archive *ar){
    if (!ar)
      return;    
    archive_read_free(ar);
  }

  typedef enum { GZIP, BZIP2, LZMA, XZ, NONE } compression_type;

  compression_type guess_compression(const std::string &filename){
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
  
  /* A callback for libarchive. */
  template <typename Source>
    ssize_t
    read_stream(struct archive* a,
		void* client_data_blob,
		const void** buff){
    archive_filter::client_data *data = (archive_filter::client_data*)client_data_blob;
    Source& source = *((Source*)data->source);
    ssize_t nread = boost::iostreams::read(source,
					   data->buffer,
					   BOOST_IOSTREAMS_DEFAULT_DEVICE_BUFFER_SIZE);
    *buff = data->buffer;
    
    if (nread < 0)
      return 0; /* libarchive-style EOF */
    else
      return nread;
  }
  
};
    
archive_filter::archive_filter(const std::string& filename) :
  reader_(archive_read_new(), archive_destructor),
  header_read_(false),
  raw_archive_(false),
  bytes_read_(0)
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

  size_t p1 = filename.rfind('/');
  if (p1 != std::string::npos) {
    size_t p2 = filename.find('.', p1+1);
    if (p2 != std::string::npos) {
      basename_ = filename.substr(p1+1,p2-p1-1);
    } else {
      basename_ = filename.substr(p1+1,filename.size()-p1-1);
    }
  }
  log_trace("(archive_filter) basename_ = %s", basename_.c_str());
  source_info_.source = NULL;
}

template <typename Source>
std::streamsize
archive_filter::read(Source& src, char_type* s, std::streamsize n)
{
  if (source_info_.source != &src) {
    source_info_.source = (void*)&src;

    int err = archive_read_open(reader_.get(),
				&source_info_,
				NULL,
				&read_stream<Source>,
				NULL);
    if (err != ARCHIVE_OK)
      log_fatal("archive_read_open() failed: %s", archive_error_string(reader_.get()));
    raw_archive_ = false;
    log_trace("(archive_filter) opened new source");
  }
  
  while (!header_read_ && !raw_archive_) {
    if (archive_read_next_header(reader_.get(), &current_entry_) == ARCHIVE_OK) {
      std::string fname(archive_entry_pathname(current_entry_));
      log_trace("(archive_filter) fname=%s", fname.c_str());

      /*
       * NB: libarchive exposes raw data as an archive with a
       * single entry whose pathname is either the filename or "data".
       */
      if (fname == "data" || fname == basename_) {
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

#endif // ARCHIVE_FILTER_HPP
