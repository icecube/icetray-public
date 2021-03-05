
#ifndef I3ICEACTGEO_H
#define I3ICEACTGEO_H

#include <string>

#include <dataclasses/I3Direction.h>
#include <dataclasses/I3Orientation.h>
#include <dataclasses/I3Position.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/Utility.h>

#include <dataclasses/IceActKey.h>

static const unsigned i3iceactgeo_version_ = 0;

class I3IceActGeo {
 public:

  enum IceActType {
    Unknown = 0,
	drs_2019 = 1,
	target_C = 2,
	drs_2019_64 = 3,
	simulation= 4,
  };

  I3IceActGeo():
	position(I3Position(0,0,0)),
	orientation(I3Orientation(I3Direction(0,0))),
	iceactType(Unknown),
	iceactName("IceAct_default"),
	numberofPixels(0),
	pixelSize(0),
	pixelstoChannels(),
	pixelPositions()
  {};
  ~I3IceActGeo();

  I3Position position;
  I3Orientation orientation;
  IceActType iceactType;
  std::string iceactName;
  int numberofPixels;
  float pixelSize;
  I3Map<unsigned, unsigned> pixelstoChannels;
  I3Map<unsigned, I3Position> pixelPositions;

  void SetPosition(I3Position &pos) {position = pos;}
  I3Position GetPosition() const {return position;}

  void SetOrientation(I3Orientation &orient) {orientation = orient;}
  I3Orientation GetOrientation() const {return orientation;}

  void SetIceActType(IceActType type) {iceactType = type;}
  IceActType GetIceActType() const {return iceactType;}

  void SetIceActName(std::string name) { iceactName = name;}
  std::string GetIceActName() const {return iceactName;}

  void SetNumberofPixels(int numpixels){ numberofPixels = numpixels; }
  int GetNumberofPixels()  {return numberofPixels; }

  void SetPixelSize(float pixSize)	{pixelSize = pixSize; }
  float GetPixelSize() const {return pixelSize; }

  void SetPixelsPositions(I3Map<unsigned, I3Position> &pixelpos) { pixelPositions = pixelpos; }
  I3Map<unsigned, I3Position> GetPixelsPositions() const { return pixelPositions; }

  void SetPixelstoChannels(I3Map<unsigned, unsigned> pixelchannel) {pixelstoChannels = pixelchannel;}
  I3Map<unsigned, unsigned> GetPixelstoChannels() const	{return pixelstoChannels;}



  bool operator==(const I3IceActGeo& rhs) const {
    return (position == rhs.position &&
            orientation == rhs.orientation &&
            iceactType == rhs.iceactType &&
            iceactName == rhs.iceactName &&
            numberofPixels == rhs.numberofPixels &&
            pixelSize == rhs.pixelSize &&
            pixelPositions == rhs.pixelPositions &&
            pixelstoChannels == rhs.pixelstoChannels
            );

  }
  bool operator!=(const I3IceActGeo& rhs) const {
    return !operator==(rhs);
  }

  std::ostream& Print(std::ostream&) const;

 private:

  friend class icecube::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, unsigned version);
};

std::ostream& operator<<(std::ostream&, const I3IceActGeo&);

I3_POINTER_TYPEDEFS(I3IceActGeo);
I3_CLASS_VERSION(I3IceActGeo, i3iceactgeo_version_);

typedef I3Map<unsigned, I3Position> I3IceActPixelPositionMap;
I3_POINTER_TYPEDEFS(I3IceActPixelPositionMap);

typedef I3Map<IceActKey, I3IceActGeo> I3IceActGeoMap;
I3_POINTER_TYPEDEFS(I3IceActGeoMap);

#endif
