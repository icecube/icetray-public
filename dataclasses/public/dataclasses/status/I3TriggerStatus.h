/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3TriggerStatus.h
 * @version $Revision$
 * @date $Date$
 * @author blaufuss
 */
#ifndef I3_TRIGGER_STATUS_H_INCLUDED
#define I3_TRIGGER_STATUS_H_INCLUDED


#include <string>
#include <map>

#include <boost/optional.hpp>

#include <dataclasses/Utility.h>
#include <dataclasses/TriggerKey.h>

/**
 * @brief A trigger status/configuration object.
 *
 * A class describing all additional trigger information beside source, type
 * and subtype that are encapsulated in a TriggerKey already.
 * It only consists of 
 * - a name and
 * - a map of trigger settings:  string, integer values,
 * since most trigger information varies by trigger type.
 *
 * For IceCube triggers, these settings are most probably ...<BR>
 * <BR>... simple multiplicity trigger:<BR>
 * "threshold" - number of hits required in a time window<BR>
 * "timeWindow" - length of sliding time window (in ns)<BR>
 * <BR>... calibration trigger<BR>
 * "hitType" - type of hit to trigger on (0 (test), 1 (CPU), 2 (SPE), 3 (flasher))<BR>
 * <BR>... min bias trigger<BR>
 * "prescale" - one over the fraction of hits to trigger on<BR>
 * <BR>... two coincidence trigger<BR>
 * "triggerType1" - trigger type of first trigger<BR>
 * "triggerConfigId1" - trigger config ID of first trigger<BR>
 * "sourceId1" - source ID of first trigger<BR>
 * "triggerType2" - trigger type of second trigger<BR>
 * "triggerConfigId2" - trigger config ID of second trigger<BR>
 * "sourceId2" - source ID of second trigger<BR>
 * <BR>... three coincidence trigger<BR>
 * "triggerType1" - trigger type of first trigger<BR>
 * "triggerConfigId1" - trigger config ID of first trigger<BR>
 * "sourceId1" - source ID of first trigger<BR>
 * "triggerType2" - trigger type of second trigger<BR>
 * "triggerConfigId2" - trigger config ID of second trigger<BR>
 * "sourceId2" - source ID of second trigger<BR>
 * "triggerType3" - trigger type of third trigger<BR>
 * "triggerConfigId3" - trigger config ID of third trigger<BR>
 * "sourceId3" - source ID of third trigger<BR>
 * 
 * Additionally, the per-trigger readout instructions, specified for each subdetector:
 *  readoutTimeMinus  : time before the trigger time to set the readout window
 *  readoutTimePlus  : time after the trigger time to set the readout window
 *  readoutTimeOffset : time shift relatve to the trigger time to set the readout windoow
 */
static const unsigned i3triggerstatus_version_ = 3;
static const unsigned i3triggerreadoutconfig_version_ = 0;

/**
 * Uses boost::lexical_cast to ensure the conversion can be made.  
 * http://www.boost.org/doc/libs/1_38_0/libs/conversion/lexical_cast.htm
 * This catches things that will silently trip up atoi and atof.
 */
template <typename F,typename T>
void Convert(const F& from, boost::optional<T>& to);

template <typename T>
void Convert(const char* from, boost::optional<T>& to);

/**
 *  A simple struct to hold the per-subdetector readout configurations.
 */
struct I3TriggerReadoutConfig
{
  double readoutTimeMinus;
  double readoutTimePlus;
  double readoutTimeOffset;
  template <class Archive>
  void serialize(Archive& ar, unsigned version);

  I3TriggerReadoutConfig()
  {
    readoutTimeMinus = NAN;
    readoutTimePlus = NAN;
    readoutTimeOffset = NAN;
  }
  
  bool operator==(const I3TriggerReadoutConfig& rhs) const
  {
    return (readoutTimeMinus == rhs.readoutTimeMinus &&
        readoutTimePlus == rhs.readoutTimePlus &&
        readoutTimeOffset == rhs.readoutTimeOffset);
  }
  bool operator!=(const I3TriggerReadoutConfig& rhs) const
  {
    return !operator==(rhs);
  }
};

I3_CLASS_VERSION(I3TriggerReadoutConfig, i3triggerreadoutconfig_version_);

class I3TriggerStatus 
{
 public:

  /**
   * Subdetector enum, values match the daq labeling for subdirs.
   *  0 = ALL doms (InIce, IceTop) together
   *  1 = IceTop portion
   *  2 = InIce DOMS
   */
  enum Subdetector{
    NOT_SPECIFIED=-1,
    ALL=0, 
    ICETOP=1,
    INICE=2
  };

  /**
   * Default constructor.
   */
  I3TriggerStatus()
    {};
  
  /**
   * Constructor.
   * 
   * @param name Name of the trigger.
   * @param settings Trigger settings: string, integer values.
   */
  I3TriggerStatus(const std::string& name,
                  const std::map<std::string, std::string>& settings)
    : name_(name), settings_(settings) {}
  
  /**
   * Destructor.
   */
  ~I3TriggerStatus(){};
  
  /**
   * Get trigger name.
   * 
   * @return Name of trigger.
   */
  const std::string& GetTriggerName() const { return name_; }
  std::string& GetTriggerName() { return name_; }

  /**
   * Get trigger settings.
   * 
   * @return Trigger settings: string, string values
   * (contents vary by trigger type).
   *
   */
  const std::map<std::string, std::string>& 
    GetTriggerSettings() const { return settings_; }

  std::map<std::string, std::string>& 
    GetTriggerSettings() { return settings_; }

  /**
   * 'Get's the trigger value for the given key.  The underlying
   * value is stored as a string ( just like in the DB ) and is
   * converted on-the-fly to whichever type you want.  Currently
   * only bool, int, float, double, and string are supported.
   */
  template<typename T>
    void GetTriggerConfigValue(const std::string& key, boost::optional<T>& value) const;
			       			       
  template<typename T>
    void GetTriggerConfigValue(const char* key, boost::optional<T>& value) const;

  template<typename T>
    void GetTriggerConfigValue(const std::string& key, T& value) const;			       
  template<typename T>
    void GetTriggerConfigValue(const char* key, T& value) const;
			       
  /**
   * Sets the trigger config value.  Currently the only supported
   * types are bool, int, float, double, string, and const char*, but
   * since the underlying code uses boost::lexical cast ( which
   * uses stringstream, it's trivial to add support for any
   * streamable type.
   */
  template <typename T>
    void SetTriggerConfigValue(const std::string& key, T value);
  
  template <typename T>
    void SetTriggerConfigValue(const char* key, T value);
  
  const std::map<Subdetector, I3TriggerReadoutConfig>& 
    GetReadoutSettings() const { return readoutconfigs_; }
  std::map<Subdetector, I3TriggerReadoutConfig>& 
    GetReadoutSettings() { return readoutconfigs_; }  
  
  bool operator==(const I3TriggerStatus& rhs) const
  {
    return (name_ == rhs.name_ &&
        settings_ == rhs.settings_ &&
        readoutconfigs_ == rhs.readoutconfigs_);
  }
  bool operator!=(const I3TriggerStatus& rhs) const
  {
    return !operator==(rhs);
  }

  std::string name_;
  std::map<std::string, std::string> settings_;
  std::map<Subdetector, I3TriggerReadoutConfig> readoutconfigs_;

 private:
  friend class icecube::serialization::access;
  template <class Archive> void load(Archive & ar, unsigned version);
  template <class Archive> void save(Archive & ar, unsigned version) const;

  I3_SERIALIZATION_SPLIT_MEMBER();

  // logging
  SET_LOGGER("I3TriggerStatus");
};

/**
 * pointer type to insulate users from memory management
 */
typedef std::map<TriggerKey, I3TriggerStatus> I3TriggerStatusMap;
I3_POINTER_TYPEDEFS(I3TriggerStatusMap);

I3_POINTER_TYPEDEFS(I3TriggerStatus);
I3_CLASS_VERSION(I3TriggerStatus, i3triggerstatus_version_);

#endif

