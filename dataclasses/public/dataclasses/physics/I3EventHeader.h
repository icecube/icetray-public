/**
 * copyright  (C) 2004
 * the icecube collaboration
 * @version $Id$
 * @file I3EventHeader.h
 * @date $Date$
 */

#ifndef I3EVENTHEADER_H_INCLUDED
#define I3EVENTHEADER_H_INCLUDED

// includes

#include <string>

#include <dataclasses/I3Time.h>
#include <icetray/I3DefaultName.h>
#include <icetray/I3FrameObject.h>
#include <icetray/I3PointerTypedefs.h>
#include <icetray/serialization.h>

static const unsigned i3eventheader_version_ = 3;

/**
 * @brief The header for data on the Event stream.
 *
 * Supposed to be a header for the event that
 * you can store in a file if you don't want to store the event.
 * It's supposed to have enough data to reconstruct the full event
 * from the database
 */
class I3EventHeader : public I3FrameObject
{
 public:
  enum State
  {
    UNKNOWN_STATE = 0,
    OK = 20,
    CONFIG_IN_TRANSITION = 40
  };
  
 private:
  unsigned runID_;
  unsigned subRunID_;
  unsigned eventID_;
  unsigned subEventID_;

  std::string subEventStream_;
  
  State state_;

  I3Time startTime_;
  I3Time endTime_;

 public:
  I3EventHeader();

  ~I3EventHeader();
    
  std::ostream& Print(std::ostream&) const override;
  
  bool operator==(const I3EventHeader& other) const
    {
      return (runID_ == other.runID_ &&
              subRunID_ == other.subRunID_ &&
              eventID_ == other.eventID_ &&
              subEventID_ == other.subEventID_ &&
              subEventStream_ == other.subEventStream_ &&
              state_ == other.state_ &&
              startTime_ == other.startTime_ &&
              endTime_ == other.endTime_);
    }
  bool operator!=(const I3EventHeader& other) const
    {
      return !operator==(other);
    }

  I3Time GetStartTime() const
    {
      return startTime_;
    }

  void SetStartTime(I3Time time)
    {
      startTime_ = time;
    }

  I3Time GetEndTime() const
    {
      return endTime_;
    }

  void SetEndTime(I3Time time)
    {
      endTime_ = time;
    }

  /**
   * @return the run id for the event
   */
  unsigned GetRunID() const { return runID_; }

  /**
   * @param runid the new run id for the event
   */
  void SetRunID(unsigned runid) { runID_ = runid; }

  /**
   * @return the subrun id for the event
   */
  unsigned GetSubRunID() const { return subRunID_; }

  /**
   * @param subrunid the new subrun id for the event
   */
  void SetSubRunID(unsigned subrunid) { subRunID_ = subrunid; }

  /**
   * @return the event id for this event
   */
  unsigned GetEventID() const { return eventID_; }

  /**
   * @param eventid the new event id for the event
   */
  void SetEventID(unsigned eventid) { eventID_ = eventid; }

  /**
   * @return the subevent id for this subevent
   */
  unsigned GetSubEventID() const { return subEventID_; }

  /**
   * @param eventid the new subevent id for the subevent
   */
  void SetSubEventID(unsigned eventid) { subEventID_ = eventid; }

  /**
   * @return the subevent stream description
   */
  std::string GetSubEventStream() const { return subEventStream_; }

  /**
   * @param desc the new subevent stream description
   */
  void SetSubEventStream(const std::string &desc) { subEventStream_ = desc; }

  /**
   * @return the state for the event
   */
  unsigned GetState() const { return state_; }

  /**
   * @param state the new state for the event
   */
  void SetState(State state) { state_ = state; }
  
  /**
   * @return the name of the stream this header is for.... "Physics"
   */
  const std::string GetDataStream(){ return "Physics";}

 private:
  friend class icecube::serialization::access;

  template <class Archive> void serialize(Archive & ar, unsigned version);
};

std::ostream& operator<<(std::ostream& oss, const I3EventHeader& eh);

I3_CLASS_VERSION(I3EventHeader, i3eventheader_version_);
I3_POINTER_TYPEDEFS(I3EventHeader);
I3_DEFAULT_NAME(I3EventHeader);

#endif //I3EVENTHEADER_H_INCLUDED

