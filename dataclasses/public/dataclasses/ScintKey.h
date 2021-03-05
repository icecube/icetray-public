#ifndef SCINTKEY_H
#define SCINTKEY_H

#include <iostream>
#include <icetray/serialization.h>
#include <icetray/I3PointerTypedefs.h>
#include <dataclasses/I3Vector.h>

static const unsigned scintkey_version_ = 0;

class ScintKey {
 private:
  int32_t stationID_;
  int32_t panelID_;

 public:
  ScintKey(): stationID_(0), panelID_(0) {}
  ScintKey(int stn, int pan): stationID_(stn), panelID_(pan) {}
  ~ScintKey() {};



  void SetStationID(int stn) {stationID_ = stn;}
  void SetPanelID(int pan) {panelID_ = pan;}

  int GetStationID() const {return stationID_;}
  int GetPanelID() const {return panelID_;}

  bool operator==(const ScintKey& rhs) const {
    return stationID_ == rhs.GetStationID() && panelID_ == rhs.GetPanelID();
  }

  bool operator!=(const ScintKey& rhs) const {
    return !(rhs == *this);
  }

  void operator=(const ScintKey& rhs) {
    stationID_ = rhs.GetStationID();
    panelID_ = rhs.GetPanelID();
  }

 private:
  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive & ar, unsigned version);

};

bool operator<(const ScintKey& lhs, const ScintKey& rhs);

std::ostream& operator<<(std::ostream&, const ScintKey& key);

I3_POINTER_TYPEDEFS(ScintKey);

typedef I3Vector<ScintKey> I3VectorScintKey;
I3_POINTER_TYPEDEFS(I3VectorScintKey);

#endif
