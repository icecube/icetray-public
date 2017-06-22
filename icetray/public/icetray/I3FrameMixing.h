#ifndef ICETRAY_I3FRAMEMIXING_H
#define ICETRAY_I3FRAMEMIXING_H

#include <boost/optional.hpp>
#include "icetray/I3Frame.h"

/**
 * When I3Frames are processed keys are mixed to subsequent frames on different
 * streams. This class encapsulates the necessary logic in one place, and 
 * provides and interface to retrieve the frames which currently have been or 
 * would be mixed into a frame of a given type.
 */
class I3FrameMixer{
public:
  /**
   * Construct a frame mixer. 
   *
   * \param track_order whether the order of mixed frames is recorded. Tracking
   *                    has some runtime overhead, but enables MostRecentMixedStream
   * \param disable_mixing whether the object will be used only to track frame 
   *                       dependencies, without performing actual mixing. This
   *                       setting cannot be changed after construction. 
   */
  explicit I3FrameMixer(bool track_order=false, bool disable_mixing=false):
  track_order_(track_order),disable_mixing_(disable_mixing){}

  /**
   * Mix keys from previous frames into frame, and if necessary update the
   * internal cache so that keys from frame can be mixed into subsequent
   * frames.
   *
   * \pre disable_mixing must have been set to false on construction
   */
  void Mix(I3Frame& frame);

  /**
   * Retrieve the frames which were most recently mixed into a frame of the
   * given stream, or if the given frame type does not appear in the cache
   * (because it is immiscable or has not been encountered), the frames which
   * would be mixed into such a frame were it encountered at this time.
   *
   * If stream order tracking is turned on, the mixed frames will be returned in
   * the order that they appeared in the sequence, otherwise they will be
   * returned in an arbitrary order.
   */
  std::vector<boost::shared_ptr<I3Frame> > GetMixedFrames(I3Frame::Stream stop);

  /**
   * Get the stop type of the most recent frame to be mixed which has been or
   * will eligible for mixing into the given stream. Note that the stream in
   * question should generally be the one for which Mix() has most recently been
   * called. 
   *
   * \pre Order tracking must be active
   * \param stop The stream into which mixing is to be considered. 
   * \return The most recently seen miscable stream, if any
   */
  boost::optional<I3Frame::Stream> 
  MostRecentMixedStream(I3Frame::Stream stop) const;
  
  /**
   * Update the frame cache as if mixing this frame, but without performing any
   * mixing. This allows GetMixedFrames to give correct results, but is 
   * incompatible with Mix. (Mix requires that only purged frames be added to
   * the cache, which this function does not enforce.)
   *
   * \pre disable_mixing must have been set to true on construction
   * \param frame the frame to possibly add to the frame cache
   */
  void UpdateDependencies(const I3Frame& frame);

  bool TrackOrder() const{ return(track_order_); }
  void TrackOrder(bool t){ track_order_ = t; }
  bool MixingDisabled() const{ return(disable_mixing_); }
  void Reset(){ parent_cache_.clear(); }
private:
  bool track_order_;
  const bool disable_mixing_;
  std::vector<boost::shared_ptr<I3Frame> > parent_cache_;
  
  /**
   * The internal implementation of UpdateDependencies which assumes that the 
   * frame has been purged and performs no error checking. 
   *
   * \param frame the frame to possibly add to the frame cache
   * \return the iterator to the new or updated entry in the parent cache
   */
  std::vector<boost::shared_ptr<I3Frame> >::iterator
  UpdateDependenciesImpl(const I3Frame& frame);
};

#endif //ICETRAY_I3FRAMEMIXING_H
