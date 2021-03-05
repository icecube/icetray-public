
#ifndef I3SCINTGEO_H
#define I3SCINTGEO_H

#include <string>

#include <dataclasses/I3Direction.h>
#include <dataclasses/I3Orientation.h>
#include <dataclasses/I3Position.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/Utility.h>

#include <dataclasses/ScintKey.h>

static const unsigned i3scintgeo_version_ = 0;

class I3ScintGeo {
 public:

  enum ScintType {
    Unknown = 0,
    KIT = 1,
    MAD = 2,
    MADKIT = 3
  };

  I3ScintGeo():
	  position(I3Position(0,0,0)),
  	  orientation(I3Orientation(I3Direction(0,0))),
	  scintType(Unknown),
	  scintName("Scint_default"),
	  heightAboveSnow(0)
  {};
  ~I3ScintGeo();

  I3Position position;
  I3Orientation orientation;
  ScintType scintType;
  std::string scintName;
  double heightAboveSnow; //In I3Units

  void SetPosition(I3Position &pos) {position = pos;}
  I3Position GetPosition() const {return position;}

  void SetOrientation(I3Orientation &orient) {orientation = orient;}
  I3Orientation GetOrientation() const {return orientation;}

  void SetScintType(ScintType type) {scintType = type;}
  ScintType GetScintType() const {return scintType;}

  void SetScintName(std::string name) { scintName = name;}
  std::string GetScintName() const {return scintName;}

  void SetHeightAboveSnow(double height) {heightAboveSnow = height;}
  double GetHeightAboveSnow() const {return heightAboveSnow;}


  bool operator==(const I3ScintGeo& rhs) const {
    return (position == rhs.position &&
            orientation == rhs.orientation &&
            scintType == rhs.scintType &&
            heightAboveSnow == rhs.heightAboveSnow &&
            scintName == rhs.scintName);
  }
  bool operator!=(const I3ScintGeo& rhs) const {
    return !operator==(rhs);
  }

  std::ostream& Print(std::ostream&) const;

 private:

  friend class icecube::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, unsigned version);
};

std::ostream& operator<<(std::ostream&, const I3ScintGeo&);

I3_POINTER_TYPEDEFS(I3ScintGeo);
I3_CLASS_VERSION(I3ScintGeo, i3scintgeo_version_);

typedef I3Map<ScintKey, I3ScintGeo> I3ScintGeoMap;
I3_POINTER_TYPEDEFS(I3ScintGeoMap);

#endif
