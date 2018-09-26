/**
 * copyright  (C) 2004
 * the icecube collaboration
 * @version $Id$
 * @file TankKey.h
 * @date $Date$
 */

#ifndef TANKKEY_H_INCLUDED
#define TANKKEY_H_INCLUDED

#include <utility>
#include <icetray/OMKey.h>
#include <icetray/serialization.h>
#include <iostream>
/**
 * @brief A small class which is the string number and tank number
 * for an IceTop tank
 *
 */
static const unsigned tankkey_version_ = 0;

struct TankKey {
public:
  enum TankID { TankA=0, TankB=1 };

  int string;
  TankID tank;


  TankKey()
    : string(0), tank(TankA)
  {}

  TankKey(int str,TankID tank_)
    : string(str), tank(tank_)
  {}
  
  explicit TankKey(const OMKey& omKey)
  {
    SetOMKey(omKey);
  }

  ~TankKey() {}

  std::ostream& Print(std::ostream&) const;
  
  /**
   * Sets the stringNumber and tankID of this tank according to the OMKey
   */
  void SetOMKey(const OMKey& omKey);

  /**
   * Get one of the DOMs inside the tank.
   * By convention, this returns the OMKey with the lower omNumber.
   *
   * Use this as an adapter to use TankKeys with I3MapOMKey* frame
   * objects. This is an effective work-around for the fact that we
   * don't have I3MapTankKey* frame objects.
   *
   * Example use-case: You want to count muons that hit tanks, the
   * tanks are defined by a collection of TankKeys. Make a
   * I3MapOMKeyUInt called "mymap". If a tank is hit, do
   * mymap[tankKey.GetDefaultOMKey()] += 1, to increase the count
   * for that tank.
   *
   * Later in the analysis, you can recover the TankKeys from mymap
   * using the special constructor TankKey(const OMkey& omKey) or
   * TankKey::SetOMKey(const OMKey& omKey).
   */
  OMKey GetDefaultOMKey() const;

  /**
   * equality operator.  
   * @return true if the string and tank numbers of the two TankKey's match
   * @param rhs the TankKey to compare this one to.
   */
  bool operator==(const TankKey& rhs) const
  {
    if(rhs.tank == tank && rhs.string == string)
      return true;
    return false;
  }

  /**
   * inequality operator
   * @return false if the string or tank numbers are different
   * @param rhs the TankKey to compare this one to.
   */
  bool operator!=(const TankKey& rhs) const { return !(rhs == *this); }


private:
  friend class icecube::serialization::access;

  template <class Archive>
  void serialize(Archive& ar, unsigned version);
};


/**
 * comparison operator.  First compares the string numbers, then compares
 * the tank numbers.  Required to put TankKeys as the key of a map
 * @param lhs the left-hand TankKey
 * @param rhs the right-hand TankKey
 * @return true if the lhs should be ordered before the rhs
 */
inline bool operator<(const TankKey& lhs, const TankKey& rhs)
{
  if(lhs.string < rhs.string)
    return true;
  if(lhs.string > rhs.string)
    return false;
  if(lhs.tank < rhs.tank)
    return true;
  return false;
}

/**
 * streams an TankKey to an arbitrary ostream.
 */
std::ostream& operator<<(std::ostream&, const TankKey& key);


I3_POINTER_TYPEDEFS(TankKey);
I3_CLASS_VERSION(TankKey, tankkey_version_);

#endif //TANKKEY_H_INCLUDED
