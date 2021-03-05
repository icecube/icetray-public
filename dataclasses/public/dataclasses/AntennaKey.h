#ifndef ANTENNAKEY_H
#define ANTENNAKEY_H

#include <iostream>
#include <icetray/serialization.h>
#include <icetray/I3PointerTypedefs.h>
#include <dataclasses/I3Vector.h>

static const unsigned antennakey_version_ = 0;

class AntennaKey {
 private:
  int32_t stationID_;
  int32_t antennaID_;

 public:
  AntennaKey(): stationID_(0), antennaID_(0) {}
  AntennaKey(int stn, int ant): stationID_(stn), antennaID_(ant) {}
  ~AntennaKey() {};

  void SetStationID(int stn) {stationID_ = stn;}
  void SetAntennaID(int ant) {antennaID_ = ant;}

  int GetStationID() const {return stationID_;}
  int GetAntennaID() const {return antennaID_;}

  bool operator==(const AntennaKey& rhs) const {
    return stationID_ == rhs.GetStationID() && antennaID_ == rhs.GetAntennaID();
  }

  bool operator!=(const AntennaKey& rhs) const {
    return !(rhs == *this);
  }

  void operator=(const AntennaKey& rhs) {
    stationID_ = rhs.GetStationID();
    antennaID_ = rhs.GetAntennaID();
  }

 private:
  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive & ar, unsigned version);

};

bool operator<(const AntennaKey& lhs, const AntennaKey& rhs);

std::ostream& operator<<(std::ostream&, const AntennaKey& key);

I3_POINTER_TYPEDEFS(AntennaKey);

typedef I3Vector<AntennaKey> I3VectorAntennaKey;
I3_POINTER_TYPEDEFS(I3VectorAntennaKey);

#endif
