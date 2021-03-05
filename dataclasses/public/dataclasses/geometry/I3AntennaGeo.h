////////////////////////////////////////////////////////////
//Author: Alan Coleman
//Description of the geometry for 1 antenna
//Based heavily on I3OMGeo.h
////////////////////////////////////////////////////////////
//
//Essentially follows the same implementation of I3OMGeo and/or I3TankGeo
//However neither have enough fields to completely describe the radio geom


#ifndef RADCUBE_I3ANTENNAGO_H
#define RADCUBE_I3ANTENNAGO_H

#include <string>

#include <dataclasses/I3Direction.h>
#include <dataclasses/I3Orientation.h>
#include <dataclasses/I3Position.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/Utility.h>

#include <dataclasses/AntennaKey.h>

static const unsigned i3antennageo_version_ = 0;

class I3AntennaGeo {
 public:

  enum AntennaType {
    Unknown = 0,
    SKALA2 = 1,
    SKALA4 = 2,
    AntennaTypeCount = 3
  };

  I3AntennaGeo(): antennaType_(Unknown) {}
  ~I3AntennaGeo();

  void SetPosition(I3Position &pos) {position_ = pos;}
  I3Position GetPosition() const {return position_;}

  void SetOrientation(I3Orientation &orient) {orientation_ = orient;}
  I3Orientation GetOrientation() const {return orientation_;}

  void SetAntennaType(AntennaType type) {antennaType_ = type;}
  AntennaType GetAntennaType() const {return antennaType_;}

  void SetAntennaName(std::string name) { antennaName_ = name;}
  std::string GetAntennaName() const {return antennaName_;}

  void SetHeightAboveSnow(double height) {heightAboveSnow_ = height;}
  double GetHeightAboveSnow() const {return heightAboveSnow_;}

  void SetCableLength(double length) {cableLength_ = length;}
  double GetCableLength() const {return cableLength_;}


  bool operator==(const I3AntennaGeo& rhs) const {
    return (position_ == rhs.position_ &&
            orientation_ == rhs.orientation_ &&
            antennaType_ == rhs.antennaType_ &&
            heightAboveSnow_ == rhs.heightAboveSnow_ &&
            antennaName_ == rhs.antennaName_ &&
            cableLength_ == rhs.cableLength_);
  }
  bool operator!=(const I3AntennaGeo& rhs) const {
    return !operator==(rhs);
  }

  std::ostream& Print(std::ostream&) const;

 private:

  I3Position position_;
  I3Orientation orientation_;
  AntennaType antennaType_;
  std::string antennaName_;
  double heightAboveSnow_; //In I3Units
  double cableLength_; //In I3Units

  friend class icecube::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, unsigned version);
};

std::ostream& operator<<(std::ostream&, const I3AntennaGeo&);

I3_POINTER_TYPEDEFS(I3AntennaGeo);
I3_CLASS_VERSION(I3AntennaGeo, i3antennageo_version_);

typedef I3Map<AntennaKey, I3AntennaGeo> I3AntennaGeoMap;
I3_POINTER_TYPEDEFS(I3AntennaGeoMap);

#endif
