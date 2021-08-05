/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3Trigger.h
 * @version $Revision$
 * @date $Date$
 * @author blaufuss
 * @author deyoung
 * @author ehrlich
 */

#ifndef I3_TRIGGER_H_INCLUDED
#define I3_TRIGGER_H_INCLUDED

#include <icetray/I3Logging.h>
#include <dataclasses/TriggerKey.h>
#include <dataclasses/Utility.h>
#include <dataclasses/I3Vector.h>

/**
 * @brief The basic trigger class.  
 * 
 * This is the base class for trigger records that will live in the I3TriggerHierarchy.
 */
static const unsigned i3trigger_version_ = 1;

class I3Trigger 
{
private:
  double     time_;             // Time at which the trigger was issued
  double     length_;           // Duration of triggered readout window
  bool       fired_;            // True, if trigger fired (used for simulations)
  TriggerKey key_;              // Trigger key that identifies source, type, subtype and config

 public:
  /**
   * Default constructor.
   */
  I3Trigger()
    : time_(0.0), length_(0.0), fired_(false) {}

  /**
   * Destructor.
   */
  ~I3Trigger();

  std::ostream& Print(std::ostream&) const;
  
  /**
   * Retrieves time at which the trigger was issued.
   * 
   * @return Trigger time.
   */
  double GetTriggerTime() const {return time_;}
  /**
   * Sets time at which the trigger was issued.
   * 
   * @param time Trigger time.
   */
  void SetTriggerTime(double time) {time_ = time;}

  /**
   * Retrieves duration of triggered readout window.
   * 
   * @return Trigger duration.
   */
  double GetTriggerLength() const {return length_;}
  /**
   * Sets duration of triggered readout window.
   * 
   * @param length Trigger duration.
   */
  void SetTriggerLength(double length) {length_ = length;}

  /**
   * Indicates, if the trigger was fired (used for simulations).
   * 
   * @return true, if the trigger was fired.
   */
  bool GetTriggerFired() const {return fired_;}
  
  /**
   * Sets, if the trigger was fired (used for simulations).
   * 
   * @param fired Indicates, if the trigger was fired.
   */
  void SetTriggerFired(bool fired) {fired_ = fired;}

  /**
   * Gets trigger key.
   * 
   * @return Key.
   */
  const TriggerKey& GetTriggerKey() const {return key_;}
  TriggerKey& GetTriggerKey() {return key_;}

  void SetTriggerKey(const TriggerKey &key) { key_ = key; }

  bool operator==(const I3Trigger& rhs) const;
  bool operator!=(const I3Trigger& rhs) const;

 private:
  static const double FROM_TENTH_NS_TO_NS_CORRECTION_FACTOR;
  
  
  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive & ar, unsigned version);


  // logging
  SET_LOGGER("I3Trigger");
};

std::ostream& operator<<(std::ostream& oss, const I3Trigger& t);

I3_CLASS_VERSION(I3Trigger, i3trigger_version_);
/**
 * pointer type to insulate users from memory management
 */
I3_POINTER_TYPEDEFS(I3Trigger);


typedef I3Vector<I3Trigger> I3VectorI3Trigger;
I3_POINTER_TYPEDEFS(I3VectorI3Trigger);

#endif


