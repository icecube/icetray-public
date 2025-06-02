// SPDX-FileCopyrightText: 2025 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef I3APPROACH_H_INCLUDED
#define I3APPROACH_H_INCLUDED

#include <iostream>
#include <icetray/serialization.h>
#include <icetray/I3PointerTypedefs.h>
#include <dataclasses/I3Vector.h>
#include <dataclasses/I3Position.h>

static const unsigned i3approach_version_ = 0;

class I3Approach {
private:
  double distance_;
  I3Position position_;

public:
  // constructors
  I3Approach(): distance_(NAN), position_() {}
  I3Approach(double d, I3Position pos): distance_(d), position_(pos) {}
  I3Approach(double d, double pos_x, double pos_y, double pos_z): distance_(d), position_(pos_x, pos_y, pos_z) {}
  // destructor
  ~I3Approach() {};

  // setters
  void SetDistance(double d) {distance_ = d;}
  void SetPosition(I3Position pos) {position_ = pos;}
  void SetPosition(double pos_x, double pos_y, double pos_z) {position_ = I3Position(pos_x, pos_y, pos_z);}

  // getters
  double GetDistance() const {return distance_;}
  I3Position GetPosition() const {return position_;}

  // comparators
  bool operator==(const I3Approach &rhs) const {
    return distance_ == rhs.GetDistance() && position_ == rhs.GetPosition();
  }
  bool operator!=(const I3Approach &rhs) const {
    return !(*this == rhs);
  }
  bool operator<(const I3Approach &rhs) const {
    return distance_ < rhs.GetDistance();
  }
  bool operator>(const I3Approach &rhs) const {
    return distance_ > rhs.GetDistance();
  }
  bool operator>=(const I3Approach &rhs) const {
    return !(*this < rhs);
  }
  bool operator<=(const I3Approach &rhs) const {
    return !(*this > rhs);
  }

  // copy another I3Approach
  void operator=(const I3Approach &rhs) {
    distance_ = rhs.GetDistance();
    position_ = rhs.GetPosition();
  }

private:
  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive & ar, unsigned version);

};

std::ostream& operator<<(std::ostream &os, const I3Approach &approach);

I3_POINTER_TYPEDEFS(I3Approach);

#endif // I3APPROACH_H_INCLUDED
