/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$
*/

#include <I3Test.h>

#include "dataclasses/Utility.h"
#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/I3Direction.h"

using namespace std;

TEST_GROUP(AsXML);

TEST(I3Position)
{
  I3Position pos;
  cout << AsXML(pos);
  I3PositionPtr pptr(new I3Position);
  cout << AsXML(pptr);
}

TEST(I3Direction)
{
  I3Direction dir;
  cout << AsXML(dir);
  I3DirectionPtr dirptr(new I3Direction);
  cout << AsXML(dirptr);
  I3DirectionConstPtr dircnstptr(new I3Direction);
  cout << AsXML(dircnstptr);
}

TEST(I3Particle)
{
  I3Particle muon;
  cout << AsXML(muon);
}

TEST(I3Geometry)
{
  I3Geometry geo;
  cout << AsXML(geo);
  I3GeometryPtr geoptr(new I3Geometry);
  cout << AsXML(geoptr);
  
  I3GeometryConstPtr geocp(new I3Geometry);
  cout << AsXML(geocp);
}
