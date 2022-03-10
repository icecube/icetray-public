/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file TriggerKey.h
 * @version $Revision$
 * @date $Date$
 * @author blaufuss
 */
#ifndef TRIGGER_KEY_H_INCLUDED
#define TRIGGER_KEY_H_INCLUDED


#include <string>

#include <icetray/I3Logging.h>
#include <dataclasses/Utility.h>
#ifndef __CINT__
#include <boost/optional.hpp>
#endif

/**
 * List the names of enumeration members defined in this file
 * here. These can be used for e.g. pybindings, which require
 * the names of the enumeration members to be known. This list
 * should be updated whenever members or new enums are added to
 * the class.
 */
#define TRIGGERKEY_H_TriggerKey_SourceID                                 \
    (IN_ICE)(ICE_TOP)(AMANDA_TWR_DAQ)(EXTERNAL)(GLOBAL)(AMANDA_MUON_DAQ) \
    (SPASE)(UNKNOWN_SOURCE)

#define TRIGGERKEY_H_TriggerKey_TypeID                                   \
    (SIMPLE_MULTIPLICITY)(CALIBRATION)(MIN_BIAS)(THROUGHPUT)             \
    (TWO_COINCIDENCE)(THREE_COINCIDENCE)(MERGED)(SLOW_PARTICLE)          \
    (FRAGMENT_MULTIPLICITY)                                              \
    (STRING)(VOLUME)(SPHERE)(UNBIASED)(SPASE_2)(UNKNOWN_TYPE)

#define TRIGGERKEY_H_TriggerKey_SubtypeID                                \
    (NO_SUBTYPE)(M18)(M24)(T0)(LASER)(UNKNOWN_SUBTYPE)

/**
 * @brief A small class which is the "key" for the trigger status/config map 
 * in I3DetectorStatus.
 * 
 * Triggers are identified by three IDs in the IceCube DAQ system/global trigger.
 * These three IDs make an "unique" identifier that can be used to look up
 * additional trigger information like "prescale" for "MIN_BIAS" triggers
 * or "threshold" and "timeWindow" for "SIMPLE_MULTIPLICITY" triggers.
 * 
 * Those three pieces are:
 *  - Source ID
 *  - Type ID
 *  - Config ID
 * 
 * Software triggers from the TWR DAQ system are somewhat special, if processed
 * by the global trigger. A configuration that corresponds to a given source,
 * type and config ID triplet describes how the trigger is processed by
 * global trigger, but it carries no information about the TWR DAQ trigger system.
 * There is no more difference between a simple multiplicity of 18 or 24. Thus is
 * this triplet complemented by a subtype - it will only be used in connection with
 * software triggers. The subtype will also avoid ambiguities, when decoding data
 * from IceCube DAQ/global trigger.
 */
static const unsigned triggerkey_version_ = 1;

class TriggerKey
{
 public:
  /**
   * Enumeration describing what "subdetector" issued a trigger.
   */
  enum SourceID	     // probably incomplete enumeration of trigger sources
  {
    IN_ICE = 0,           // InIce trigger system
    ICE_TOP = 10,         // IceTop trigger system
    AMANDA_TWR_DAQ = 20,  // AMANDA TWR DAQ triggers
    EXTERNAL = 30,        // Some external trigger
    GLOBAL = 40,          // IceCube global trigger system
    AMANDA_MUON_DAQ = 50, // AMANDA Muon DAQ triggers
    SPASE = 70,           // A trigger from the SPASE shack
    UNKNOWN_SOURCE = 80   // A mysterious source
  };

  /**
   * Enumeration describing what "algorithm" issued a trigger.
   * More details about a specific trigger can be stored in the
   * I3TriggerStatus maps as part of the detector status
   */
  enum TypeID         // probably incomplete enumeration of trigger types
  {
    SIMPLE_MULTIPLICITY = 0,   // Any trigger the uses summed multiplicity
    CALIBRATION = 10,          // A calibration trigger
    MIN_BIAS = 20,             // Minumum bias (random) trigger
    THROUGHPUT = 30,           // An IceCube global trigger pass-thru trigger
    TWO_COINCIDENCE = 40,      // IceCube global trigger:  combined 2 trigs
    THREE_COINCIDENCE = 50,    // IceCube global trigger:  combined 3 trigs
    MERGED = 70,               // A trigger from a merging of events
    SLOW_PARTICLE = 80,        // A trigger for slow particles (long readouts)
    FRAGMENT_MULTIPLICITY = 105, // TWR trigger based on fragment counts
    STRING = 120,              // A trigger based on # hits in a string
    VOLUME = 125,              // A volume/geometry specific trigger
    SPHERE = 127,
    UNBIASED = 129,              // For things like the DAQ FixedRateTrigger
    SPASE_2 = 170,             // SPASE trigger
                               // ... I still don't understand for what this is needed for,
                               // ... or why this isn't a value of SourceID (SPASE_1/SPASE_2)
                               // ... (tschmidt)
    UNKNOWN_TYPE = 180               
  };
  
  /**
   * Enumeration describing how a software trigger was orginally "configured"
   * within the TWR DAQ trigger system.
   */
  enum SubtypeID
  {
    NO_SUBTYPE = 0,
    M18 = 50,
    M24 = 100,
    T0 = 150,
    LASER = 200,
    UNKNOWN_SUBTYPE = 250
  };


  /**
   * Returns the name of a source ID.
   *
   * @param source The source ID.
   * @return The name.
   */
  static const char* GetSourceString(SourceID source);
  static SourceID GetSourceFromString(const std::string& sourcestr);

  /**
   * Returns the name of a type ID.
   * 
   * @param type The type ID.
   * @return The name.
   */
  static const char* GetTypeString(TypeID type);
  static TypeID GetTypeFromString(const std::string& typestr);

 private:
  SourceID source_;
  TypeID type_;
  SubtypeID subtype_;
#ifndef __CINT__
  boost::optional<int> configID_;
#endif

 public:
  /**
   * Default constructor.
   */
  TriggerKey()
    : source_(UNKNOWN_SOURCE), type_(UNKNOWN_TYPE), subtype_(NO_SUBTYPE)
      {}

  /**
   * Constructor.
   * 
   * @param source Source ID.
   * @param type Type ID.
   * @param subtype Subtype ID (optional; default is NO_SUBTYPE).
   */
  TriggerKey(SourceID source, TypeID type, SubtypeID subtype = NO_SUBTYPE)
    : source_(source), type_(type), subtype_(subtype)
      {}

  /**
   * Constructor.
   * 
   * @param source Source ID.
   * @param type Type ID.
   * @param configID Configuration ID.
   * @param subtype Subtype ID (optional; default is NO_SUBTYPE).
   */
  TriggerKey(SourceID source, TypeID type, int configID, SubtypeID subtype = NO_SUBTYPE)
    : source_(source), type_(type), subtype_(subtype),
      configID_(configID) {}
    
  std::ostream& Print(std::ostream&) const;
  
  /**
   * Retrieves the source ID.
   * 
   * @return Source ID.
   */
  SourceID GetSource() const
  {
    return source_;
  }

  /**
   * Returns the name of the source ID.
   * 
   * @return The name.
   */
  const char* GetSourceString() const
  {
    return GetSourceString(source_);
  }

  /**
   * Sets the source ID.
   * 
   * @param source Source ID.
   */
  void SetSource(SourceID source)
  {
    source_ = source;
  }

  /**
   * Retrieves the type ID.
   * 
   * @return Type ID.
   */
  TypeID GetType() const
  {
    return type_;
  }

  /**
   * Returns the name of the type ID.
   * 
   * @return The name.
   */
  const char* GetTypeString() const
  {
    return GetTypeString(type_);
  }

  /**
   * Sets the type ID.
   * 
   * @param type Type ID.
   */
  void SetType(TypeID type)
  {
    type_ = type;
  }

  /**
   * Retrieves the subtype ID.
   * 
   * @return Subtype ID.
   */
  SubtypeID GetSubtype() const
  {
    return subtype_;
  }

  /**
   * Sets the subtype ID.
   * 
   * @param subtype Subtype ID.
   */
  void SetSubtype(SubtypeID subtype)
  {
    subtype_ = subtype;
  }

  /**
   * Indicates, if a configuration ID is set.
   * 
   * The triplet of (SourceID source, TypeID type, int configID)
   * corresponds to a trigger status/config used in the IceCube DAQ.
   * AMANDA has no such trigger configurations, so that the
   * configuration ID might not be set.
   * @return True, if configuration ID is set.
   */
  bool CheckConfigID() const
  {
    return configID_ ? true : false;
  }

  /**
   * Retrieves the configuration ID.
   * 
   * @return Configuration ID.
   */
  int GetConfigID() const
  {
    if( ! configID_ )
      log_fatal("No configuration ID set.");
    
    return configID_.get();
  }

  boost::optional<int> GetConfigIDOptional() const
  {
    return configID_ ;
  }
 
  /**
   * Resets the configuration ID.
   */
  void SetConfigID()
  {
    configID_ = boost::optional<int>();
  }

  /**
   * Sets the configuration ID.
   * 
   * @param configID Configuration ID.
   */
  void SetConfigID(int configID)
  {
    configID_ = boost::optional<int>(configID);
  }

  /**
   * Equality operator.
   * 
   * @param rhs The trigger key to compare this one to.
   * @return true if source, type, subtype and configuration ID of the two
   * trigger keys match.
   */
  bool operator==(const TriggerKey& rhs) const
  {
    return((rhs.source_ == source_)
           && (rhs.type_ == type_)
           && (rhs.subtype_ == subtype_)
           && ((!rhs.configID_ && !configID_)
               || (rhs.configID_ && configID_
                   && (rhs.configID_ == configID_))));
  }

  /**
   * Inequality operator.
   * 
   * @param rhs The trigger key to compare this one to.
   * @return false if source, type, subtype and configuration ID of the two
   * trigger keys are different.
   */
  bool operator!=(const TriggerKey& rhs) const
  {
    return !(rhs == *this);
  }

 private:
  friend class icecube::serialization::access;
  template <class Archive> void save(Archive & ar, unsigned version) const;
  template <class Archive> void load(Archive & ar, unsigned version);
  I3_SERIALIZATION_SPLIT_MEMBER()

  // logging
  SET_LOGGER("TriggerKey");
};

/**
 * pointer type to insulate users from memory management
 */
I3_POINTER_TYPEDEFS(TriggerKey);
I3_CLASS_VERSION(TriggerKey, triggerkey_version_);

/**
 * Comparison operator. Required to use TriggerKeys as a key of a map.
 * @param lhs the left-hand trigger key.
 * @param rhs the right-hand trigger key.
 * @return true if the lhs should be ordered before the rhs.
 *
 * Precedence between source, type, subtype and configuration ID is somewhat arbitrary,
 * so I've picked:
 * source, type, subtype, configuration ID in order of descending importance.
 */
inline bool
operator<(const TriggerKey& lhs, const TriggerKey& rhs)
{
  if(lhs.GetSource() < rhs.GetSource())
    return true;
  if(lhs.GetSource() > rhs.GetSource())
    return false;
  if(lhs.GetType() < rhs.GetType())
    return true;
  if(lhs.GetType() > rhs.GetType())
    return false;
  if(lhs.GetSubtype() < rhs.GetSubtype())
    return true;
  if(lhs.GetSubtype() > rhs.GetSubtype())
    return false;

  // OK, so the source ID and type ID etc. are equal ... save us configID!
  return((!lhs.CheckConfigID() && rhs.CheckConfigID())
         || (lhs.CheckConfigID() && rhs.CheckConfigID()
             && lhs.GetConfigID() < rhs.GetConfigID()));
}


/**
 * Comparison operator.
 * @param lhs the left-hand trigger key.
 * @param rhs the right-hand trigger key.
 * @return true if the rhs should be ordered before the lhs
 * or if lhs and rhs are equal.
 */
inline bool
operator>=(const TriggerKey& lhs, const TriggerKey& rhs)
{
  return !(lhs < rhs);
}


/**
 * Comparison operator.
 * @param lhs the left-hand trigger key.
 * @param rhs the right-hand trigger key.
 * @return true if the rhs should be ordered before the lhs.
 */
inline bool
operator>(const TriggerKey& lhs, const TriggerKey& rhs)
{
  return((lhs >= rhs) && (lhs != rhs));
}


/**
 * Comparison operator.
 * @param lhs the left-hand trigger key.
 * @param rhs the right-hand trigger key.
 * @return true if the lhs should be ordered before the rhs
 * or if lhs and rhs are equal.
 */
inline bool
operator<=(const TriggerKey& lhs, const TriggerKey& rhs)
{
  return !(lhs > rhs);
}

std::ostream& operator<<(std::ostream& oss, const TriggerKey& k);

#endif
