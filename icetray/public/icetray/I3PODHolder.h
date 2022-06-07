/**
 *  $Id$
 *  
 *  Copyright (C) 2014
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *  
 *  SPDX-License-Identifier: BSD-2-Clause
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
