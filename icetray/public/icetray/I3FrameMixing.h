#ifndef ICETRAY_I3FRAMEMIXING_H
#define ICETRAY_I3FRAMEMIXING_H

#include "icetray/I3Frame.h"

/**
 * When I3Frames are processed keys are mixed to subsequent frames on different
 * streams. This class encapsulates the necessary logic in one place, and 
 * provides and interface to retrieve the frames which currently have been or 
 * would be mixed into a frame of a given type.
 */
class I3FrameMixer{
public:
  explicit I3FrameMixer(bool track_order=false):
  track_order_(track_order){}

  /**
   * Mix keys from previous frames into frame, and if necessary update the
   * internal cache so that keys from frame can be mixed into subsequent
   * frames.
   */
  void Mix(I3Frame& frame);

  /**
   * Retrieve the frames which were most recently mixed into a frame of the
   * given stream, or if the given frame type does not appear in the cache
   * (because it is immiscable or has not been encountered), the frames which
   * would be mixed into such a frame were it encountered at this time.
   *
   * If stream order tracking is turned on, the mixed frames will be returned in
   * the order that they appearsed in the sequence, otherwise they will be
   * returned in an arbitrary order.
   */
  std::vector<boost::shared_ptr<I3Frame> > GetMixedFrames(I3Frame::Stream stop);

  bool TrackOrder() const{ return(track_order_); }
  void TrackOrder(bool t){ track_order_ = t; }
  void Reset(){ parent_cache_.clear(); }
private:
  bool track_order_;
  std::vector<boost::shared_ptr<I3Frame> > parent_cache_;
};

#endif //ICETRAY_I3FRAMEMIXING_H
