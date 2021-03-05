#ifndef ICEACTKEY_H
#define ICEACTKEY_H

#include <iostream>
#include <icetray/serialization.h>
#include <icetray/I3PointerTypedefs.h>
#include <dataclasses/I3Vector.h>

static const unsigned iceactkey_version_ = 0;

class IceActKey {
 private:
  int32_t stationID_;
  int32_t telescopeID_;

 public:
  IceActKey(): stationID_(0), telescopeID_(0) {}
  IceActKey(int stn, int tel): stationID_(stn), telescopeID_(tel) {}
  ~IceActKey() {};

  void SetStationID(int stn) {stationID_ = stn;}
  void SetTelescopeID(int tel) {telescopeID_ = tel;}

  int GetStationID() const {return stationID_;}
  int GetTelescopeID() const {return telescopeID_;}

  bool operator==(const IceActKey& rhs) const {
    return stationID_ == rhs.GetStationID() && telescopeID_ == rhs.GetTelescopeID();
  }

  bool operator!=(const IceActKey& rhs) const {
    return !(rhs == *this);
  }

  void operator=(const IceActKey& rhs) {
    stationID_ = rhs.GetStationID();
    telescopeID_ = rhs.GetTelescopeID();
  }

 private:
  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive & ar, unsigned version);

};

bool operator<(const IceActKey& lhs, const IceActKey& rhs);

std::ostream& operator<<(std::ostream&, const IceActKey& key);

I3_POINTER_TYPEDEFS(IceActKey);

typedef I3Vector<IceActKey> I3VectorIceActKey;
I3_POINTER_TYPEDEFS(I3VectorIceActKey);

#endif
