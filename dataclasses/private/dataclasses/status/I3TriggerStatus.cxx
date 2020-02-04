/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3TriggerStatus.cxx
 * @version $Revision$
 * @date $Date$
 */
#include <icetray/serialization.h>
#include <dataclasses/status/I3TriggerStatus.h>
#include <sstream>
#include <iostream>

#include <boost/foreach.hpp>
#include <boost/preprocessor.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>

/**
 * Uses boost::lexical_cast to ensure the conversion can be made.  
 * http://www.boost.org/doc/libs/1_38_0/libs/conversion/lexical_cast.htm
 * This catches things that will silently trip up atoi and atof.
 */
template <typename F,typename T>
void Convert(const F& from, boost::optional<T>& to){
  try{
    T value = boost::lexical_cast<T>(from);
    to = boost::optional<T>(value);
  }catch(boost::bad_lexical_cast &){
    to = boost::optional<T>();
  } 
};

// specialization for const char*
template <typename T>
void Convert(const char* from, boost::optional<T>& to){
  Convert(std::string(from),to);
};

// specialization for bools
// checks for strings 'true','false','T', and 'F'
// case-insensitive
template <>
void Convert(const std::string& from, boost::optional<bool>& to){
  try{
    bool value = boost::lexical_cast<bool>(from);
    to = boost::optional<bool>(value);
  }catch(boost::bad_lexical_cast &){    
    to = boost::make_optional(false, bool());
    //to = boost::optional<bool>();
    // GCCFalsePositive
    // Fix an false positive detection of -Wmaybe-uninitialized
    //see https://www.boost.org/doc/libs/1_71_0/libs/optional/doc/html/boost_optional/tutorial/gotchas/false_positive_with__wmaybe_uninitialized.html

    // the default is unset
    // if the string is "true","false","t", or "f"
    // then set the bool accordingly.
    std::string tmp(from);
    boost::algorithm::to_lower(tmp);
    if( tmp == "true" || tmp == "t")
      to = boost::optional<bool>(true);
    if( tmp == "false" || tmp == "f")
      to = boost::optional<bool>(false);
  }
};

// specialization for const char*
template <>
void Convert(const char* from, boost::optional<bool>& to){
  Convert(std::string(from),to);
};

template <typename T>
void I3TriggerStatus::SetTriggerConfigValue(const std::string& key, T value){
  boost::optional<std::string> str_value;
  Convert(value,str_value);
  if(str_value)
    settings_[key] = str_value.get();
  else
    log_error("Conversion failed.");
}

template <typename T>
void I3TriggerStatus::SetTriggerConfigValue(const char* key, T value){
  std::string key_str(key);
  SetTriggerConfigValue(key_str,value);
}

template <typename T>
void I3TriggerStatus::GetTriggerConfigValue(const std::string& key, 
					    boost::optional<T>& value) const{
  typedef std::map<std::string,std::string> map_t;
  map_t::const_iterator iter = settings_.find(key);
  if(iter == settings_.end()){
    log_debug("Couldn't find '%s' in settings",key.c_str());
    // stream the possible key values to give the user
    // an idea of how to fix what's broken.
    std::stringstream keys;
    BOOST_FOREACH(const map_t::value_type& vt, settings_){
      keys<<" '"<<vt.first<<"'";
    }
    log_debug("  Possible keys : %s",keys.str().c_str());
  }else{
    Convert(iter->second,value);
  }
}

template <typename T>
void I3TriggerStatus::GetTriggerConfigValue(const char* key, 
					    boost::optional<T>& value) const {
  std::string key_str(key);
  GetTriggerConfigValue(key_str,value);
}

template <typename T>
void I3TriggerStatus::GetTriggerConfigValue(const std::string& key, T& value) const {
  boost::optional<T> opt_value;
  GetTriggerConfigValue(key,opt_value);
  if(!opt_value) log_fatal("Couldn't find '%s' in the I3TriggerStatus.", key.c_str());
  else value = opt_value.get();
}

template <typename T>
void I3TriggerStatus::GetTriggerConfigValue(const char* key, T& value) const {
  std::string key_str(key);
  boost::optional<T> opt_value;
  GetTriggerConfigValue(key_str,opt_value);
  if(!opt_value) log_fatal("Couldn't find '%s' in the I3TriggerStatus.", key);
  else value = opt_value.get();
}

template <class Archive>
void I3TriggerReadoutConfig::serialize(Archive& ar, unsigned version)
{
  if (version>i3triggerreadoutconfig_version_)
    log_fatal("Attempting to read version %u from file but running version %u of "
	      "I3TriggerReadoutConfig class.",version,i3triggerreadoutconfig_version_);
  ar & make_nvp("ReadoutTimeMinus",readoutTimeMinus);
  ar & make_nvp("ReadoutTimePlus",readoutTimePlus);
  ar & make_nvp("ReadoutTimeOffset",readoutTimeOffset);
}

// always write the latest class
template <class Archive>
void I3TriggerStatus::save(Archive& ar, unsigned version) const
{
  ar & make_nvp("Name", name_);
  ar & make_nvp("Settings", settings_);
  ar & make_nvp("ReadoutWindowConfigs",readoutconfigs_);
}

template <class Archive>
void I3TriggerStatus::load(Archive& ar, unsigned version) 
{
  if (version>i3triggerstatus_version_)
    log_fatal("Attempting to read version %u from file but running version %u of "
	      "I3TriggerStatus class.",
	      version,i3triggerstatus_version_);

  ar & make_nvp("Name", name_);
  if(version<3)
    {
      std::map<std::string, int> settings;
      ar & make_nvp("Settings", settings);
      for(std::map<std::string,int>::iterator iter = settings.begin();
	  iter != settings.end(); ++iter){
	std::stringstream str;
	str<<iter->second;
	settings_[iter->first] = str.str();
      }
    }
  else
    {
      ar & make_nvp("Settings", settings_);
    }
  //in version 1, only a single readout window was saved -> map to global subdetector (0)
  // only provided for backward compatibility reading...
  if(version==1)
    {
      I3TriggerReadoutConfig readout;
      ar & make_nvp("ReadoutTimeMinus", readout.readoutTimeMinus);
      ar & make_nvp("ReadoutTimePlus", readout.readoutTimePlus);
      ar & make_nvp("ReadoutTimeOffset", readout.readoutTimeOffset);
      readoutconfigs_[I3TriggerStatus::ALL] = readout;
    }
  if(version>1)
    {
      ar & make_nvp("ReadoutWindowConfigs",readoutconfigs_);
    }
}

I3_SPLIT_SERIALIZABLE(I3TriggerStatus);
I3_SERIALIZABLE(I3TriggerReadoutConfig);

#define TRIGGER_CONFIG_TYPES(TYPE)				                      \
template void I3TriggerStatus::SetTriggerConfigValue(const std::string&,TYPE);        \
template void I3TriggerStatus::SetTriggerConfigValue(const char*,TYPE);               \
template void I3TriggerStatus::GetTriggerConfigValue(const std::string&,              \
						     boost::optional<TYPE>&) const;   \
template void I3TriggerStatus::GetTriggerConfigValue(const char*,                     \
						     boost::optional<TYPE>&) const;   \
template void I3TriggerStatus::GetTriggerConfigValue(const std::string&,TYPE&) const; \
template void I3TriggerStatus::GetTriggerConfigValue(const char*,TYPE&) const;        \
template void Convert(const std::string&, boost::optional<TYPE>&);                    \
template void Convert(const char*, boost::optional<TYPE>&);  

#pragma GCC diagnostic push 
#ifdef __clang__
#pragma GCC diagnostic ignored "-Winstantiation-after-specialization"
#endif
TRIGGER_CONFIG_TYPES(bool);
#pragma GCC diagnostic pop 
TRIGGER_CONFIG_TYPES(int);
TRIGGER_CONFIG_TYPES(float);
TRIGGER_CONFIG_TYPES(double);
TRIGGER_CONFIG_TYPES(unsigned);
TRIGGER_CONFIG_TYPES(long);
TRIGGER_CONFIG_TYPES(std::string);

template void I3TriggerStatus::SetTriggerConfigValue(const std::string&,const char*);
template void I3TriggerStatus::SetTriggerConfigValue(const char*,const char*);
