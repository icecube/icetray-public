/**
 *  $Id: I3Frame.h 51748 2009-01-08 14:55:14Z troy $
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
 */
#ifndef ICETRAY_I3FRAME_H_INCLUDED
#define ICETRAY_I3FRAME_H_INCLUDED

#include <deque>
#include <string>
#include <vector>
#include <I3/hash_map.h>
#include <stdint.h>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_const.hpp>

#include <icetray/I3DefaultName.h>
#include <icetray/I3FrameObject.h>
#include <icetray/I3Logging.h>
//#include <icetray/I3Tray.h>
#include <icetray/IcetrayFwd.h>
#include <icetray/is_shared_ptr.h>
#include <I3/name_of.h>

const int i3frame_version = 6;

/**
   The I3Frame is the container that I3Modules use to communicate with
   one another.  It is passed from I3Module to I3Module by the icetray
   framework.
*/
class I3Frame 
{
 public:
  /**
     The I3Frame takes these in the constructor.  They indicate what
     stream this frame is on (in other words, which I3Module functions
     should get called as this frame goes downstream: Physics(),
     Geometry(), Calibration(), etc.
     
     These *were* just an enum... But if streams change in the future,
     we want to be backwards-compatible with how we serialize these
     guys, and that means incrementing the version number on the
     serialization method for these.
  */
  struct Stream
  {
    Stream() : value('N') { }
    /* explicit */ Stream(char c) : value(c) { }
    Stream(const Stream& rhs) : value(rhs.value) { }
    Stream& operator=(const Stream& rhs) 
    { 
      if (this != &rhs) 
        value = rhs.value; 
      return *this; 
    }

    std::string str() const;
    template <typename Archive> 
    void serialize(Archive& ar, unsigned);

    bool operator==(const Stream& rhs) const { return value == rhs.value; }
    bool operator!=(const Stream& rhs) const { return value != rhs.value; }
    bool operator<(const Stream& rhs) const { return value < rhs.value; }

    char id() const { return value; }

  private:

    char value;

  };

  const static Stream None;
  const static Stream Geometry;
  const static Stream Calibration;
  const static Stream DetectorStatus;
  const static Stream DAQ;
  const static Stream Physics;
  const static Stream TrayInfo;

 private:
  /// This is the type of the map with which the I3Frame is
  /// implemented.  It maps strings to shared-pointers-to-I3FrameObjects.
  struct blob_t
  {
    std::string type_name;
    std::vector<char> buf;
    void reset() {
      type_name = "";
      std::vector<char>().swap(buf); // special brute-force-clear
    }
  };

  struct value_t
  {
    blob_t blob;
    I3FrameObjectConstPtr ptr;
    I3Frame::Stream stream;
  };

#if 1 //GCC_VERSION <= 40300
  typedef __gnu_cxx::hash_map<std::string, boost::shared_ptr<value_t> > map_t;
#else
  typedef std::unordered_map<std::string, boost::shared_ptr<value_t> > map_t;
#endif
  /// may change type_name field in value
  static std::string type_name(const value_t&);


  Stream stop_;

  /// Policy: Drop the blobs after you deserialize them.  Do this if
  /// you're low on memory.  You would not do this (as in the JEB),
  /// that you're just going to have to serialize them again.
  bool drop_blobs_;

  mutable map_t map_;

  /// Internal implementation of Get, where the actual deserialization
  /// is done.
  I3FrameObjectConstPtr get_impl(map_t::const_reference value,
                                 bool quietly = false) const;

  map_t::size_type size(const value_t& value) const { return value.blob.buf.size(); }

 public:
  struct typename_transform
  {
    typedef std::pair<std::string, std::string> pair_t;
    typedef const pair_t& result_type;


    mutable pair_t result;
    result_type operator()(map_t::const_reference pr) const
    {
      result.first = pr.first;
      result.second = I3Frame::type_name(*pr.second);

      return result;
    }
  };
  typedef boost::transform_iterator<typename_transform, 
                                    map_t::const_iterator,
                                    const std::pair<std::string, std::string>&
                                    > typename_iterator_t;

  class deserialize_transform 
  {
   private:
    typedef std::pair<std::string, I3FrameObjectConstPtr> pair_t;


    const I3Frame* frame_;
    mutable pair_t result;

   public:
    typedef const pair_t& result_type;


    deserialize_transform() : frame_(0) { }
    explicit deserialize_transform(const I3Frame* frame) : frame_(frame) { }
    result_type operator()(map_t::const_reference pr) const
    {
      result.first = pr.first;
      result.second = frame_->get_impl(pr);

      return result;
    }
  };
  typedef boost::transform_iterator<deserialize_transform, 
                                    map_t::const_iterator,
                                    const std::pair<std::string, I3FrameObjectConstPtr>&
                                    > deserialize_iterator;

  struct typename_iterator : typename_iterator_t { 
    typename_iterator(const map_t::const_iterator& iter)
      : typename_iterator_t(iter) 
    { }
  };

  class const_iterator : public deserialize_iterator 
  {
   public:
    const_iterator() { }
    const_iterator(const map_t::const_iterator& iter, const I3Frame* frame) :
      deserialize_iterator(iter, deserialize_transform(frame))
    { }
    const_iterator& operator=(const const_iterator& rhs)
    {
      ((deserialize_iterator*)this)->operator=(rhs);
      return *this;
    }
  };

  typedef map_t::size_type size_type;


  explicit I3Frame(Stream stop = I3Frame::None);
  explicit I3Frame(char stop);
  ~I3Frame() { }
  I3Frame(const I3Frame& rhs);
  I3Frame& operator=(const I3Frame& rhs);

  Stream GetStop() const { return stop_; }
  void SetStop(Stream newstop) { stop_ = newstop; }

  bool drop_blobs() const { return drop_blobs_; }
  /** Determine policy: Drop the blobs after deserialization?
   * 
   * @param drop True corresponds to <em>drop blobs</em>.
   */
  void drop_blobs(bool drop) { drop_blobs_ = drop; }

  size_type size() const { return map_.size(); }
  void clear() { map_.clear(); }

  // just calls operator=
  void assign(const I3Frame& rhs);

  const_iterator begin() const { return const_iterator(map_.begin(), this); } 
  const_iterator end() const { return const_iterator(map_.end(), this); } 

  typename_iterator typename_begin() const { return typename_iterator(map_.begin()); }
  typename_iterator typename_end() const { return typename_iterator(map_.end()); }
  typename_iterator typename_find(const std::string& key) const 
  { 
    return typename_iterator(map_.find(key));
  }

  size_type size(const std::string& key) const;
  size_type count(const std::string& key) const { return map_.count(key); }
  const_iterator find(const std::string& key) const 
  { 
    return const_iterator(map_.find(key), this); 
  }
  /** Test, if a frame object exists at a given "slot".
   * 
   * @param key The "slot" in the frame to check.
   * @return true, if something exists in the frame at slot <VAR>key</VAR>, otherwise false.
   */
  bool Has(const std::string& key) const { return map_.count(key); }
  bool Has(const std::string& key, const Stream& stream) const;

  I3Frame::Stream GetStop(const std::string& key) const;

  void merge(const I3Frame& rhs);

  // delete any frame objects we're carrying that are on stream 'what'
  void purge(const Stream& what);

  // delete all frame objects we're carrying that aren't on our stream 
  void purge();

  void swap(I3Frame& rhs);

  //
  //  takes "what" from rhs into *this as "as", buffers and all.
  //
  void take(const I3Frame& rhs, const std::string& what, const std::string& as);
  //
  //  takes "what" from rhs into *this, buffers and all.
  //
  void take(const I3Frame& rhs, const std::string& what)
  {
    take(rhs, what, what);
  }
  /** Get a frame object.
   * 
   * \note
   * This version of Get is called when the template argument is a
   * shared_ptr.  
   *
   * @param name This is the name of the object in the frame.  That
   * is, the key into the map under which the object is stored.  This
   * is not necessarily the same as the name of the type of the
   * object...  e.g. there might be multiple I3Particles stored at
   * different names like "linefit" and "dipolefit", and there might
   * be only one I3Geometry stored at "I3Geometry".
   * @param quietly Be quiet or not
   * @return shared_ptr<const T::value_type>.  That is, if Get is
   * called as Get<shared_ptr<const T> >(name), or the equivalent
   * Get<I3SomethingConstPtr>(name), this function returns
   * shared_ptr<const T> or I3SomethingConstPtr.  If T is a nonconst
   * shared_ptr, const is added: e.g. Get<I3SomethingPtr>(name)
   * returns I3SomethingConstPtr.
   * More generally, Get<shared_ptr<T> >(name) returns shared_ptr<const T>.
   */
  template <typename T>
  T
  Get(const std::string& name = I3DefaultName<typename T::value_type>::value(),
      bool quietly = false,
      typename boost::enable_if<is_shared_ptr<T> >::type * = 0,
      typename boost::enable_if<boost::is_const<typename T::value_type> >::type* = 0) const
  {
    log_trace("Get<%s>(\"%s\")", I3::name_of<T>().c_str(), name.c_str());

    map_t::iterator iter = map_.find(name);
    if (iter == map_.end())
      return boost::shared_ptr<typename T::value_type>();

    I3FrameObjectConstPtr focp = get_impl(*iter, quietly);
    
    return boost::dynamic_pointer_cast<typename T::value_type>(focp);
  }
  /** Get a frame object.
   * 
   * \note
   * This version of Get is called when the template argument is not a shared pointer.
   * It returns a const reference to T.
   */
  template <typename T>
  const T&
  Get(const std::string& name = I3DefaultName<T>::value(), 
      typename boost::disable_if<is_shared_ptr<T> >::type * = 0) const
  {
    log_trace("Get<%s>(\"%s\")", I3::name_of<T>().c_str(), name.c_str());

    boost::shared_ptr<const T> sp_t = this->template Get<boost::shared_ptr<const T> >(name);
    if (!sp_t)
      log_fatal("object in frame at \"%s\" doesn't exist "
                "or won't dynamic cast to type \"%s\"",
                name.c_str(), I3::name_of<T>().c_str());
    else if (sp_t.unique())
      log_fatal("cannot get synthetic frame object \"%s\" (\"%s\") "
                "by reference, only by shared pointer",
                name.c_str(), I3::name_of<T>().c_str());
    else
      return *sp_t;
  }

  /** Puts something into the frame.
   * 
   * @param name Where to put it.
   * @param element What to put in there. 
   * Must be a shared_ptr to something that inherits from I3FrameObject.
   * @param stream The stream to act on.
   * @returns Nothing.
   * @throw If something exists already in the frame at key <VAR>name</VAR>,
   * this function will throw via log_fatal.
   */
  void Put(const std::string& name, 
	   boost::shared_ptr<const I3FrameObject> element,
	   const I3Frame::Stream& stream);

  void Put(const std::string& name, 
	   boost::shared_ptr<const I3FrameObject> element);

  /** Puts something into the frame at its "default" location.
   *
   * @param element What to put in there.
   * @returns Nothing.
   * @throw Throws via log_fatal if something is already in the frame at the default
   * location for T.
   */
  template <typename T>
  void Put(boost::shared_ptr<T> element)
  {
    Put(I3DefaultName<T>::value(),element);
  }

  ///
  ///  Renames something.
  ///
  void Rename(const std::string& from, const std::string& to);

  ///
  ///  Changes a frame object's stream.
  void ChangeStream(const std::string& key, Stream stream);

  ///
  /// Deletes something.  
  ///
  void Delete(const std::string& key);

  std::string as_xml(const std::string& key) const;

  const std::type_info* type_id(const std::string& key, bool quietly = false) const;
  std::string type_name(const std::string& key) const;

  std::vector<std::string> keys() const;

  template <typename OStreamT>
  void 
  save(OStreamT& os, const std::vector<std::string>& vs = std::vector<std::string>(), int version = i3frame_version) const;

  template <typename IStreamT>
  bool 
  load(IStreamT& is, const std::vector<std::string>& vs = std::vector<std::string>(), bool verify_checksums = true);

  std::string Dump() const;

  ///
  ///  these are used from unit tests to be sure frame is
  /// (de)serializing properly
  ///
#ifdef I3_I3FRAME_TESTING
  bool has_blob(const std::string& name) const
  {
    map_t::const_iterator iter = map_.find(name);
    if (iter == map_.end())
      return false;
    return iter->second->blob.buf.size() != 0;
  }
  bool has_ptr(const std::string& name) const
  {
    map_t::const_iterator iter = map_.find(name);
    if (iter == map_.end())
      return false;
    return iter->second->ptr;
  }
#endif

 private:

  template <typename IStreamT>
  bool load_old(IStreamT& ifs, const std::vector<std::string>& skip, 
		unsigned version);

  template <typename IStreamT>
  bool load_v4(IStreamT& ifs, const std::vector<std::string>& skip);

  template <typename IStreamT>
  bool load_v56(IStreamT& ifs, const std::vector<std::string>& skip, bool v6,
       bool verify_checksums);


  friend std::ostream& operator<<(std::ostream& o, const I3Frame& frame);
};

std::ostream& operator<<(std::ostream& os, const I3Frame::Stream& stream);
std::ostream& operator<<(std::ostream& os, const I3Frame& frame);

I3_POINTER_TYPEDEFS(I3Frame);

typedef std::deque<I3FramePtr> FrameFifo;
I3_POINTER_TYPEDEFS(FrameFifo);

#endif // ICETRAY_I3FRAME_H_INCLUDED
