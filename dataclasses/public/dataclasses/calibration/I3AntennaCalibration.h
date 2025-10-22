/**
 *
 * Definition of I3AntennaCal class, storing calibration info for the surface radio array.
 *
 * Copyright  (C) 2025 the IceCube collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 * @version $Id$
 * @file I3AntennaCalibration.h
 * @date $Date$
 */


#ifndef I3ANTENNACALIBRATION_H_INCLUDED
#define I3ANTENNACALIBRATION_H_INCLUDED

#include <map>

#include <dataclasses/I3Map.h>
#include <dataclasses/Utility.h>
#include <dataclasses/AntennaKey.h>

#include <dataclasses/I3Time.h>
#include <icetray/I3FrameObject.h>
#include <icetray/I3DefaultName.h>

static const unsigned i3antennacalibration_version_ = 0;

class I3AntennaCal {
public:

  enum class AntennaType {
    Unknown = 0,
    SKALA2 = 1,
    SKALA4 = 2,
    AntennaTypeCount = 3
  };

  enum class CableType {
    Unknown = 0,
    LMR400 = 1
  };

  // Combines response of radioTad ("Board") and TAXI
  enum class DaqType {
    Unknown = 0,
    Board1_Taxi3_0 = 1, // radioTad v1 + TAXI v3.0
    Board2_Taxi3_2 = 2  // radioTad v2 + TAXI v3.2
  };

  I3AntennaCal(): antennaType_(AntennaType::Unknown), cableType_(CableType::Unknown), daqType_(DaqType::Unknown), cableLength_(std::numeric_limits<double>::quiet_NaN()) {}
  ~I3AntennaCal();

  void SetAntennaType(AntennaType type) {antennaType_ = type;}
  AntennaType GetAntennaType() const {return antennaType_;}

  void SetCableType(CableType type) {cableType_ = type;}
  CableType GetCableType() const {return cableType_;}

  void SetDaqType(DaqType version) {daqType_ = version;}
  DaqType GetDaqType() const {return daqType_;}

  void SetCableLength(double length) {cableLength_ = length;}
  double GetCableLength() const {return cableLength_;}

  bool operator==(const I3AntennaCal& rhs) const {
    return (antennaType_ == rhs.antennaType_ &&
            cableType_ == rhs.cableType_ &&
            daqType_ == rhs.daqType_ &&
            cableLength_ == rhs.cableLength_);
}

bool operator!=(const I3AntennaCal& rhs) const {
  return !operator==(rhs);
}

// Define hash function to be able to use as key in unordered_map in radcube I3ElectronicsResponse
// Inspired by http://stackoverflow.com/a/1646913/126995
struct hash
{
  std::size_t operator()(const I3AntennaCal& c) const
  {
    size_t res = 17;
    res = res * 31 + std::hash<int>()(static_cast<int>(c.antennaType_));
    res = res * 31 + std::hash<int>()(static_cast<int>(c.cableType_));
    res = res * 31 + std::hash<int>()(static_cast<int>(c.daqType_));
    res = res * 31 + std::hash<double>()(static_cast<int>(c.cableLength_));
    return res;
  }
};

std::ostream& Print(std::ostream&) const;

private:
  AntennaType antennaType_;
  CableType cableType_;
  DaqType daqType_;
  double cableLength_; //In I3Units

  friend class icecube::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, unsigned version);
};

std::ostream& operator<<(std::ostream&, const I3AntennaCal&);

std::ostream& operator<<(std::ostream&, const I3AntennaCal::AntennaType);
std::ostream& operator<<(std::ostream&, const I3AntennaCal::CableType);
std::ostream& operator<<(std::ostream&, const I3AntennaCal::DaqType);


I3_POINTER_TYPEDEFS(I3AntennaCal);
I3_CLASS_VERSION(I3AntennaCal, i3antennacalibration_version_);

// define the map
typedef I3Map<AntennaKey, I3AntennaCal> I3AntennaCalMap;
I3_POINTER_TYPEDEFS(I3AntennaCalMap);


// Define a frame object which holds an I3AntennaCalMap in the C frame
static const unsigned i3radcalibration_version_ = 0;

class I3AntennaCalibration : public I3FrameObject
{
public:
  I3Time startTime;
  I3Time endTime;

  I3AntennaCalibration();
    
  ~I3AntennaCalibration();
  
  I3AntennaCalMap antennaCalMap;
  
  bool operator==(const I3AntennaCalibration& rhs)
  {
    return (startTime == rhs.startTime &&
            endTime == rhs.endTime &&
            antennaCalMap == rhs.antennaCalMap);
  }
  bool operator!=(const I3AntennaCalibration& rhs)
  {
    return !operator==(rhs);
  }
  
private:
  friend class icecube::serialization::access;
  template <class Archive> void load(Archive & ar, unsigned version);
  template <class Archive> void save(Archive & ar, unsigned version) const;
  I3_SERIALIZATION_SPLIT_MEMBER();
};

I3_CLASS_VERSION(I3AntennaCalibration, i3radcalibration_version_);
I3_DEFAULT_NAME(I3AntennaCalibration);
I3_POINTER_TYPEDEFS(I3AntennaCalibration);    

#endif // I3ANTENNACALIBRATION_H_INCLUDED