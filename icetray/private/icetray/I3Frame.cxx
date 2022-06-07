/**
 *  $Id$
 *  
 *  Copyright (C) 2007-8
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

#include <algorithm>
#include <fstream>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/interprocess/streams/bufferstream.hpp>
#include <boost/interprocess/streams/vectorstream.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_pod.hpp>

#include <icetray/serialization.h>
#include <icetray/Utility.h>
#include <icetray/I3Frame.h>

#include "crc-ccitt.h"

// working around a libc++ bug in istream::ignore()
// http://llvm.org/bugs/show_bug.cgi?id=16427
#if defined(_LIBCPP_VERSION) && (_LIBCPP_VERSION <= 1101)
#define WORKAROUND_LIBCPP_ISTREAM_IGNORE_ISSUE
namespace {
    template <typename IStreamT>
    inline IStreamT& istream_ignore_workaround(IStreamT& is, std::streamsize n)
    {
        std::streamsize count = n;
        
        const std::streamsize buf_size = 10240;
        char buffer[buf_size];
        while (count > 0) {
            std::streamsize read_bytes = std::min(buf_size, count);
            is.read(buffer, read_bytes);
            count -= read_bytes;
        }

        return is;
    }
}
#endif

using namespace std;
namespace io = boost::iostreams;
namespace ar = icecube::archive;

template <class Archive>
void I3Frame::Stream::serialize(Archive& ar, unsigned version)
{
  ar & make_nvp("value", value);
}

I3_BASIC_SERIALIZABLE(I3Frame::Stream);


string I3Frame::Stream::str() const {
  switch (value) {
  case 'N':
    return "None";
    break;
  case 'G':
    return "Geometry";
    break;
  case 'C':
    return "Calibration";
    break;
  case 'D':
    return "DetectorStatus";
    break;
  case 'S':
    return "Simulation";
    break;
  case 'Q':
    return "DAQ";
    break;
  case 'P':
    return "Physics";
    break;
  case 'I':
    return "TrayInfo";
    break;
  default:
    return (boost::format("('%c')") % value).str();
  }
}

const I3Frame::Stream I3Frame::None('N');
const I3Frame::Stream I3Frame::Geometry('G');
const I3Frame::Stream I3Frame::Calibration('C');
const I3Frame::Stream I3Frame::DetectorStatus('D');
const I3Frame::Stream I3Frame::Simulation('S');
const I3Frame::Stream I3Frame::DAQ('Q');
const I3Frame::Stream I3Frame::Physics('P');
const I3Frame::Stream I3Frame::TrayInfo('I');


///
/// Legacy Code:
/// A "Frame Event", such as Get, Put, Delete, Rename.
///
///
namespace {
  struct event_t
  {
    char op;
    std::string module, key_1, key_1_type, key_2;

    template <typename Archive>
    void serialize(Archive& ar, unsigned version)
    {
      ar & make_nvp("module", module);
      ar & make_nvp("op", op);
      ar & make_nvp("key1", key_1);
      ar & make_nvp("key1type", key_1_type);
      ar & make_nvp("key2", key_2);
    }
  };
  I3_BASIC_SERIALIZABLE(event_t);
}


I3Frame::I3Frame(Stream stop)
  : stop_(stop),
    drop_blobs_(true)
{ }

I3Frame::I3Frame(char stop)
  : stop_(I3Frame::Stream(stop)),
    drop_blobs_(true)
{ }

I3Frame::I3Frame(const I3Frame& rhs)
{
  *this = rhs;
}


vector<string> 
I3Frame::keys() const
{
  vector<string> keys_;
  for(I3Frame::map_t::const_iterator iter = map_.begin(); 
      iter != map_.end();
      iter++)
    {
      keys_.push_back(iter->first.string);
    }  
  std::sort(keys_.begin(), keys_.end());
  return keys_;
}

I3Frame& I3Frame::operator=(const I3Frame& rhs)
{
  if (this != &rhs)
    {
      stop_ = rhs.stop_;
      drop_blobs_ = rhs.drop_blobs_;
      map_ = rhs.map_;
    }

  return *this;
}


I3Frame::size_type I3Frame::size(const string& key) const
{
  map_t::iterator iter = map_.find(key);
  if (iter == map_.end())
    log_fatal("attempt to get size of nonexistent frame object \"%s\"", key.c_str());
  return size(*iter->second);
}

void I3Frame::purge(const Stream& what)
{
  map_t::iterator it = map_.begin();
  while (it != map_.end()) {
    if (it->second->stream == what)
      map_.erase(it++);
    else
      it++;
  }
}

void I3Frame::purge()
{
  map_t::iterator it = map_.begin();
  while (it != map_.end()) {
    if (it->second->stream != stop_)
      map_.erase(it++);
    else
      it++;
  }
}

void I3Frame::merge(const I3Frame& rhs)
{
  map_.insert(rhs.map_.begin(), rhs.map_.end());
}

void I3Frame::take(const I3Frame& rhs, const string& what, const string& as)
{
  map_t::const_iterator iter = rhs.map_.find(what);
  if (iter != rhs.map_.end())
    map_[as] = iter->second;
  else
    log_fatal("attempt to take \"%s\" from a frame that doesn't have one", what.c_str());
}

I3Frame::Stream
I3Frame::GetStop(const std::string& key) const
{
	map_t::const_iterator iter = map_.find(key);
	if (iter == map_.end())
		log_fatal("The key '%s' doesn't exist in this frame", key.c_str());
	else
		return iter->second->stream;
}

void I3Frame::validate_name(const std::string& name)
{
  // this should be more exhausive than just space tab newline.
  if (name.empty())
    log_fatal("attempt to Put an element into frame with an empty string used as the key");
  if (name.find_first_of(" \t\n") != string::npos)
    log_fatal("attempt to Put an element into frame at name \"%s\", "
              "which contains an illegal whitespace character",
              name.c_str());
}

void I3Frame::Put(const string& name, I3FrameObjectConstPtr element)
{
  Put(name, element, stop_);
}

void I3Frame::Put(const string& name, I3FrameObjectConstPtr element, const I3Frame::Stream& on_stream)
{
  map_t::iterator it = map_.find(name);
  if (it != map_.end())
    {
      log_fatal("frame already contains \"%s\", of type \"%s\"", 
                name.c_str(), type_name(name).c_str());
    }

  validate_name(name);
  
  boost::shared_ptr<value_t> sptr(new value_t);
  map_[name] = sptr;
  value_t& value = *sptr;
  value.size = 0;
  value.ptr = element;
  value.stream = on_stream;
}

void I3Frame::Replace(const std::string& name, I3FrameObjectConstPtr element)
{
  validate_name(name);
  
  auto it=map_.find(name);
  if(it==map_.end())
    log_fatal_stream("Attempt to replace object at key '" << name << 
                     "' but there is nothing there.");
  boost::shared_ptr<value_t> sptr(new value_t);
  map_[name] = sptr;
  value_t& value = *sptr;
  value.size = 0;
  value.ptr = element;
  value.stream = stop_;
}

void I3Frame::Rename(const string& fromname, const string& toname)
{
  validate_name(toname);
  
  map_t::iterator fromiter = map_.find(fromname);
  if (fromiter == map_.end())
    log_fatal("attempt to rename \"%s\" to \"%s\", but the source is empty",
              fromname.c_str(), toname.c_str());

  map_t::const_iterator toiter = map_.find(toname);
  if (toiter != map_.end())
    log_fatal("attempt to rename \"%s\" to \"%s\", but the destination is already full",
              fromname.c_str(), toname.c_str());

  map_[toname] = map_[fromname];
  map_.erase(fromiter);
}

void I3Frame::ChangeStream(const string& key, I3Frame::Stream stream)
{
  map_t::iterator fromiter = map_.find(key);
  if (fromiter == map_.end())
    log_fatal("attempt to change stream of \"%s\", but it doesn't exist",
      key.c_str());

  // Duplicate value_t to avoid potential caching issues
  boost::shared_ptr<value_t> sptr(new value_t(*fromiter->second));
  sptr->stream = stream;
  fromiter->second = sptr;
}

void I3Frame::Delete(const string& name)
{
  map_t::iterator what = map_.find(name);
  if (what != map_.end())
    map_.erase(what);
}


string I3Frame::as_xml(const string& key) const
{
  try {
    I3FrameObjectConstPtr focp = Get<I3FrameObjectConstPtr>(key);
    const I3FrameObject *tobj_p(focp.get());

    ostringstream oss;
    ar::xml_oarchive xoa(oss, ar::no_header);
    xoa << make_nvp("I3FrameObject", tobj_p);
    return oss.str();
  } catch (const ar::archive_exception& e) {
    return string("***\n***\n***\n  archive error: \"")
                  +  e.what() + "\"\n***\n***\n***";
  }
}

string I3Frame::type_name(const string& key) const
{
  map_t::iterator iter = map_.find(key);
  // first check to see if it is there, otherwise throw
  if (iter == map_.end())
    log_fatal("attempt to get type name for \"%s\", which does not exists",
              key.c_str());

  return type_name(*iter->second);
}

const type_info* I3Frame::type_id(const string& key) const
{
  map_t::const_iterator iter = map_.find(key);
  if (iter == map_.end())
    return NULL;
  const I3FrameObject* fo = get_impl(*iter).get();
  if (fo == NULL)
    return NULL;

  // typeid ignores toplevel const/volatile and works the same on 
  // pointers and references 
  return &typeid(*fo);
}

string I3Frame::type_name(const value_t& value)
{
  // if there is a type_name, return that
  if (value.blob.type_name.length() != 0)
    return value.blob.type_name;

  string* strp = const_cast<string*>(&value.blob.type_name);

  const I3FrameObject& obj=*(value.ptr.get());
  *strp = value.ptr ? I3::name_of(typeid(obj)) : "(null)";

  // and return that
  return value.blob.type_name;
}

string I3Frame::Dump() const
{
  ostringstream o;
  o << *this;
  return o.str();
}

extern "C" unsigned long crc32c(unsigned long crc, const uint8_t *buf, unsigned int len);

namespace 
{
  typedef struct crc_ {
    uint32_t crc;
    bool is_crc32;

    crc_(bool crc32 = true) : crc(crc32 ? 0 : 0xffff), is_crc32(crc32) {}
    uint32_t checksum() { return crc; }
    inline void process_bytes(const void *bytes, size_t size) {
       if (is_crc32)
         crc = crc32c(crc, (const uint8_t *)bytes, size);
       else
         crc = crc_ccitt(crc, (const uint8_t *)bytes, size);
    }
    inline void process_byte(uint8_t byte) {
       if (is_crc32)
         crc = crc32c(crc, &byte, 1);
       else
         crc = crc_ccitt_byte(crc, byte);
    }
  } crc_t;

  template <typename T, typename CRC>
  inline void
  crcit (const T& container, CRC& crc, bool orly = true, 
	 typename boost::disable_if<boost::is_pod<T> >::type* foo = 0)
  {
    if (! orly)
      return;
    uint32_t size = container.size();
#if BYTE_ORDER == BIG_ENDIAN
    uint32_t swapped = size;
    icecube::archive::portable::swap(swapped);
    crc.process_bytes(&swapped, sizeof(size));
#else
    crc.process_bytes(&size, sizeof(size));
#endif
    crc.process_bytes(&(container[0]), size);
  }

  template <typename T, typename CRC>
  inline void 
  crcit (const T& pod, CRC& crc, bool orly = true, 
	 typename boost::enable_if<boost::is_pod<T> >::type* foo = 0)
  {
    if (!orly)
      return;
#if BYTE_ORDER == BIG_ENDIAN
    T swapped = pod;
    icecube::archive::portable::swap(swapped);
    crc.process_bytes(&swapped, sizeof(T));
#else
    crc.process_bytes(&pod, sizeof(T));
#endif
  }
}    


typedef uint64_t i3frame_size_t;
typedef uint32_t i3frame_checksum_t;
typedef uint32_t i3frame_version_t; // this could be uint8_t, but is 32 for hysterical reasons
typedef uint32_t i3frame_nslots_t;

typedef char i3frame_tag_t[4];
const static i3frame_tag_t tag = { '[', 'i', '3', ']' };

const static i3frame_version_t version = 6;


void I3Frame::create_blob_impl(I3Frame::value_t &value)
{
  const I3FrameObject& obj=*(value.ptr.get());
  value.blob.type_name = value.ptr ? I3::name_of(typeid(obj)) : "(null)";

  typedef io::stream<io::back_insert_device<vector<char> > > vecstream_t;
  vecstream_t blobBufStream(value.blob.buf);
  {
    icecube::archive::portable_binary_oarchive blobBufArchive(blobBufStream);
    blobBufArchive << make_nvp("T", value.ptr);
  }
  blobBufStream.flush();
  value.size = value.blob.buf.size();
}

void I3Frame::create_blob(bool drop_memory_data, const std::string &key) const
{
  map_t::const_iterator iter = map_.find(key);
  if (iter == map_.end())
    log_fatal("Tried to create a blob for unknown key %s", key.c_str());
  value_t& value = *(iter->second);

  if (value.blob.buf.size() == 0) {
    // only create a blob if there is none yet
    try {
      create_blob_impl(value);
    } catch (const exception &e) {
      log_fatal("caught \"%s\" while writing frame object \"%s\" of type \"%s\"",
                e.what(), key.c_str(), value.blob.type_name.c_str());
    }
  }

  if (drop_memory_data) {
    // drop the memory shared pointer if requested
    value.ptr.reset();
  }
}

void I3Frame::create_blobs(bool drop_memory_data, const std::vector<std::string>& skip) const
{
  for (map_t::iterator iter = map_.begin();
       iter != map_.end();
       iter++)
  {
    bool skipIt = false;
    for (vector<string>::const_iterator skipIter = skip.begin();
         !skipIt && (skipIter != skip.end());
         skipIter++)
      {
        boost::regex reg(*skipIter);
        skipIt = boost::regex_match(iter->first.string, reg);
      }

    if (iter->second->stream != stop_.id())
      skipIt = true;

    if (skipIt) continue;

    create_blob(drop_memory_data, iter->first.string);
  }
}

template <typename OStreamT>
void I3Frame::save(OStreamT& os, const vector<string>& skip) const
{
  crc_t crc;

  os.write(tag, sizeof(i3frame_tag_t));
  {
    icecube::archive::portable_binary_oarchive poa(os);
    poa << make_nvp("version", version);
    // version does *not* get crc'ed

    poa << make_nvp("stream", stop_);
    crc.process_byte(stop_.id());

    // save map values in a set to check, if keys (guaranteed in a map) and pointers are
    // unique.  skip values, where key matches an element in vector skip.
    std::set<std::string> mapAsSet;
    for (map_t::iterator iter = map_.begin();
         iter != map_.end();
         iter++)
      {
        bool skipIt = false;
        for (vector<string>::const_iterator skipIter = skip.begin();
             !skipIt && (skipIter != skip.end());
             skipIter++)
          {
            boost::regex reg(*skipIter);
            skipIt = boost::regex_match(iter->first.string, reg);
          }

        if (iter->second->stream != stop_.id())
          skipIt = true;

        if (skipIt) continue;

        mapAsSet.insert(iter->first.string);
      }

    i3frame_nslots_t size = mapAsSet.size();
    poa << make_nvp("size", size);
    crcit(size, crc);

    for (std::set<std::string>::iterator iter = mapAsSet.begin();
         iter != mapAsSet.end();
         iter++)
      {
        const string &key = *iter;
        value_t& value = *map_[key];

        poa << make_nvp("key", key);
        crcit(key, crc);
        if (value.blob.buf.size()) // there's a buffer there.  use it and its type_name.
          {
            string type_name = value.blob.type_name;
            poa << make_nvp("type_name", type_name);
            crcit(type_name, crc);
            poa << make_nvp("buf", value.blob.buf);
            crcit(value.blob.buf, crc);
          }
        else
          {
              try {
              create_blob_impl(value);
              } catch (const exception &e) {
                log_fatal("caught \"%s\" while writing frame object \"%s\" of type \"%s\"", 
                          e.what(), key.c_str(), value.blob.type_name.c_str());
              } 

            string type_name = value.blob.type_name;
            poa << make_nvp("type_name", type_name);
            crcit(type_name, crc);
            poa << make_nvp("buf", value.blob.buf);
            crcit(value.blob.buf, crc);

            if (drop_blobs_)
              value.blob.reset();
          }
      }
    i3frame_checksum_t sum = crc.checksum();
    poa << make_nvp("checksum", sum);
  }
}


//
//  Toplevel load interface, dispatches to versioned versions
//
template <typename IStreamT>
bool I3Frame::load(IStreamT& is, const vector<string>& skip, bool verify_cksum)
{
  if (!is.good())
    log_fatal("attempt to read from stream in error state");

  // read and verify frame tag "[i3]", if not get version and dispatch to load_old
  i3frame_tag_t frameTagRead;
  is.read(frameTagRead, sizeof(i3frame_tag_t));
  if (is.eof())
    return false;

  //
  //  There might not be a frame tag '[i3]' at the top of the file,
  //  if not, check to see if it is i3_frame_version_t type with value < 4
  //  and if so call the load_old serialization routine.
  //
  if (frameTagRead[0] != tag[0])
    {  
      // reinterpret tag as version #
#if BYTE_ORDER == BIG_ENDIAN
      BOOST_STATIC_ASSERT(sizeof(frameTagRead) == 4);
      uint32_t &frameTagInt = reinterpret_cast<uint32_t &>(frameTagRead);
      icecube::archive::portable::swap(frameTagInt);
#endif
      const i3frame_version_t* tmpVersion =
	reinterpret_cast<i3frame_version_t*>(frameTagRead);
      if (*tmpVersion < 4)
	return load_old(is, skip, *tmpVersion);
    }

  // dispatch to load_old didn't happen. verify that this is an .i3 file
  int cmp = memcmp(frameTagRead, tag, 4);
  if (cmp != 0){
    log_debug("Frame tag found is %c%c%c%c, not [i3] as expected.  Is this really an .i3 file?",	      
	      frameTagRead[0], frameTagRead[1], frameTagRead[2], frameTagRead[3]);
    log_fatal("Your I3File is corrupt.");
  }

  // if we are here, this looks like an .i3 file.  Get the frame serialization version.
  {
    icecube::archive::portable_binary_iarchive bufArchive(is);
    // this might return garbage and set eof() on stream
    i3frame_version_t versionRead;
    bufArchive >> make_nvp("i3version", versionRead);

    if (versionRead == 4)
      return load_v4(is, skip);
    else if (versionRead == 5 || versionRead == 6)
      return load_v56(is, skip, versionRead == 6, verify_cksum);
    else
      log_fatal("Frame is version %u, this software can read only up to version %d", versionRead, version);
  }

  return false;
}

//
//
//  load versions 5 and 6 (latest)
//  these differ in using CRC16-CCITT vs. CRC32
//
//
template <typename IStreamT>
bool I3Frame::load_v56(IStreamT& is, const vector<string>& skip, bool v6, bool verify)
{
  if (!is.good())
    log_fatal("attempt to read from stream in error state");

  i3frame_checksum_t checksumRead;

  crc_t crc(v6);
  bool calc_crc = (skip.size() == 0);

  // read size of the entire (serialized) frame
  // read checksum plus entire frame and process/test checksum
  {
    icecube::archive::portable_binary_iarchive bia(is);

    bia >> make_nvp("stream", stop_);
    if (verify)
      crcit(stop_.id(), crc, calc_crc);

    i3frame_nslots_t nslots;
    bia >> make_nvp("size", nslots);
    if (verify)
      crcit(nslots, crc, calc_crc);
#ifdef USING_GCC_EXT_HASH_MAP
    map_.resize(nslots);
#else
    map_.reserve(nslots);
#endif

    for (unsigned int i = 0; i < nslots; i++)
      {
        string key, type_name;

        bia >> make_nvp("key", key);
        if (verify)
	  crcit(key, crc, calc_crc);

        bia >> make_nvp("type_name", type_name);
        if (verify)
	  crcit(type_name, crc, calc_crc);

        bool skipIt = false;
        for (vector<string>::const_iterator skipIter = skip.begin();
             !skipIt && (skipIter != skip.end());
             skipIter++)
          {
            boost::regex reg(*skipIter);
            skipIt = boost::regex_match(key, reg);
          }

        if (skipIt)
          {
	    uint32_t count;
	    bia >> make_nvp("count", count);
#ifdef WORKAROUND_LIBCPP_ISTREAM_IGNORE_ISSUE
	    istream_ignore_workaround(is, count);
#else
	    is.ignore(count);
#endif
          }
        else
          {
            boost::shared_ptr<value_t> vp(new value_t);
	    vp->stream = stop_.id();
            map_[key] = vp;
            blob_t& blob = vp->blob;
	    try {
	      bia >> make_nvp("buf", blob.buf);
	    } catch (const std::bad_alloc& e) {
	      log_fatal("Fatal length error while trying to deserialize object '%s' of type %s: object exceeds its maximum permitted size.", key.c_str(), type_name.c_str());
	    }
            if (blob.buf.size() == 0)
              log_fatal("read a zero-size buffer from input stream?");
            if (verify)
	      crcit(blob.buf, crc, calc_crc);
            blob.type_name = type_name;
            vp->size = blob.buf.size();
          }
      }

    bia >> make_nvp("checksum", checksumRead);
    if (verify && calc_crc && (crc.checksum() != checksumRead))
      log_fatal("checksums don't match");
  }

  return true;
}


//
//
//  load version 4 frames, which have the checksum at the beginning
//
//
template <typename IStreamT>
bool I3Frame::load_v4(IStreamT& is, const vector<string>& skip)
{
  if (!is.good())
    log_fatal("attempt to read from stream in error state");

  i3frame_size_t sizeRead;
  i3frame_checksum_t checksumRead;

  // read version and process it - version < 4 needs to run load_old
  // read size of the entire (serialized) frame
  // read checksum plus entire frame and process/test checksum
  {
    icecube::archive::portable_binary_iarchive bia(is);

    bia >> make_nvp("size", sizeRead);
    bia >> make_nvp("checksum", checksumRead);

    //
    //  The checksumming that used to be here looked like this, 
    // but has been disabled as memory consumption became an issue.
    //
    // read entire frame into main buffer
    //    buf_.resize(sizeRead);
    //    io::read(is, &(buf_[0]), buf_.size());

    // verify checksum on buffer (whole frame)
    //    boost::crc_32_type crc;
    //    crc.process_bytes(&(buf_[0]), buf_.size());
    //    if (crc.checksum() != checksumRead)
    //      log_fatal("data corruption - checksums don't match");

    //    io::array_source bufSource(&(buf_[0]), buf_.size());
    //    io::filtering_istream fis(bufSource);
    //    icecube::archive::portable_binary_iarchive bia(fis);

    bia >> make_nvp("Stream", stop_);
    std::vector<event_t> tmp_history_;
    bia >> make_nvp("history", tmp_history_);

    i3frame_nslots_t size;
    bia >> make_nvp("size", size);

    for (unsigned int i = 0; i < size; i++)
      {
        string key, type_name;
        bia >> make_nvp("key", key);
        bia >> make_nvp("type_name", type_name);

        bool skipIt = false;
        for (vector<string>::const_iterator skipIter = skip.begin();
             !skipIt && (skipIter != skip.end());
             skipIter++)
          {
            boost::regex reg(*skipIter);
            skipIt = boost::regex_match(key, reg);
          }

        if (skipIt)
          {
	    uint32_t count;
	    bia >> make_nvp("count", count);
#ifdef WORKAROUND_LIBCPP_ISTREAM_IGNORE_ISSUE
	    istream_ignore_workaround(is, count);
#else
	    is.ignore(count);
#endif
          }
        else
          {
            boost::shared_ptr<value_t> vp(new value_t);
	    vp->stream = stop_.id();
            map_[key] = vp;
            blob_t& blob = vp->blob;
	    try {
	      bia >> make_nvp("buf", blob.buf);
	    } catch (const std::bad_alloc& e) {
	      log_fatal("Fatal length error while trying to deserialize object '%s' of type %s: object exceeds its maximum permitted size.", key.c_str(), type_name.c_str());
	    }
            if (blob.buf.size() == 0)
              log_fatal("read a zero-size buffer from input stream?");
            blob.type_name = type_name;
            vp->size = blob.buf.size();
          }
      }
  }

  return true;
}

//
//
//  load <= version 3 frames, which had partial checksums that we no longer bother with
//
//
template <class IStream>
bool I3Frame::load_old(IStream& is,
                       const vector<string>& skip,
                       uint32_t versionRead)
{
  if (is.eof())        
    return false;

  if (!is.good())
    log_fatal("attempt to read from stream in error state");

  icecube::archive::portable_binary_iarchive bufArchive(is);

  std::vector<event_t> history_;
  if (versionRead > 1)
    bufArchive >> make_nvp("history", history_);

  bufArchive >> make_nvp("Stream", stop_);

  i3frame_nslots_t size;
  bufArchive >> make_nvp("size", size);
      
  for (unsigned int i = 0; i < size; i++)
    {
      string key, type_name, buf;
      bufArchive >> make_nvp("key", key);
      bufArchive >> make_nvp("type_name", type_name);
      bool skipIt = false;
      for (vector<string>::const_iterator skipIter = skip.begin();
           !skipIt && (skipIter != skip.end());
           skipIter++)
        {
          boost::regex reg(*skipIter);
          skipIt = boost::regex_match(key, reg);
        }
      if (skipIt) 
        {
	  uint32_t count;
	  bufArchive >> make_nvp("count", count);
#ifdef WORKAROUND_LIBCPP_ISTREAM_IGNORE_ISSUE
	  istream_ignore_workaround(is, count);
#else
	  is.ignore(count);
#endif
	}
      else
	{
	  try {
	    bufArchive >> make_nvp("buf", buf);



	  } catch (const std::length_error& e) {
	    log_fatal("Fatal length error while trying to deserialize object '%s' of type %s: object exceeds its maximum permitted size.", key.c_str(), type_name.c_str());
	  }
	
  
	  boost::shared_ptr<value_t> spv(new value_t);
	  spv->stream = stop_.id();
	  map_[key] = spv;
	  blob_t& blob = spv->blob;
	  blob.type_name = type_name;
	  blob.buf.resize(buf.size());

	  std::copy(buf.begin(), buf.end(), blob.buf.begin());
	  spv->size = blob.buf.size();
	}
    }
  // as of version 4 this is a no-op since the iarchive itself no
  // longer contains crc code.  It didn't belong there anyhow, and
  // it had bugs (didn't actually checksum everything).  So we
  // can't verify version 3 checksums, but we can still load them.
  if (versionRead == 3)
    {
      uint64_t dummySize;
      uint32_t dummyChecksum;

      bufArchive >> make_nvp("size", dummySize);
      bufArchive >> make_nvp("checksum", dummyChecksum);
    }

  return true;
}


ostream& operator<<(ostream& os, const I3Frame::Stream& stream)
{
  os << stream.str();

  return os;
}

ostream& operator<<(ostream& os, const I3Frame& frame)
{
  // 
  //  for readability print these in sorted order.
  //
  vector<string> keys;
  for(I3Frame::map_t::const_iterator iter = frame.map_.begin(); 
      iter != frame.map_.end();
      iter++)
    {
      keys.push_back(iter->first.string);
    }  
  std::sort(keys.begin(), keys.end());

  os << "[ I3Frame  (" << frame.stop_.str() << "):\n";
  for(vector<string>::iterator iter = keys.begin(); 
      iter != keys.end();
      iter++)
    {
      os << "  '" << *iter << "' [" << frame.map_[*iter]->stream << "]"
	 << " ==> ";
      os << frame.type_name(*iter);

      if (frame.size(*iter) > 0)
	os << " (" << frame.size(*iter) << ")";
      else
	os << " (unk)";
	
      os << "\n";
    }
  os << "]\n";

  return os;
}




I3FrameObjectConstPtr I3Frame::get_impl(map_t::const_reference pr) const
{
  value_t& value = const_cast<value_t&>(*pr.second);
  if (value.ptr) 
    {
      if (drop_blobs_)
	value.blob.reset();
      
      return value.ptr;
    }
  if (!value.ptr && value.blob.buf.size() == 0)
    return I3FrameObjectConstPtr();

  io::array_source src(&(value.blob.buf[0]), value.blob.buf.size());
  io::filtering_istream fis(src);
  icecube::archive::portable_binary_iarchive pia(fis);
  I3FrameObjectPtr fop;
  try {
    pia >> fop;
    value.ptr = fop;
    if (drop_blobs_)
      value.blob.reset();
  } catch (const ar::archive_exception& e) {
      log_debug("frame caught exception \"%s\" while loading class type \"%s\" "
                "at key \"%s\"", e.what(), value.blob.type_name.c_str(), pr.first.string.c_str());
    return I3FrameObjectConstPtr();
  }
  
  return value.ptr;
}


template bool I3Frame::load(io::filtering_istream&, const vector<string>&, bool);
template bool I3Frame::load(istream& is, const vector<string>&, bool);
template bool I3Frame::load(ifstream& is, const vector<string>&, bool);
template bool I3Frame::load(boost::interprocess::bufferstream& is, const vector<string>&, bool);
template bool I3Frame::load(boost::interprocess::basic_vectorstream<std::vector<
char> >& is, const vector<string>&, bool);

template void I3Frame::save(io::filtering_ostream&, const vector<string>&) const;
template void I3Frame::save(boost::interprocess::basic_vectorstream<std::vector<char> >&, const vector<string>&) const;
template void I3Frame::save(ostream&, const vector<string>&) const;
template void I3Frame::save(ofstream&, const std::vector<string>&) const;
