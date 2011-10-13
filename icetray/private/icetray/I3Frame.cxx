/**
 *  $Id: I3Frame.h 41943 2008-02-07 15:14:07Z tschmidt $
 *  
 *  Copyright (C) 2007-8
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
#include <boost/crc.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_pod.hpp>

#include <icetray/serialization.h>
#include <icetray/Utility.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Tray.h>


using namespace std;
namespace io = boost::iostreams;
namespace ar = boost::archive;

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
    return (boost::format("Frame type %c") % value).str();
  }
}

const I3Frame::Stream I3Frame::None('N');
const I3Frame::Stream I3Frame::Geometry('G');
const I3Frame::Stream I3Frame::Calibration('C');
const I3Frame::Stream I3Frame::DetectorStatus('D');
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
      keys_.push_back(iter->first);
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



void
I3Frame::assign(const I3Frame& rhs)
{
  *this = rhs;
}

void I3Frame::purge(const Stream& what)
{
  std::vector<std::string> keys;
  for (map_t::const_iterator it = map_.begin();
       it != map_.end();
       it++)
    if (it->second->stream == what)
      keys.push_back(it->first);
  for (unsigned i=0; i< keys.size(); i++)
    Delete(keys[i]);
}

void I3Frame::purge()
{
  std::vector<std::string> keys;
  for (map_t::const_iterator it = map_.begin();
       it != map_.end();
       it++)
    if (it->second->stream != stop_)
      keys.push_back(it->first);
  for (unsigned i=0; i< keys.size(); i++)
    Delete(keys[i]);
}

void I3Frame::merge(const I3Frame& rhs)
{
  for(map_t::const_iterator it = rhs.map_.begin();
      it != rhs.map_.end();
      it++)
    {
      if (map_.find(it->first) == map_.end())
	map_[it->first] = it->second;
    }
}


void I3Frame::swap(I3Frame& rhs)
{
  map_.swap(rhs.map_);
}

void I3Frame::take(const I3Frame& rhs, const string& what, const string& as)
{
  map_t::const_iterator iter = rhs.map_.find(what);
  if (iter != rhs.map_.end())
    map_[as] = iter->second;
  else
    log_fatal("attempt to take \"%s\" from a frame that doesn't have one", what.c_str());
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

  // this should be more exhausive than just space tab newline.
  if (name.size() == 0)
    log_fatal("attempt to Put an element into frame with an empty string used as the key");
  if (name.find_first_of(" \t\n") != string::npos)
    log_fatal("attempt to Put an element into frame at name \"%s\", "
              "which contains an illegal whitespace character",
              name.c_str());
  
  boost::shared_ptr<value_t> sptr(new value_t);
  map_[name] = sptr;
  value_t& value = *sptr;
  value.ptr = element;
  value.stream = on_stream;
}

void I3Frame::Rename(const string& fromname, const string& toname)
{
  map_t::const_iterator fromiter = map_.find(fromname);
  if (fromiter == map_.end())
    log_fatal("attempt to rename \"%s\" to \"%s\", but the source is empty",
              fromname.c_str(), toname.c_str());

  map_t::const_iterator toiter = map_.find(toname);
  if (toiter != map_.end())
    log_fatal("attempt to rename \"%s\" to \"%s\", but the destination is already full",
              fromname.c_str(), toname.c_str());

  map_[toname] = map_[fromname];
  map_.erase(fromname);

}

void I3Frame::ChangeStream(const string& key, I3Frame::Stream stream)
{
  map_t::const_iterator fromiter = map_.find(key);
  if (fromiter == map_.end())
    log_fatal("attempt to change stream of \"%s\", but it doesn't exist",
      key.c_str());

  fromiter->second->stream = stream;
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
    I3FrameObjectConstPtr focp = Get<I3FrameObjectConstPtr>(key, true);
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

const type_info* I3Frame::type_id(const string& key, bool quietly) const
{
  map_t::const_iterator iter = map_.find(key);
  if (iter == map_.end())
    return NULL;
  const I3FrameObject* fo = get_impl(*iter, quietly).get();
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

  *strp = value.ptr ? I3::name_of(typeid(*(value.ptr.get()))) : "(null)";

  // and return that
  return value.blob.type_name;
}

string I3Frame::Dump() const
{
  ostringstream o;
  o << *this;
  return o.str();
}

namespace 
{
  typedef boost::crc_optimal<16, 0x1021, 0xFFFF, 0, false, false> crc_t;

  template <typename T, typename CRC>
  inline void
  crcit (const T& container, CRC& crc, bool orly = true, 
	 typename boost::disable_if<boost::is_pod<T> >::type* foo = 0)
  {
    if (! orly)
      return;
    uint32_t size = container.size();
#ifdef BOOST_PORTABLE_BINARY_ARCHIVE_BIG_ENDIAN
    uint32_t swapped = size;
    boost::archive::portable::swap_impl<sizeof(size)>::swap(swapped);
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
#ifdef BOOST_PORTABLE_BINARY_ARCHIVE_BIG_ENDIAN
    T swapped = pod;
    boost::archive::portable::swap_impl<sizeof(T)>::swap(swapped);
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

const static i3frame_version_t version = 5;

template <typename OStreamT>
void I3Frame::save(OStreamT& os, const vector<string>& skip) const
{
  crc_t crc;

  os.write(tag, sizeof(i3frame_tag_t));
  {
    boost::archive::portable_binary_oarchive poa(os);
    poa << make_nvp("version", version);
    // version does *not* get crc'ed

    poa << make_nvp("stream", stop_);
    crc.process_byte(stop_.id());

    // save map values in a set to check, if keys (guaranteed in a map) and pointers are
    // unique.  skip values, where key matches an element in vector skip.
    set<map_t::value_type*> mapAsSet;
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
            skipIt = boost::regex_match(iter->first, reg);
          }

	if (iter->second->stream != stop_.id())
	  skipIt = true;

        if (skipIt) continue;
        
        if (!mapAsSet.insert(&*iter).second)
          log_fatal("frame contains a duplicated pointer for \"%s\"", iter->first.c_str());
      }

    i3frame_nslots_t size = mapAsSet.size();
    poa << make_nvp("size", size);
    crcit(size, crc);
    
    for (set<map_t::value_type*>::iterator iter = mapAsSet.begin();
         iter != mapAsSet.end();
         iter++)
      {
        map_t::value_type& i = **iter;
        const string &key = i.first;
        value_t& value = *i.second;

        poa << make_nvp("key", key);
	crcit(key, crc);
        if (value.blob.buf.size()) // there's a buffer there.  use it and it's type_name.
          {
            string type_name = value.blob.type_name;
            poa << make_nvp("type_name", type_name);
	    crcit(type_name, crc);
            poa << make_nvp("buf", value.blob.buf);
	    crcit(value.blob.buf, crc);
          }
        else
          {
            value.blob.type_name = value.ptr ? I3::name_of(typeid(*(value.ptr.get()))) : "(null)";
            poa << make_nvp("type_name", value.blob.type_name);
	    crcit(value.blob.type_name, crc);
	    typedef io::stream<io::back_insert_device<vector<char> > > vecstream_t;
            vecstream_t blobBufStream(value.blob.buf);
            {
              boost::archive::portable_binary_oarchive blobBufArchive(blobBufStream);
              try {
                blobBufArchive << make_nvp("T", value.ptr);
              } catch (const exception &e) {
                log_fatal("caught \"%s\" while writing frame object \"%s\" of type \"%s\"", 
                          e.what(), key.c_str(), value.blob.type_name.c_str());
              } 
            }
            blobBufStream.flush();
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
bool I3Frame::load(IStreamT& is, const vector<string>& skip)
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
#ifdef BOOST_PORTABLE_BINARY_ARCHIVE_BIG_ENDIAN
      boost::archive::portable::swap_impl<sizeof(frameTagRead)>::swap(frameTagRead);
#endif
      const i3frame_version_t* tmpVersion =
	reinterpret_cast<i3frame_version_t*>(frameTagRead);
      if (*tmpVersion < 4)
	return load_old(is, skip, *tmpVersion);
    }

  // dispatch to load_old didn't happen. verify that this is an .i3 file
  int cmp = memcmp(frameTagRead, tag, 4);
  if (cmp != 0)
    log_fatal("Frame tag found is %c%c%c%c, not [i3] as expected.  Is this really an .i3 file?",
	      frameTagRead[0], frameTagRead[1], frameTagRead[2], frameTagRead[3]);

  // if we are here, this looks like an .i3 file.  Get the frame serialization version.
  {
    boost::archive::portable_binary_iarchive bufArchive(is);
    // this might return garbage and set eof() on stream
    i3frame_version_t versionRead;
    bufArchive >> make_nvp("i3version", versionRead);

    if (versionRead == 4)
      return load_v4(is, skip);
    if (versionRead == 5)
      return load_v5(is, skip);
    else
      log_fatal("Frame is version %u, this software can read only up to version 5", versionRead);
  }
}

//
//
//  load version 5 (latest)
//
//
template <typename IStreamT>
bool I3Frame::load_v5(IStreamT& is, const vector<string>& skip)
{
  if (!is.good())
    log_fatal("attempt to read from stream in error state");

  i3frame_checksum_t checksumRead;

  crc_t crc;
  bool calc_crc = (skip.size() == 0);

  // read size of the entire (serialized) frame
  // read checksum plus entire frame and process/test checksum
  {
    boost::archive::portable_binary_iarchive bia(is);

    bia >> make_nvp("stream", stop_);
    crcit(stop_.id(), crc, calc_crc);

    i3frame_nslots_t nslots;
    bia >> make_nvp("size", nslots);
    crcit(nslots, crc, calc_crc);

    for (unsigned int i = 0; i < nslots; i++)
      {
        string key, type_name;

        bia >> make_nvp("key", key);
	crcit(key, crc, calc_crc);

        bia >> make_nvp("type_name", type_name);
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
	    is.ignore(count);
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
	    crcit(blob.buf, crc, calc_crc);
            blob.type_name = type_name;
          }
      }

    bia >> make_nvp("checksum", checksumRead);
    if (calc_crc && (crc.checksum() != checksumRead))
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
    boost::archive::portable_binary_iarchive bia(is);

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
    //    boost::archive::portable_binary_iarchive bia(fis);

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
	    is.ignore(count);
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

  boost::archive::portable_binary_iarchive bufArchive(is);

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
	  is.ignore(count);
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
      keys.push_back(iter->first);
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




I3FrameObjectConstPtr I3Frame::get_impl(map_t::const_reference pr,
                                        bool quietly) const
{
  value_t& value = const_cast<value_t&>(*pr.second);
  if (value.ptr) 
    {
      if (drop_blobs_) value.blob.reset();
      return value.ptr;
    }
  if (!value.ptr && value.blob.buf.size() == 0)
    return I3FrameObjectConstPtr();

  io::array_source src(&(value.blob.buf[0]), value.blob.buf.size());
  io::filtering_istream fis(src);
  boost::archive::portable_binary_iarchive pia(fis);
  I3FrameObjectPtr fop;
  try {
    pia >> fop;
    value.ptr = fop;
    if (drop_blobs_)
      value.blob.reset();
  } catch (const ar::archive_exception& e) {
    if (!quietly)
      log_error("frame caught exception \"%s\" while loading class type \"%s\" "
                "at key \"%s\"", e.what(), value.blob.type_name.c_str(), pr.first.c_str());
    throw e;
  }
  
  return value.ptr;
}


template bool I3Frame::load(io::filtering_istream&, const vector<string>&);
template bool I3Frame::load(istream& is, const vector<string>&);
template bool I3Frame::load(ifstream& is, const vector<string>&);
template bool I3Frame::load(boost::interprocess::bufferstream& is, const vector<string>&);
template bool I3Frame::load(boost::interprocess::basic_vectorstream<std::vector<
char> >& is, const vector<string>&);

template void I3Frame::save(io::filtering_ostream&, const vector<string>&) const;
template void I3Frame::save(boost::interprocess::basic_vectorstream<std::vector<char> >&, const vector<string>&) const;
template void I3Frame::save(ostream&, const vector<string>&) const;
template void I3Frame::save(ofstream&, const std::vector<string>&) const;
