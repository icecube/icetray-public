/**
 *  $Id$
 *  
 *  Copyright (C) 2012
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

#ifndef MODULEKEY_H_INCLUDED
#define MODULEKEY_H_INCLUDED

#include <iostream>
#include <string>

#include <icetray/IcetrayFwd.h>
#include <icetray/I3FrameObject.h>
#include <icetray/serialization.h>

#include "dataclasses/I3Map.h"

static const unsigned modulekey_version_ = 0;

/**
 * @brief A small class which is the string number and om number
 * for a "module". Most of the time this will be an optical module.
 * Use this when you index something that is specific to an
 * optical module, not to a PMT inside it (it might have multiple PMTs).
 * If you need an index for PMTs (you will probably want this most of
 * the time), use the "OMKey" class.
 *
 */
class ModuleKey : public I3FrameObject
{
  int stringNumber_;
  unsigned int omNumber_;

 public:

  ModuleKey() : stringNumber_(0), omNumber_(0) {}

  ModuleKey(int str,unsigned int om) 
    : stringNumber_(str), omNumber_(om) {}

  ~ModuleKey();
  
  std::ostream& Print(std::ostream&) const override;

  /**
   * retrieves the string number for this ModuleKey
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
   * equality operator.  
   * @return true if the string and om numbers of the two ModuleKey's match
   * @param rhs the ModuleKey to compare this one to.
   */
  bool operator==(const ModuleKey& rhs) const
    {
      if(rhs.omNumber_ == omNumber_ && 
         rhs.stringNumber_ == stringNumber_)
        return true;
      return false;
    }

  /**
   * inequality operator
   * @return false if the string or om numbers are different
   * @param rhs the ModuleKey to compare this one to.
   */
  bool operator!=(const ModuleKey& rhs) const
    {
      if(rhs == *this)
        return false;
      return true;
    }

  struct hash
  {
    size_t operator()(const ModuleKey& key) const
    {
      return ((static_cast<size_t>(abs(key.GetString()+19)) * 64) + 
               static_cast<size_t>(key.GetOM()));
    }
  };

 private:
  friend class icecube::serialization::access;

  template <class Archive>
  void serialize(Archive& ar, unsigned version);
};

I3_CLASS_VERSION(ModuleKey,modulekey_version_);

/**
 * comparison operator.  First compares the string numbers, then compares
 * the om numbers.  Required to put ModuleKeys as the key of a map
 * @param lhs the left-hand ModuleKey
 * @param rhs the right-hand ModuleKey
 * @return true if the lhs should be ordered before the rhs
 */
inline bool operator<(const ModuleKey& lhs,const ModuleKey& rhs)
{
  if(lhs.GetString() < rhs.GetString()) {
    return true;
  } else if(lhs.GetString() > rhs.GetString()) {
    return false;
  } else if(lhs.GetOM() < rhs.GetOM()) {
    return true;
  } else {
    return false;
  }
}

/**
 * streams an ModuleKey to an arbitrary ostream.  These are important,
 * the tray uses these conversion internally.
 */
std::ostream& operator<<(std::ostream&, const ModuleKey& key);
std::istream& operator>>(std::istream&, ModuleKey&);

I3_POINTER_TYPEDEFS(ModuleKey);

typedef I3Map<ModuleKey, std::string> I3MapModuleKeyString;
I3_POINTER_TYPEDEFS(I3MapModuleKeyString);

#endif //MODULEKEY_H_INCLUDED
