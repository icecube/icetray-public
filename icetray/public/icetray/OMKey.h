/**
 *  $Id$
 *  
 *  Copyright (C) 2003-2007
 *  The IceCube Collaboration <http://www.icecube.wisc.edu>
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

#ifndef OMKEY_H_INCLUDED
#define OMKEY_H_INCLUDED

#include <utility>
#include "Utility.h"
#include <iostream>
#include <icetray/IcetrayFwd.h>
#include <icetray/serialization.h>

static const unsigned omkey_version_ = 2;
#pragma GCC diagnostic push
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

/**
 * @brief A small class which is the string number, om number
 * and pmt number for a specific PMT inside a DOM.
 *
 * For IceCube, the PMT number will always be 0
 * and "PMT" is equivalent to "DOM". For IceTop, the PMT number
 * can be 0 or 1.
 */

class OMKey 
{
  int stringNumber_;
  unsigned int omNumber_;
  unsigned char pmtNumber_;

 public:

  OMKey() : stringNumber_(0), omNumber_(0), pmtNumber_(0) {}

  OMKey(int str,unsigned int om) 
    : stringNumber_(str), omNumber_(om), pmtNumber_(0) {}

  OMKey(int str,unsigned int om, unsigned char pmt) 
    : stringNumber_(str), omNumber_(om), pmtNumber_(pmt) {}

  virtual ~OMKey(); 

  /**
   * retrieves the string number for this OMKey
   */
  int GetString() const { return stringNumber_; }

  /**
   * Sets the string number for this OM
   */
  void SetString(int str){ stringNumber_ = str; }

  /**
   * gets the OM number on the string
   */
  unsigned int GetOM() const { return omNumber_; }

  /**
   * sets the OM number on the string
   */
  void SetOM(unsigned int om){ omNumber_ = om; }

  /**
   * gets the PMT number in the DOM
   */
  unsigned char GetPMT() const { return pmtNumber_; }
    
  /**
   * sets the PMT number in the DOM
   */
  void SetPMT(unsigned char pmt){ pmtNumber_ = pmt; }

  /**
   * equality operator.  
   * @return true if the string and om numbers of the two OMKey's match
   * @param rhs the OMKey to compare this one to.
   */
  bool operator==(const OMKey& rhs) const
    {
      if(rhs.omNumber_ == omNumber_ && 
         rhs.stringNumber_ == stringNumber_ && 
         rhs.pmtNumber_ == pmtNumber_)
	return true;
      return false;
    }

  /**
   * inequality operator
   * @return false if the string or om numbers are different
   * @param rhs the OMKey to compare this one to.
   */
  bool operator!=(const OMKey& rhs) const
    {
      if(rhs == *this)
	return false;
      return true;
    }

  std::string str() const;

  struct hash
  {
    size_t operator()(const OMKey& key) const
    {
      return (((static_cast<size_t>(abs(key.GetString()+19)) * 64) + 
               static_cast<size_t>(key.GetOM()))) * 256 + 
              static_cast<size_t>(key.GetPMT());
    }
  };

 private:
  friend class icecube::serialization::access;

  template <class Archive>
  void save(Archive& ar, unsigned version) const;

  template <class Archive>
  void load(Archive& ar, unsigned version);

  I3_SERIALIZATION_SPLIT_MEMBER()
};

I3_CLASS_VERSION(OMKey,omkey_version_);

/**
 * comparison operator.  First compares the string numbers, then compares
 * the om numbers.  Required to put OMKeys as the key of a map
 * @param lhs the left-hand OMKey
 * @param rhs the right-hand OMKey
 * @return true if the lhs should be ordered before the rhs
 */
inline bool operator<(const OMKey& lhs,const OMKey& rhs)
{
  if(lhs.GetString() < rhs.GetString()) {
    return true;
  } else if(lhs.GetString() > rhs.GetString()) {
    return false;
  } else if(lhs.GetOM() < rhs.GetOM()) {
    return true;
  } else if(lhs.GetOM() > rhs.GetOM()) {
    return false;
  } else if(lhs.GetPMT() < rhs.GetPMT()) {
    return true;
  } else {
    return false;
  }
}

/**
 * streams an OMKey to an arbitrary ostream.  These are important,
 * the tray uses these conversion internally.
 */
std::ostream& operator<<(std::ostream&, const OMKey& key);
std::istream& operator>>(std::istream&,  OMKey&);
#pragma GCC diagnostic pop

I3_POINTER_TYPEDEFS(OMKey);

#endif //OMKEY_H_INCLUDED
