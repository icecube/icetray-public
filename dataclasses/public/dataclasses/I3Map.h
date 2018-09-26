/**
    copyright  (C) 2004
    the icecube collaboration
    @version $Id$
    @date    $Date$
*/

#ifndef DATACLASSES_I3MAP_H_INCLUDED
#define DATACLASSES_I3MAP_H_INCLUDED

#include <icetray/serialization.h>
#include <map>
#include <string>
#include <vector>

#include <dataclasses/Utility.h>
#include <icetray/I3Logging.h>
#include <icetray/I3FrameObject.h>
#include <icetray/has_operator.h>
#include <icetray/OMKey.h>
#include "dataclasses/TriggerKey.h"
#include "dataclasses/ostream_overloads.hpp"

#ifndef __CINT__  // it puts the lotion in the basket
#include <boost/lexical_cast.hpp>
#endif

template <typename Key, typename Value>
struct I3Map : public I3FrameObject, public std::map<Key, Value>
{ 
  template <class Archive>
  void serialize(Archive & ar, unsigned version)
  {
    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    ar & make_nvp("map", base_object< std::map<Key, Value> >(*this));
  }

  ~I3Map();

  const Value& 
  at(const Key& where) const
  {
    typename std::map<Key, Value>::const_iterator iter = this->find(where);
    if (iter == this->end())
      log_fatal("Map contains nothing at %s.", boost::lexical_cast<std::string>(where).c_str());

    return iter->second;
  }

  Value& 
  at(const Key& where) 
  {
    typename std::map<Key, Value>::iterator iter = this->find(where);
    if (iter == this->end())
      log_fatal("Map contains nothing at %s.", boost::lexical_cast<std::string>(where).c_str());

    return iter->second;
  }
  
  std::ostream& Print(std::ostream& os) const override{
    constexpr bool can_print=has_operator::insertion<std::ostream&,Key>::value
                             && has_operator::insertion<std::ostream&,Value>::value;
    return(PrintImpl(os,std::integral_constant<bool,can_print>()));
  }

private:
  std::ostream& PrintImpl(std::ostream& os, std::true_type can_print) const{
    os << '[';
    bool first=true;
    for(const auto& entry : *this){
      if(first)
        first=false;
      else
        os << ",\n";
      os << entry.first << " => " << entry.second;
    }
    os << ']';
    return os;
  }

  std::ostream& PrintImpl(std::ostream& os, std::false_type cant_print) const{
    os << '[' << this->size() << " element" << (this->size()==1?"":"s") << ']';
    return os;
  }
};

template <typename Key, typename Value>
I3Map<Key, Value> :: ~I3Map() { }
  
template <typename Key, typename Value>
std::ostream& operator<<(std::ostream& os, const I3Map<Key, Value> m){
  return(m.Print(os));
}

typedef I3Map<std::string, double> I3MapStringDouble;
typedef I3Map<std::string, int> I3MapStringInt;
typedef I3Map<std::string, bool> I3MapStringBool;
typedef I3Map<std::string, std::string> I3MapStringString;

typedef I3Map<std::string, std::vector<double> > I3MapStringVectorDouble;


typedef I3Map<std::string, I3MapStringDouble> I3MapStringStringDouble;

typedef I3Map<unsigned, unsigned> I3MapUnsignedUnsigned;
typedef I3Map<unsigned short, unsigned short> I3MapUShortUShort;

typedef I3Map<int, std::vector<int> > I3MapIntVectorInt;
typedef I3Map<OMKey, std::vector<double> > I3MapKeyVectorDouble;
typedef I3Map<OMKey, std::vector<int> > I3MapKeyVectorInt;
typedef I3Map<OMKey, double > I3MapKeyDouble;
typedef I3Map<OMKey, unsigned int > I3MapKeyUInt;
typedef I3Map<TriggerKey, std::vector<unsigned int> > I3MapTriggerVectorUInt;
typedef I3Map<TriggerKey, double > I3MapTriggerDouble;
typedef I3Map<TriggerKey, unsigned int> I3MapTriggerUInt;

I3_POINTER_TYPEDEFS(I3MapStringDouble);
I3_POINTER_TYPEDEFS(I3MapStringInt);
I3_POINTER_TYPEDEFS(I3MapStringBool);
I3_POINTER_TYPEDEFS(I3MapStringString);
I3_POINTER_TYPEDEFS(I3MapStringVectorDouble);
I3_POINTER_TYPEDEFS(I3MapStringStringDouble);
I3_POINTER_TYPEDEFS(I3MapUnsignedUnsigned);
I3_POINTER_TYPEDEFS(I3MapUShortUShort);
I3_POINTER_TYPEDEFS(I3MapIntVectorInt);
I3_POINTER_TYPEDEFS(I3MapKeyVectorDouble);
I3_POINTER_TYPEDEFS(I3MapKeyVectorInt);
I3_POINTER_TYPEDEFS(I3MapKeyDouble);
I3_POINTER_TYPEDEFS(I3MapKeyUInt);
I3_POINTER_TYPEDEFS(I3MapTriggerVectorUInt);
I3_POINTER_TYPEDEFS(I3MapTriggerDouble);
I3_POINTER_TYPEDEFS(I3MapTriggerUInt);

#endif // I3MAP_H_INCLUDED

