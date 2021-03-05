#include <I3Test.h>

#include <dataclasses/geometry/I3ScintGeo.h>

TEST_GROUP(I3ScintGeo);

TEST(constructor) {
  I3ScintGeo theGeo;
  ENSURE(I3ScintGeo::ScintType::Unknown == theGeo.GetScintType());

  I3ScintGeoPtr thePtr(new I3ScintGeo());
}

TEST(setters_n_getters) {
  I3ScintGeo theGeo;

  I3Position pos(1, 2, -57);
  theGeo.SetPosition(pos);
  ENSURE(pos == theGeo.GetPosition());

  I3Orientation orientation(1, 0, 0, 0, 0, 1);
  theGeo.SetOrientation(orientation);
  ENSURE(orientation == theGeo.GetOrientation());

  theGeo.SetScintType(I3ScintGeo::ScintType::MADKIT);
  ENSURE(I3ScintGeo::ScintType::MADKIT == theGeo.GetScintType());

  theGeo.SetScintName("Steve");
  ENSURE("Steve" == theGeo.GetScintName());

  theGeo.SetHeightAboveSnow(-99);
  ENSURE(-99 == theGeo.GetHeightAboveSnow());

}

TEST(comparison_operators) {
  I3ScintGeo theGeo;
  I3Position pos(1, 2, -57);
  theGeo.SetPosition(pos);
  I3Orientation orientation(1, 0, 0, 0, 0, 1);
  theGeo.SetOrientation(orientation);
  theGeo.SetScintType(I3ScintGeo::ScintType::MADKIT);
  theGeo.SetScintName("Steve");
  theGeo.SetHeightAboveSnow(-99);


  I3ScintGeo theOtherGeo;
  theOtherGeo.SetPosition(pos);
  theOtherGeo.SetOrientation(orientation);
  theOtherGeo.SetScintType(I3ScintGeo::ScintType::MADKIT);
  theOtherGeo.SetScintName("Steve");
  theOtherGeo.SetHeightAboveSnow(-99);

  ENSURE(theOtherGeo == theGeo);

  I3Position posOther(1, 3.14, -57);
  theOtherGeo.SetPosition(posOther);
  ENSURE(theOtherGeo != theGeo);
  theOtherGeo.SetPosition(pos);
  ENSURE(theOtherGeo == theGeo);

  I3Orientation orientationOther(1, 1, 0, 0, 0, 1);
  theOtherGeo.SetOrientation(orientationOther);
  ENSURE(theOtherGeo != theGeo);
  theOtherGeo.SetOrientation(orientation);
  ENSURE(theOtherGeo == theGeo);

  theOtherGeo.SetScintType(I3ScintGeo::ScintType::KIT);
  ENSURE(theOtherGeo != theGeo);
  theOtherGeo.SetScintType(I3ScintGeo::ScintType::MADKIT);
  ENSURE(theOtherGeo == theGeo);

  theOtherGeo.SetScintName("Carl");
  ENSURE(theOtherGeo != theGeo);
  theOtherGeo.SetScintName("Steve");
  ENSURE(theOtherGeo == theGeo);

  theOtherGeo.SetHeightAboveSnow(0);
  ENSURE(theOtherGeo != theGeo);
  theOtherGeo.SetHeightAboveSnow(-99);
  ENSURE(theOtherGeo == theGeo);


}

TEST(scintgeomap) {
  I3ScintGeoMap geoMap;
  I3ScintGeo theGeo;
  I3Position pos(1, 2, -57);
  theGeo.SetPosition(pos);
  I3Orientation orientation(1, 0, 0, 0, 0, 1);
  theGeo.SetOrientation(orientation);
  theGeo.SetScintType(I3ScintGeo::ScintType::MADKIT);
  theGeo.SetScintName("Steve");
  theGeo.SetHeightAboveSnow(-99);

  ScintKey key(0, 0);
  geoMap[key] = theGeo;
}

TEST(printing) {
  I3ScintGeo theGeo;
  I3Position pos(1, 2, -57);
  theGeo.SetPosition(pos);
  I3Orientation orientation(1, 0, 0, 0, 0, 1);
  theGeo.SetOrientation(orientation);
  theGeo.SetScintType(I3ScintGeo::ScintType::MADKIT);
  theGeo.SetScintName("Steve");
  theGeo.SetHeightAboveSnow(-99);

  std::cerr << theGeo << std::endl;
}
