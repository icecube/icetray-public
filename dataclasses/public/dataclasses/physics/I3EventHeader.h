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
 /**
  * @brief Unique identifier assigned sequentially to every run the DAQ has ever recorded.
  */
  unsigned runID_;

  /**
   * @brief Unique identifier assigned sequentially by the DAQ to every configuration change
   * which occurs during a single run. The subRunID is almost always going to be 0.
   * 
   * The DAQ starts each run with subRunID=0 and only increments it when the run configuration changes.
   * Since, flasher runs are the only time when more than one configuration is used, they are the only
   * time you will see a non-zero subRunID.
   *  
   * SubRunID is often confused with PFFilt file number. Pnf breaks up both its PFRaw and PFFilt
   * output stream into multiple files for transmission over the satellite and storage.
   * These file partitions are based solely on the size of the file and have no significance to the 
   * data or the DAQ.
   * The file number is not recorded anywhere in the frame and cannot easily be added in the frame.
   * Also note that since PnF uses a different writer module to write PFRaw and PFFilt files that
   * there will be a different number of PFRaw and PFFilt files and no correspondence between which
   * file an event will land between the two output streams.
   * 
   * Experimental data filenames will often contain three values: the runID, the subRunID, and 
   * the PFFilt file number. But since the subRun is almost always 0 it often gets confused with 
   * the PFFilt file number.
   * For example the filename `PFFilt_PhysicsFiltering_Run00136690_Subrun00000000_00000205.tar.bz2` 
   * indicates a runID of 136690, a subRunID of 0, and a PFFilt file number of 205 but will often
   * be erroneously referred to as subrun 205.
   */
  unsigned subRunID_;

  /**
   * @brief Unique identifier assigned by the DAQ to every event (Q-frame) in the given run.
   */
  unsigned eventID_;

  /**
   * @brief Unique identifier assigned to every P-frame by the splitter.
   * 
   * This quantity is not assigned by the DAQ. When Q-frames are split into multiple
   * P-frames by a splitter module this is the unique number assigned to each P-frame.
   */
  unsigned subEventID_;

  /** 
   * @brief Name of the P-frame stream assigned by the splitter module
   */
  std::string subEventStream_;
  
  State state_;

  /**
   * @brief The 
   * 
   */
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

