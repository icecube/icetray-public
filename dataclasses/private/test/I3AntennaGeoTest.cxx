// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <I3Test.h>

#include <dataclasses/geometry/I3AntennaGeo.h>

TEST_GROUP(I3AntennaGeo);

TEST(constructor) {
  I3AntennaGeo theGeo;

  I3AntennaGeoPtr thePtr(new I3AntennaGeo());
}

TEST(setters_n_getters) {
  I3AntennaGeo theGeo;

  I3Position pos(1, 2, -57);
  theGeo.SetPosition(pos);
  ENSURE(pos == theGeo.GetPosition());

  I3Orientation orientation(1, 0, 0, 0, 0, 1);
  theGeo.SetOrientation(orientation);
  ENSURE(orientation == theGeo.GetOrientation());

  theGeo.SetAntennaName("Bob");
  ENSURE("Bob" == theGeo.GetAntennaName());

  theGeo.SetHeightAboveSnow(-99);
  ENSURE(-99 == theGeo.GetHeightAboveSnow());
}

TEST(comparison_operators) {
  I3AntennaGeo theGeo;
  I3Position pos(1, 2, -57);
  theGeo.SetPosition(pos);
  I3Orientation orientation(1, 0, 0, 0, 0, 1);
  theGeo.SetOrientation(orientation);
  theGeo.SetAntennaName("Bob");
  theGeo.SetHeightAboveSnow(-99);

  I3AntennaGeo theOtherGeo;
  theOtherGeo.SetPosition(pos);
  theOtherGeo.SetOrientation(orientation);
  theOtherGeo.SetAntennaName("Bob");
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

  theOtherGeo.SetAntennaName("Carl");
  ENSURE(theOtherGeo != theGeo);
  theOtherGeo.SetAntennaName("Bob");
  ENSURE(theOtherGeo == theGeo);

  theOtherGeo.SetHeightAboveSnow(0);
  ENSURE(theOtherGeo != theGeo);
  theOtherGeo.SetHeightAboveSnow(-99);
  ENSURE(theOtherGeo == theGeo);
}

TEST(radgeomap) {
  I3AntennaGeoMap geoMap;
  I3AntennaGeo theGeo;
  I3Position pos(1, 2, -57);
  theGeo.SetPosition(pos);
  I3Orientation orientation(1, 0, 0, 0, 0, 1);
  theGeo.SetOrientation(orientation);
  theGeo.SetAntennaName("Bob");
  theGeo.SetHeightAboveSnow(-99);

  AntennaKey key(0, 0);
  geoMap[key] = theGeo;
}

TEST(printing) {
  I3AntennaGeo theGeo;
  I3Position pos(1, 2, -57);
  theGeo.SetPosition(pos);
  I3Orientation orientation(1, 0, 0, 0, 0, 1);
  theGeo.SetOrientation(orientation);
  theGeo.SetAntennaName("Bob");
  theGeo.SetHeightAboveSnow(-99);

  std::cerr << theGeo << std::endl;
}
