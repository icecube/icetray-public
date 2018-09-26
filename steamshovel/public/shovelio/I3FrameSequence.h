#ifndef SHOVEL_I3FRAMESTREAM_H_
#define SHOVEL_I3FRAMESTREAM_H_

#include "FileGroup.h"

#include <icetray/I3Frame.h>
#include <icetray/I3FrameMixing.h>
#include <icetray/I3Logging.h>

#if !defined(NDEBUG)
#define BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING
#define BOOST_MULTI_INDEX_ENABLE_SAFE_MODE
#endif

#include <boost/noncopyable.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/flyweight.hpp>
#include <boost/flyweight/no_tracking.hpp>

#include <string>
#include <vector>

namespace shovelio {

  /** I3FrameSequence presents the frames from a group of
   *  files as a sequence with random access. The file access
   *  may be cached (strongly recommended) and frames are
   *  indexed on-the-fly in a separate thread when files are
   *  added. The index stores the positions of frames
   *  in the stream, their type, and parent. The cache is
   *  used to recall the most recently visited frames and
   *  to prefetch the next frame(s) in the sequence.
   */
  class I3FrameSequence: private boost::noncopyable {
  SET_LOGGER("shovelio");
  public:
    struct IndexItem {
      uint64_t pos_; // in stream
      I3Frame::Stream stream_;
      boost::flyweights::flyweight<
        std::string
        , boost::flyweights::no_tracking
      > sub_stream_;
      unsigned parent_;
      IndexItem(uint64_t pos, I3FramePtr frame, unsigned parent);
    };

  private:
    raw::FileGroup files_;
    std::vector<std::string> filter_;
    I3FrameMixer mixer_;

    // Index structures
    typedef std::vector<IndexItem> Index;
    Index index_;
    bool index_complete_;
    bool index_thread_active_;

    // Cache structures
    struct CacheItem {
      bool keep_;
      unsigned idx_;
      I3FramePtr frame_;
      CacheItem(unsigned idx, I3FramePtr frame, bool keep):
        keep_(keep), idx_(idx), frame_(frame) {}
    };

    struct CacheItemInvalid {
      const unsigned max_idx_;
      CacheItemInvalid(unsigned max_idx) : max_idx_(max_idx) {}
      bool operator()(const CacheItem& item) {
        return item.idx_ >= max_idx_;
      }
    };

    typedef boost::multi_index::multi_index_container<
      CacheItem,
      boost::multi_index::indexed_by<
        boost::multi_index::random_access<>,
        boost::multi_index::hashed_unique<
          BOOST_MULTI_INDEX_MEMBER(CacheItem, unsigned, idx_)
        >
      >
    > Cache;

    Cache cache_;
    unsigned nbackward_; // number of visited frames to remember
    unsigned nforward_; // number of prefetched frames

    // thread structures
    typedef boost::lock_guard<boost::recursive_mutex> Lock;
    boost::recursive_mutex mtx_;
    boost::thread index_thread_; // runs PopulateIndex()

    void MergeParents(const unsigned, I3FramePtr);
    void PopulateIndex(uint64_t);
    unsigned IndexAppend(uint64_t, I3FramePtr);
    I3FramePtr Fetch(unsigned);

    // cache interface
    void Remember(unsigned, I3FramePtr);
    I3FramePtr Recall(unsigned);

  public:
    // Index for user access, lock released in dtor
    class LockedIndex : private boost::noncopyable {
      friend class I3FrameSequence;

      I3FrameSequence& parent_;
      Lock lock_;

    public:
      LockedIndex(I3FrameSequence& parent) :
        parent_( parent ),
        lock_( parent.mtx_ )
      {}

      const IndexItem& operator[](unsigned idx) const
      { return parent_.index_[idx]; }

      unsigned GetSize() const
      { return parent_.index_.size(); }
    };

    I3FrameSequence();
    ~I3FrameSequence();

    raw::File::Type AddFile(const std::string& filename);
    void CloseAllFiles();
    void CloseLastFile();

    unsigned GetSize() const { return index_.size(); }
    I3FramePtr operator[](unsigned);

    void SetNForward(unsigned nforward)
    { nforward_ = nforward; }
    void SetNBackward(unsigned nbackward)
    { nbackward_ = std::max(unsigned(3), nbackward); }
    unsigned GetNForward() const { return nforward_; }
    unsigned GetNBackward() const { return nbackward_; }

    unsigned GetNFiles() const
    { return files_.GetNFiles(); }

    const std::string& GetPath(unsigned i) const
    { return files_.GetPath(i); }

    raw::File::Type GetType(unsigned i) const
    { return files_.GetType(i); }

    bool IsIndexing () const
    { return !index_complete_; }

    const std::vector<std::string>& GetFilter() const
    { return filter_; }

    void SetFilter(const raw::FilterList& filter)
    { filter_ = filter; }
  };

} // NS shovelio

#endif
