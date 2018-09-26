#include "shovelio/I3FrameSequence.h"

#include <icetray/serialization.h>
#include <icetray/I3Logging.h>
#include <icetray/I3FrameMixing.h>
#include <dataclasses/physics/I3EventHeader.h>

#include <boost/foreach.hpp>
#include <boost/date_time.hpp> // don't use boost/chrono.hpp, not in boost-1.38
#include <boost/iostreams/device/file_descriptor.hpp>

#include <cassert>
#include <iostream>
#include <unistd.h>

namespace shovelio {

// Takes a boolean and flips it upon destruction
class DeadMansSwitch {
  bool& b_;
public:
  DeadMansSwitch(bool& b) : b_(b) {}
  ~DeadMansSwitch(){ b_ = !b_; }
};

bool keepFrameInCache( I3FramePtr frame ){
    const I3Frame::Stream stop = frame->GetStop();
    return stop == I3Frame::Geometry ||
           stop == I3Frame::Calibration ||
           stop == I3Frame::DetectorStatus;
}

I3FrameSequence::IndexItem::IndexItem(uint64_t pos, I3FramePtr frame, unsigned parent):
  pos_(pos),
  stream_(frame->GetStop()),
  parent_(parent)
{
  if( stream_ == I3Frame::Physics ){
    I3EventHeaderConstPtr header = frame->Get<I3EventHeaderConstPtr>();
    if( header )
      sub_stream_ = header->GetSubEventStream();
  }
}

I3FrameSequence::I3FrameSequence():
  mixer_(true),
  index_complete_(true),
  index_thread_active_(false),
  nbackward_(5),
  nforward_(5)
{}

I3FrameSequence::~I3FrameSequence()
{
  CloseAllFiles();
}

raw::File::Type
I3FrameSequence::AddFile(const std::string& filename)
{
  Lock lock( mtx_ );
  uint64_t current_pos = files_.GetSize();
  const raw::File& f = files_.AddFile(filename);

  // indexing has not yet started
  if( index_complete_ ){
    index_complete_ = false;
    index_thread_active_ = true;
    index_thread_ = boost::thread(&I3FrameSequence::PopulateIndex, this, current_pos);
  }

  return f.GetType();
}

void
I3FrameSequence::CloseAllFiles()
{
  if( index_thread_active_ ){
    index_thread_.interrupt();
    index_thread_.join();
  }

  // now only main thread is running
  index_complete_ = true;
  index_.clear();
  cache_.clear();
  mixer_.Reset();
  files_.CloseAllFiles();
}

void
I3FrameSequence::CloseLastFile()
{
  Lock lock( mtx_ );

  if( files_.GetNFiles() == 0 )
    return;

  if( files_.GetNFiles() == 1 ){
    CloseAllFiles();
    return;
  }

  files_.CloseLastFile();
  if( files_.HasSize() ){
    // shrink index
    while( index_.back().pos_ >= files_.GetSize() )
      index_.pop_back();
    // erase invalid cache items
    cache_.remove_if(CacheItemInvalid(index_.size()));
    // reset the mixer state
    mixer_.Reset();
    for( unsigned idx = 0; idx < index_.size(); ++idx ){
      I3FramePtr frame = Fetch( idx );
      mixer_.Mix( *frame );
    }
  }
  // if files_.HasSize() == false, PopulateIndex
  // is still running and everything is fine
}

void
I3FrameSequence::MergeParents(const unsigned idx, I3FramePtr frame)
{
  // recursively merge parents
  IndexItem item = index_[idx];
  unsigned pidx = idx;
  while( item.parent_ != pidx ){
    pidx = item.parent_;
    item = index_[pidx];

    // try to get pre-merged frame from cache
    I3FramePtr pframe = Recall(pidx);
    const bool parent_is_merged = bool(pframe);

    if( pframe ){
      log_trace_stream( "merging: found stream=" << item.stream_.str() 
                        << " in cache, nkeys="
                        << pframe->size() );
    }else{
      // try to load frame from file
      pframe.reset( new I3Frame() );
      if( !files_.AtStreamPos(pframe, item.pos_, filter_) )
        log_fatal( "could not merge parent!" );
      log_trace_stream( "merging: got stream=" << item.stream_.str()
                        << " from file, nkeys="
                        << pframe->size() );
    }

    log_trace_stream( "merging: parent=" << pidx 
                      << " into idx=" << idx );
    frame->merge(*pframe);

    // frames from cache are pre-merged, we can stop here
    if( parent_is_merged ) break;
  }
}

void
I3FrameSequence::PopulateIndex(uint64_t read_pos)
{
  /* Start by populating the cache until it's full. 
     Periodically release lock, so that main thread can run. */

  DeadMansSwitch dms( index_thread_active_ );

  log_trace_stream( "index: called with pos=" << read_pos << ", "
                    "cur=" << files_.CurrentStreamPos() );

  I3FramePtr frame( new I3Frame() );
  bool fast = cache_.size() < nbackward_;

  while( !index_complete_ ){
    boost::this_thread::interruption_point();
    Lock lock( mtx_ );

    const raw::FilterList& filter = fast ?
      raw::kFilter.OnlyKeepEventHeader : filter_;

    // check whether another thread has moved the stream position
    const bool frame_loaded = read_pos == files_.CurrentStreamPos() ?
      files_.Next( frame, fast, filter ) :
      files_.AtStreamPos( frame, read_pos, filter );

    if( !frame_loaded ){
      // Next or AtStreamPos fail if file reaches end
      // or for SinglePass file with Next(..., true)
      if( files_.HasSize() ){
        // okay, reached end
        index_complete_ = true;
      } // else: is a SinglePass file, leave index incomplete

      log_trace_stream( "index thread ends, index size="
                        << index_.size() 
                        << (index_complete_ ? " (complete)" : " (incomplete)") );
      break;
    }

    unsigned idx = IndexAppend( read_pos, frame ); // IndexAppend does frame mixing
    read_pos = files_.CurrentStreamPos();

    // fill cache until it's full, then switch to fast reading
    if( !fast ){
      // always do a proper merge when putting something in the cache
      MergeParents( idx, frame );
      Remember( idx, frame );
      frame.reset( new I3Frame() ); // replace with a fresh frame
      fast = cache_.size() >= nbackward_;
    }

  }
}

unsigned
I3FrameSequence::IndexAppend(uint64_t pos, I3FramePtr frame)
{
  const unsigned idx = index_.size();
  mixer_.Mix( *frame );

  boost::optional<I3Frame::Stream> pstream =
    mixer_.MostRecentMixedStream( frame->GetStop() );

  unsigned pidx = idx;
  // find position of most recent mixin
  if( pstream ){
    --pidx;
    while( pidx > 0 && *pstream != index_[pidx].stream_ ){
      if( index_[pidx].parent_ != pidx )
        pidx = index_[pidx].parent_;
      else
        --pidx;
    }
  }

  index_.push_back( IndexItem(pos, frame, pidx) );

  log_trace_stream( "index: adding stream=" << frame->GetStop().str()
                    << ", idx=" << idx 
                    << ", parent=" << pidx
                    << ", pos=" << pos );

  return idx;
}

I3FramePtr
I3FrameSequence::Fetch(unsigned idx)
{
  log_trace_stream( "fetching " << idx );

  // position outside index requested: finish index thread
  while( idx >= index_.size() && index_thread_active_ ){
    log_trace_stream( "waiting for index, size=" << index_.size() );
    boost::this_thread::sleep( boost::posix_time::millisec(500) );
  }

  // lock internal structures
  Lock lock( mtx_ );

  if( idx < index_.size() ){
    // we are inside indexed space
    I3FramePtr frame = Recall(idx);
    if( frame )
      return frame;

    log_trace_stream( "cache miss for idx=" << idx
                      << ", fetching from file..." );

    frame.reset( new I3Frame() );
    if( files_.AtStreamPos(frame, index_[idx].pos_, filter_) ){
      MergeParents( idx, frame );
      Remember( idx, frame );
      return frame;        
    }else{
      // we failed to read from a single pass file, return empty frame
      return I3FramePtr();
    }
  }

  // really outside of index? return null frame
  if( index_complete_ && idx >= index_.size() )
    return I3FramePtr();

  // if we arrive here, this must be a single pass file,
  // read forward until we reach requested position
  I3FramePtr frame;
  for( unsigned i = index_.size(); i <= idx; ++i ){
    frame.reset( new I3Frame );
    const uint64_t pos = files_.CurrentStreamPos();
    if( files_.Next(frame, false, filter_) ){
      IndexAppend( pos, frame ); // IndexAppend does frame mixing
      Remember( i, frame );
    }else{
      // we arrived at the file end, return empty frame
      index_complete_ = true;
      return I3FramePtr();
    }
  }
  return frame;
}

void
I3FrameSequence::Remember(unsigned idx, I3FramePtr frame)
{
  log_trace( "cache (size=%lu): remembers idx=%u, nkeys=%lu",
             cache_.size(), idx, frame->size() );
  cache_.push_front( CacheItem(idx, frame, keepFrameInCache(frame)) );

  while( cache_.size() > (nbackward_ + nforward_) ){
    // delete least-recently-used item with keep==false
    log_trace_stream( "cache full, dropping idx="
                      << cache_.back().idx_ << ", stream=" 
                      << cache_.back().frame_->GetStop().str() );
    cache_.pop_back();
  }
}

I3FramePtr
I3FrameSequence::Recall(unsigned idx)
{
  Cache::nth_index<1>::type::iterator iter = cache_.get<1>().find(idx);
  if( iter == cache_.get<1>().end() ) // cache miss
    return I3FramePtr();
  log_trace( "cache: recalled frame at idx=%i, nkeys=%lu",
             idx, iter->frame_->size() );
  // move item to front of cache
  cache_.get<0>().relocate( cache_.get<0>().begin(),
                            cache_.project<0>(iter) );
  return iter->frame_;
}

I3FramePtr
I3FrameSequence::operator[](unsigned idx)
{
  I3FramePtr frame = Fetch(idx);

  if( !index_thread_active_ && !index_complete_ ){
    // single-pass file, pre-fetching next frame(s) into the cache
    ++idx;
    for( unsigned end = idx + nforward_; idx != end; ++idx )
      Fetch( idx );
  }

  return frame;
}

} // NS shovelio
