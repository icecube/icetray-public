/**
    copyright  (C) 2006
    the icecube collaboration
    @version $Id$
    @date    $Date$
*/

#ifndef DATACLASSES_I3VECTOR_H_INCLUDED
#define DATACLASSES_I3VECTOR_H_INCLUDED

#include <string>
#include <icetray/serialization.h>
#include <icetray/I3FrameObject.h>
#include <icetray/ostream_pair.hpp>
#include <icetray/has_operator.h>
#include <vector>
#include "dataclasses/Utility.h"
#include "icetray/OMKey.h"
#include "dataclasses/ModuleKey.h"
#include "dataclasses/TankKey.h"
#include "dataclasses/I3Position.h"
#include "dataclasses/StationKey.h"

template <typename T>
struct I3Vector : public std::vector<T>, public I3FrameObject
{ 
  typedef std::vector<T> base_t;

  I3Vector() { }
  
  I3Vector(typename base_t::size_type s, const T& value) 
    : base_t(s, value) { }
    
  explicit I3Vector(typename base_t::size_type n) : base_t(n) { }

  I3Vector(const I3Vector& rhs) : base_t(rhs) { }

  explicit I3Vector(const base_t& rhs) : base_t(rhs) { }

  // strangely, this constructor-from-pair-of-iterators is not visible
  // to users of I3Vector.  When you add it here, it hides all the
  // other constructors, so you add them all, explicitly.

  template <typename Iterator>
  I3Vector(Iterator l, Iterator r) : base_t(l, r) { }

  template <class Archive>
  void serialize(Archive & ar, unsigned version)
  {
    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    ar & make_nvp("vector", base_object< std::vector<T> >(*this));
  }
  
  std::ostream& Print(std::ostream& os) const override{
    constexpr bool can_print=has_operator::insertion<std::ostream&,T>::value;
    return(PrintImpl(os,std::integral_constant<bool,can_print>()));
  }
  
private:
  std::ostream& PrintImpl(std::ostream& os, std::true_type can_print) const{
    os << '[';
    bool first=true;
    for(const auto& item : *this){
      if(first)
        first=false;
      else
        os << ", ";
      os << item;
    }
    os << ']';
    return os;
  }

  std::ostream& PrintImpl(std::ostream& os, std::false_type cant_print) const{
    os << '[' << this->size() << " element" << (this->size()==1?"":"s") << ']';
    return os;
  }
};
  
template <typename T>
std::ostream& operator<<(std::ostream& os, const I3Vector<T> v){
  return(v.Print(os));
}

// important to use the raw types here.  Don't do I3Vector<int64_t>, as that
// will be I3Vector<long> some places and I3Vector<long long> others, and then
// dynamic casting breaks since the two have different typeids. 
typedef I3Vector<bool> I3VectorBool;
typedef I3Vector<char> I3VectorChar;
typedef I3Vector<short> I3VectorShort;
typedef I3Vector<unsigned short> I3VectorUShort;
typedef I3Vector<int> I3VectorInt;
typedef I3Vector<unsigned int> I3VectorUInt;

#if __WORDSIZE == 64 || defined(_LP64)
typedef I3Vector<long> I3VectorInt64;
typedef I3Vector<unsigned long> I3VectorUInt64;
#else
typedef I3Vector<long long> I3VectorInt64;
typedef I3Vector<unsigned long long> I3VectorUInt64;
#endif

typedef I3Vector<std::string> I3VectorString;
typedef I3Vector<float> I3VectorFloat;
typedef I3Vector<double> I3VectorDouble;
typedef I3Vector<std::pair<double, double> > I3VectorDoubleDouble;
typedef I3Vector<I3Position> I3VectorI3Position;

typedef I3Vector<OMKey> I3VectorOMKey;
typedef I3Vector<TankKey> I3VectorTankKey;
typedef I3Vector<StationKey> I3VectorStationKey;
typedef I3Vector<ModuleKey> I3VectorModuleKey;

I3_POINTER_TYPEDEFS(I3VectorBool);
I3_POINTER_TYPEDEFS(I3VectorChar);
I3_POINTER_TYPEDEFS(I3VectorInt);
I3_POINTER_TYPEDEFS(I3VectorUInt);
I3_POINTER_TYPEDEFS(I3VectorInt64);
I3_POINTER_TYPEDEFS(I3VectorUInt64);
I3_POINTER_TYPEDEFS(I3VectorFloat);
I3_POINTER_TYPEDEFS(I3VectorDouble);
I3_POINTER_TYPEDEFS(I3VectorString);

I3_POINTER_TYPEDEFS(I3VectorOMKey);
I3_POINTER_TYPEDEFS(I3VectorTankKey);
I3_POINTER_TYPEDEFS(I3VectorStationKey);
I3_POINTER_TYPEDEFS(I3VectorModuleKey);
I3_POINTER_TYPEDEFS(I3VectorDoubleDouble);
I3_POINTER_TYPEDEFS(I3VectorI3Position);

// inconsistent... if you take this out, though, you have to be sure
// that you add it to I3Vector.cxx so that I3VectorUnsignedInt is
// always serialized with that name.  
typedef I3Vector<unsigned int> I3VectorUnsignedInt;
I3_POINTER_TYPEDEFS(I3VectorUnsignedInt);

#endif // I3VECTOR_H_INCLUDED

