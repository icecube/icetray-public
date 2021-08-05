/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file TriggerKey.cxx
 * @version $Revision$
 * @date $Date$
 */
#include <icetray/serialization.h>
#include <dataclasses/TriggerKey.h>

const char*
TriggerKey::GetSourceString(SourceID source)
{
  switch (source)
  {
    case IN_ICE:
      return("IN_ICE");
    case ICE_TOP:
      return("ICE_TOP");
    case AMANDA_TWR_DAQ:
      return("AMANDA_TWR_DAQ");
    case EXTERNAL:
      return("EXTERNAL");
    case GLOBAL:
      return("GLOBAL");
    case AMANDA_MUON_DAQ:
      return("AMANDA_MUON_DAQ");
    case SPASE:
      return("SPASE");
    case UNKNOWN_SOURCE:
      return("UNKNOWN_SOURCE");
  }
  log_warn("undefined source ID %u",source);
  
  return NULL;
}

TriggerKey::SourceID
TriggerKey::GetSourceFromString(const std::string& sourcestr)
{
  if (sourcestr=="IN_ICE")
    return(IN_ICE);
  if (sourcestr=="ICE_TOP")
    return(ICE_TOP);
  if (sourcestr=="AMANDA_TWR_DAQ")
    return(AMANDA_TWR_DAQ);
  if (sourcestr=="EXTERNAL")
    return(EXTERNAL);
  if (sourcestr=="GLOBAL")
    return(GLOBAL);
  if (sourcestr=="AMANDA_MUON_DAQ")
    return(AMANDA_MUON_DAQ);
  if (sourcestr=="SPASE")
    return(SPASE);
  if (sourcestr=="UNKNOWN_SOURCE")
    return(UNKNOWN_SOURCE);

  log_warn("undefined source string %s",sourcestr.c_str());
  
  return UNKNOWN_SOURCE;
}

const char *
TriggerKey::GetTypeString(TypeID type)
{
  switch(type)
  {
    case SIMPLE_MULTIPLICITY:
      return("SIMPLE_MULTIPLICITY");
    case CALIBRATION:
      return("CALIBRATION");
    case MIN_BIAS:
      return("MIN_BIAS");
    case THROUGHPUT:
      return("THROUGHPUT");
    case TWO_COINCIDENCE:
      return("TWO_COINCIDENCE");
    case THREE_COINCIDENCE:
      return("THREE_COINCIDENCE");
    case MERGED:
      return("MERGED");
    case SLOW_PARTICLE:
      return("SLOW_PARTICLE");
    case FRAGMENT_MULTIPLICITY:
      return("FRAGMENT_MULTIPLICITY");
    case STRING:
      return("STRING");
    case VOLUME:
      return("VOLUME");
    case SPHERE:
      return("SPHERE");
    case UNBIASED:
      return("UNBIASED");
    case SPASE_2:
      return("SPASE_2");
    case UNKNOWN_TYPE:
      return("UNKNOWN_TYPE");
  }
  log_warn("undefined type ID %u",type);

  return NULL;
}

TriggerKey::TypeID
TriggerKey::GetTypeFromString(const std::string& typestr)
{
  if (typestr=="SIMPLE_MULTIPLICITY")
    return(SIMPLE_MULTIPLICITY);
  if (typestr=="CALIBRATION")
    return(CALIBRATION);
  if (typestr=="MIN_BIAS")
    return(MIN_BIAS);
  if (typestr=="THROUGHPUT")
    return(THROUGHPUT);
  if (typestr=="TWO_COINCIDENCE")
    return(TWO_COINCIDENCE);
  if (typestr=="THREE_COINCIDENCE")
    return(THREE_COINCIDENCE);
  if (typestr=="MERGED")
    return(MERGED);
  if (typestr=="SLOW_PARTICLE")
    return(SLOW_PARTICLE);
  if (typestr=="FRAGMENT_MULTIPLICITY")
    return(FRAGMENT_MULTIPLICITY);
  if (typestr=="STRING")
    return(STRING);
  if (typestr=="VOLUME")
    return(VOLUME);
  if (typestr=="SPHERE")
    return(SPHERE);
  if (typestr=="UNBIASED")
    return(UNBIASED);
  if (typestr=="SPASE_2")
    return(SPASE_2);
  if (typestr=="UNKNOWN_TYPE")
    return(UNKNOWN_TYPE);

  log_warn("undefined type string %s",typestr.c_str());

  return UNKNOWN_TYPE;
}

template <class Archive>
void TriggerKey::load(Archive& ar, unsigned version)
{
  if (version>triggerkey_version_)
    log_fatal("Attempting to read version %u from file but running version %u of TriggerKey class.",version,triggerkey_version_);

  ar & make_nvp("SourceID", source_);
  ar & make_nvp("TypeID", type_);
  // for version > 0 there is a subtype ID to be serialized
  // for version <= 0 there is no subtype ID available ... use default value instead
  if(version > 0) ar & make_nvp("SubtypeID", subtype_);
  else subtype_ = NO_SUBTYPE;

  bool configIDSet;
  ar & make_nvp("ConfigIDSet", configIDSet);
  if (configIDSet){
    int configID;
    ar & make_nvp("ConfigID", configID);
    configID_ = configID;
  }else{
    configID_ = boost::make_optional(false, int());
    //configID_ = boost::optional<int>();
    // GCCFalsePositive
    // Fix an false positive detection of -Wmaybe-uninitialized
    //see https://www.boost.org/doc/libs/1_71_0/libs/optional/doc/html/boost_optional/tutorial/gotchas/false_positive_with__wmaybe_uninitialized.html
  }
}

template <class Archive>
void TriggerKey::save(Archive& ar, unsigned version) const
{
  if (version>triggerkey_version_)
    log_fatal("Attempting to read version %u from file but running version %u of TriggerKey class.",version,triggerkey_version_);

  ar & make_nvp("SourceID", source_);
  ar & make_nvp("TypeID", type_);
  // for version > 0 there is a subtype ID to be serialized
  // for version <= 0 there is no subtype ID available ... use default value instead
  ar & make_nvp("SubtypeID", subtype_);
 
  bool configIDSet( configID_ ? true : false);
  ar & make_nvp("ConfigIDSet", configIDSet );
  if (configID_) ar & make_nvp("ConfigID", configID_.get() );
}

std::ostream& TriggerKey::Print(std::ostream& oss) const{
  oss << "[";
  oss << GetSourceString() << ":"
  << GetTypeString();
  if(CheckConfigID())
    oss << ":"<<GetConfigID() ;
  oss << "]" ;
  return oss;
}

std::ostream& operator<<(std::ostream& oss, const TriggerKey& k){
  return(k.Print(oss));
}

I3_SPLIT_SERIALIZABLE(TriggerKey);
