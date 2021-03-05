#include <I3Test.h>

#include <dataclasses/geometry/I3IceActGeo.h>

TEST_GROUP(I3IceActGeo);

TEST(constructor) {
  I3IceActGeo theGeo;
  ENSURE(I3IceActGeo::IceActType::Unknown == theGeo.GetIceActType());

  I3IceActGeoPtr thePtr(new I3IceActGeo());
}

TEST(setters_n_getters) {
  I3IceActGeo theGeo;

  I3Position pos(1, 2, -57);
  theGeo.SetPosition(pos);
  ENSURE(pos == theGeo.GetPosition());

  I3Orientation orientation(1, 0, 0, 0, 0, 1);
  theGeo.SetOrientation(orientation);
  ENSURE(orientation == theGeo.GetOrientation());

  theGeo.SetIceActType(I3IceActGeo::IceActType::target_C);
  ENSURE(I3IceActGeo::IceActType::target_C == theGeo.GetIceActType());

  theGeo.SetIceActName("Dieter");
  ENSURE("Dieter" == theGeo.GetIceActName());

  theGeo.SetNumberofPixels(64);
  ENSURE(64 == theGeo.GetNumberofPixels());

  theGeo.SetPixelSize(1);
  ENSURE(1 == theGeo.GetPixelSize());

  I3Map<unsigned, I3Position> pixmap;
  pixmap.insert(std::make_pair(1,pos));
  theGeo.SetPixelsPositions(pixmap);
  ENSURE(pixmap == theGeo.GetPixelsPositions());

  I3Map<unsigned, unsigned> chnmap;
  chnmap.insert(std::make_pair(1,1));
  theGeo.SetPixelstoChannels(chnmap);
  ENSURE(chnmap == theGeo.GetPixelstoChannels());
}

TEST(comparison_operators) {
  I3IceActGeo theGeo;
  I3Position pos(1, 2, -57);
  theGeo.SetPosition(pos);
  I3Orientation orientation(1, 0, 0, 0, 0, 1);
  theGeo.SetOrientation(orientation);
  theGeo.SetIceActType(I3IceActGeo::IceActType::target_C);
  theGeo.SetIceActName("Dieter");
  theGeo.SetNumberofPixels(64);
  theGeo.SetPixelSize(1);

  I3Map<unsigned, I3Position> pixmap;
  pixmap.insert(std::make_pair(1,pos));
  theGeo.SetPixelsPositions(pixmap);
  I3Map<unsigned, unsigned> chnmap;
  chnmap.insert(std::make_pair(1,1));
  theGeo.SetPixelstoChannels(chnmap);

  I3IceActGeo theOtherGeo;
  theOtherGeo.SetPosition(pos);
  theOtherGeo.SetOrientation(orientation);
  theOtherGeo.SetIceActType(I3IceActGeo::IceActType::target_C);
  theOtherGeo.SetIceActName("Dieter");
  theOtherGeo.SetNumberofPixels(64);
  theOtherGeo.SetPixelSize(1);
  theOtherGeo.SetPixelsPositions(pixmap);
  theOtherGeo.SetPixelstoChannels(chnmap);
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

  theOtherGeo.SetIceActType(I3IceActGeo::IceActType::drs_2019);
  ENSURE(theOtherGeo != theGeo);
  theOtherGeo.SetIceActType(I3IceActGeo::IceActType::target_C);
  ENSURE(theOtherGeo == theGeo);

  theOtherGeo.SetIceActName("Thomas");
  ENSURE(theOtherGeo != theGeo);
  theOtherGeo.SetIceActName("Dieter");
  ENSURE(theOtherGeo == theGeo);

  theOtherGeo.SetNumberofPixels(63);
  ENSURE(theOtherGeo != theGeo);
  theOtherGeo.SetNumberofPixels(64);
  ENSURE(theOtherGeo == theGeo);

  theOtherGeo.SetPixelSize(2);
  ENSURE(theOtherGeo != theGeo);
  theOtherGeo.SetPixelSize(1);
  ENSURE(theOtherGeo == theGeo);

  I3Map<unsigned, I3Position> otherpixmap;
  otherpixmap.insert(std::make_pair(2,pos));

  theOtherGeo.SetPixelsPositions(otherpixmap);
  ENSURE(theOtherGeo != theGeo);
  theOtherGeo.SetPixelsPositions(pixmap);
  ENSURE(theOtherGeo == theGeo);

  I3Map<unsigned, unsigned> otherchnmap;
  otherchnmap.insert(std::make_pair(2,2));

  theOtherGeo.SetPixelstoChannels(otherchnmap);
  ENSURE(theOtherGeo != theGeo);
  theOtherGeo.SetPixelstoChannels(chnmap);
  ENSURE(theOtherGeo == theGeo);

}

TEST(IceActgeomap) {
  I3IceActGeoMap geoMap;
  I3IceActGeo theGeo;
  I3Position pos(1, 2, -57);
  theGeo.SetPosition(pos);
  I3Orientation orientation(1, 0, 0, 0, 0, 1);
  theGeo.SetOrientation(orientation);
  theGeo.SetIceActType(I3IceActGeo::IceActType::target_C);
  theGeo.SetIceActName("Dieter");
  theGeo.SetNumberofPixels(64);
  theGeo.SetPixelSize(1);

  IceActKey key(0, 0);
  geoMap[key] = theGeo;
}

TEST(printing) {
  I3IceActGeo theGeo;
  I3Position pos(1, 2, -57);
  theGeo.SetPosition(pos);
  I3Orientation orientation(1, 0, 0, 0, 0, 1);
  theGeo.SetOrientation(orientation);
  theGeo.SetIceActType(I3IceActGeo::IceActType::target_C);
  theGeo.SetIceActName("Dieter");
  theGeo.SetNumberofPixels(64);
  theGeo.SetPixelSize(1);

  std::cerr << theGeo << std::endl;
}
