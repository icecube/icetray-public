/**
 *  $Id$
 *  
 *  Copyright (C) 2003-2007
 *  The IceCube Collaboration <http://www.icecube.wisc.edu>
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

#ifndef OMKEY_H_INCLUDED
#define OMKEY_H_INCLUDED

#include <utility>
#include "Utility.h"
#include <iostream>
#include <icetray/IcetrayFwd.h>
#include <icetray/serialization.h>

static const unsigned omkey_version_ = 2;

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
   * bool function: is it an InIce DOM?
   */
  bool IsInIce() const { 
      // only strings 1-86 have IceTop
    if((stringNumber_<1) || (stringNumber_>86)) return true;
    if(omNumber_>=1 && omNumber_<=60) return true;
    else return false;
  }

  /**
   * bool function: is it an IceTop DOM?
   */
  bool IsIceTop() const { 
    // only strings 1-86 have IceTop
    if((stringNumber_<1) || (stringNumber_>86)) return false; 
    if(omNumber_>=61 && omNumber_<=64) return true;
    else return false;
  }

  /**
   * bool function: is it a scintillator DOM?
   */
  bool IsScintillator() const {
    if(omNumber_>=65 && omNumber_<=66) return true;
    else return false;
  }

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
  friend class boost::serialization::access;

  template <class Archive>
  void save(Archive& ar, unsigned version) const;

  template <class Archive>
  void load(Archive& ar, unsigned version);

  BOOST_SERIALIZATION_SPLIT_MEMBER();
};

BOOST_CLASS_VERSION(OMKey,omkey_version_);

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

I3_POINTER_TYPEDEFS(OMKey);

#endif //OMKEY_H_INCLUDED
