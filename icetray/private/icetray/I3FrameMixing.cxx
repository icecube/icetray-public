#include "icetray/I3FrameMixing.h"

#include <boost/make_shared.hpp>

namespace{
  struct sameStop{
    sameStop(I3Frame::Stream stop):stop(stop){}
    bool operator()(const boost::shared_ptr<I3Frame>& f){
      return(f->GetStop()==stop);
    }
    I3Frame::Stream stop;
  };
}

std::vector<boost::shared_ptr<I3Frame> >::iterator
I3FrameMixer::UpdateDependenciesImpl(const I3Frame& frame)
{
  I3Frame::Stream stop=frame.GetStop();
  std::vector<boost::shared_ptr<I3Frame> >::iterator sit =
  std::find_if(parent_cache_.begin(),parent_cache_.end(),sameStop(stop));
  if (stop != I3Frame::TrayInfo &&
      stop != I3Frame::Physics){
    //if this frame type is not in the cache, put it there
    if(sit==parent_cache_.end()){
      parent_cache_.push_back(boost::make_shared<I3Frame>(frame));
      //update the pointer to this cache entry
      sit=parent_cache_.end()-1;
    }
    //if this type is in the cache and we are tracking the order, reshuffle to
    //move it to the end
    else if(track_order_){
      boost::shared_ptr<I3Frame> old_entry=*sit;
      std::copy(sit+1,parent_cache_.end(),sit);
      parent_cache_.back()=old_entry;
      *parent_cache_.back()=frame;
      //update the pointer to this cache entry
      sit=parent_cache_.end()-1;
    }
    else //otherwise update the cache entry in place
      **sit=frame;
  }
  return(sit);
}

void I3FrameMixer::UpdateDependencies(const I3Frame& frame){
  if(!disable_mixing_)
    log_fatal("UpdateDependencies() called on an I3FrameMixer without disable_mixing set.\n"
              "disable_mixing is intended to indicate that the Mixer instance "
              "will only be used for dependency tracking via UpdateDependencies() "
              "and never for actual mixing.");
  UpdateDependenciesImpl(frame);
}

void I3FrameMixer::Mix(I3Frame& frame)
{
  if(disable_mixing_)
    log_fatal("Mix() called on an I3FrameMixer with disable_mixing set.\n"
              "disable_mixing is intended to indicate that the Mixer instance "
              "will only be used for dependency tracking via UpdateDependencies() "
              "and never for actual mixing.");
  frame.purge();
  //while the frame contains only its native keys, see if we should put it in
  //the cache
  auto sit = UpdateDependenciesImpl(frame);

  //copy keys from all other frame types in the cache into this frame
  for(std::vector<boost::shared_ptr<I3Frame> >::iterator it=parent_cache_.begin(),
      end=parent_cache_.end(); it!=end; it++){
    if(it!=sit) //skip the cache entry of the same type
      frame.merge(**it);
  }
}

std::vector<boost::shared_ptr<I3Frame> >
I3FrameMixer::GetMixedFrames(I3Frame::Stream stop)
{
  if(track_order_){ //easy case, parents are ordered
    std::vector<boost::shared_ptr<I3Frame> >::iterator sit =
    std::find_if(parent_cache_.begin(),parent_cache_.end(),sameStop(stop));
    return(std::vector<boost::shared_ptr<I3Frame> >(parent_cache_.begin(),sit));
  }
  else{ //copy all parents, and remove any match to the requested frame type
    std::vector<boost::shared_ptr<I3Frame> > results=parent_cache_;
    std::vector<boost::shared_ptr<I3Frame> >::iterator sit =
    std::find_if(results.begin(),results.end(),sameStop(stop));
    if(sit!=results.end())
      results.erase(sit);
    return(results);
  }
}

boost::optional<I3Frame::Stream> 
I3FrameMixer::MostRecentMixedStream(I3Frame::Stream stop) const{
  if(!track_order_)
    throw std::logic_error("MostRecentMixedStream called on an I3FrameMixer "
	                       "which has not been set to track stream order");
  std::vector<boost::shared_ptr<I3Frame> >::const_iterator sit =
  std::find_if(parent_cache_.begin(),parent_cache_.end(),sameStop(stop));
  if(sit==parent_cache_.begin()) //no previous frames seen, no answer exists
    return(boost::optional<I3Frame::Stream>());
  if(sit==parent_cache_.end()) //stop not in cache, return last thing which is
    return(parent_cache_.empty() ? //which might be nothing. . . 
	       boost::optional<I3Frame::Stream>() : 
		   parent_cache_.back()->GetStop());
  //otherwise return the previous stop type
  sit--;
  return((*sit)->GetStop());
}

