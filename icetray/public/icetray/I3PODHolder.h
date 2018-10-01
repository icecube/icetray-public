/**
 *  $Id$
 *  
 *  Copyright (C) 2014
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */
#ifndef ICETRAY_I3PODHOLDER_H_INCLUDED
#define ICETRAY_I3PODHOLDER_H_INCLUDED

#include <boost/preprocessor/seq/for_each.hpp>

#include <icetray/IcetrayFwd.h>
#include <icetray/I3FrameObject.h>
#include <icetray/serialization.h>

#define REGISTER_PODHOLDER_OPERATORS(r, data, OP) \
bool operator OP(const I3PODHolder& other) const \
{ return value OP other.value; }

//
//  Dumb-struct holders of PODS should *not* automatically convert
//  to/from the underlying type lest people think that it is necessary
//  to use, say, vector<I3Double> instead of just vector<double>.
//  They are instead holders of values, with member "value".
//
template<typename T>
struct I3PODHolder : public I3FrameObject
{
  T value;

  I3PODHolder() { value = T(); }
  I3PODHolder(T v) : value(v) { }
  I3PODHolder(const I3PODHolder<T>& rhs) : value(rhs.value) { }
  I3PODHolder& operator=(const I3PODHolder<T>& rhs)
  {
    if (this != &rhs)
      value = rhs.value;
    return *this;
  }
  
  std::ostream& Print(std::ostream& os) const override{
    std::ios_base::fmtflags oldFlags=os.flags();
    os.setf(oldFlags|std::ios::boolalpha);
    os << value;
    os.setf(oldFlags);
    return os;
  }
  
  BOOST_PP_SEQ_FOR_EACH(REGISTER_PODHOLDER_OPERATORS, I3PODHolder<T>, (==)(!=)(<)(<=)(>)(>=) )

  template <typename Archive>
  void serialize(Archive& ar,unsigned version)
  {
    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    ar & make_nvp("value", value);
  }
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const I3PODHolder<T>& t){
  return(t.Print(os));
}

#undef REGISTER_PODHOLDER_OPERATORS

#endif
