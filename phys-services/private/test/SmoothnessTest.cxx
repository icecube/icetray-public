#include <I3Test.h>

#include "phys-services/I3Calculator.h"
#include "phys-services/I3Cuts.h"
#include "dataclasses/I3Position.h"
#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/physics/I3RecoPulse.h"

using namespace I3Calculator;
using namespace I3Cuts;

TEST_GROUP(SmoothnessTest);

namespace SmoothnessDirTest{
  void AddHitAtDepth(double depth,
		     I3Geometry& geometry,
		     I3RecoPulseSeriesMap& hitsmap,
		     unsigned position)
  {
    OMKey pos(1,position);
    I3OMGeo geo;
    geo.position=I3Position(0,0,depth,I3Position::car);
    geometry.omgeo[pos] = geo;

    I3RecoPulseSeries hits;
    I3RecoPulse h;
    //h.SetTime(time[i]);
    h.SetCharge(1.0);
    hits.push_back(h);
    hitsmap[pos] = hits;
  } 
}

TEST(verysmooth)
{
  I3Particle bt(I3Particle::InfiniteTrack);
  bt.SetPos(0,0,100);
  bt.SetDir(0,0);

  I3Geometry geometry;
  I3RecoPulseSeriesMap hitsmap;

  SmoothnessDirTest::AddHitAtDepth(0,geometry,hitsmap,1);
  SmoothnessDirTest::AddHitAtDepth(10,geometry,hitsmap,2);
  SmoothnessDirTest::AddHitAtDepth(20,geometry,hitsmap,3);
  SmoothnessDirTest::AddHitAtDepth(30,geometry,hitsmap,4);
  SmoothnessDirTest::AddHitAtDepth(40,geometry,hitsmap,5);
  SmoothnessDirTest::AddHitAtDepth(50,geometry,hitsmap,6);
  SmoothnessDirTest::AddHitAtDepth(60,geometry,hitsmap,7);
  SmoothnessDirTest::AddHitAtDepth(70,geometry,hitsmap,8);
  SmoothnessDirTest::AddHitAtDepth(80,geometry,hitsmap,9);
  SmoothnessDirTest::AddHitAtDepth(90,geometry,hitsmap,10);
  SmoothnessDirTest::AddHitAtDepth(100,geometry,hitsmap,11);

  std::cout<<SmoothAll(bt,geometry,hitsmap,-10,10)<<std::endl;//###
  ENSURE_DISTANCE(0.0,
		  SmoothAll(bt,geometry,hitsmap,-10,10),
		  0.001,
		  "Smoothness of evenly spaced hits should be 0");


}


TEST(lesssmooth_positive)
{
  I3Particle bt(I3Particle::InfiniteTrack);
  bt.SetPos(0,0,100);
  bt.SetDir(0,0);

  I3Geometry geometry;
  I3RecoPulseSeriesMap hitsmap;

  SmoothnessDirTest::AddHitAtDepth(0,geometry,hitsmap,1);
  SmoothnessDirTest::AddHitAtDepth(10,geometry,hitsmap,2);
  SmoothnessDirTest::AddHitAtDepth(20,geometry,hitsmap,3);
  SmoothnessDirTest::AddHitAtDepth(30,geometry,hitsmap,4);
  SmoothnessDirTest::AddHitAtDepth(40,geometry,hitsmap,5);
  SmoothnessDirTest::AddHitAtDepth(45,geometry,hitsmap,6);
  SmoothnessDirTest::AddHitAtDepth(60,geometry,hitsmap,7);
  SmoothnessDirTest::AddHitAtDepth(70,geometry,hitsmap,8);
  SmoothnessDirTest::AddHitAtDepth(80,geometry,hitsmap,9);
  SmoothnessDirTest::AddHitAtDepth(90,geometry,hitsmap,10);
  SmoothnessDirTest::AddHitAtDepth(100,geometry,hitsmap,11);

  ENSURE_DISTANCE(-5./100.,
		  SmoothAll(bt,geometry,hitsmap,-10,10),
		  0.001,
		  "Smoothness of evenly spaced hits should be predictable");


}


TEST(lesssmooth_negative)
{
  I3Particle bt(I3Particle::InfiniteTrack);
  bt.SetPos(0,0,100);
  bt.SetDir(0,0);

  I3Geometry geometry;
  I3RecoPulseSeriesMap hitsmap;

  SmoothnessDirTest::AddHitAtDepth(0,geometry,hitsmap,1);
  SmoothnessDirTest::AddHitAtDepth(10,geometry,hitsmap,2);
  SmoothnessDirTest::AddHitAtDepth(20,geometry,hitsmap,3);
  SmoothnessDirTest::AddHitAtDepth(30,geometry,hitsmap,4);
  SmoothnessDirTest::AddHitAtDepth(40,geometry,hitsmap,5);
  SmoothnessDirTest::AddHitAtDepth(55,geometry,hitsmap,6);
  SmoothnessDirTest::AddHitAtDepth(60,geometry,hitsmap,7);
  SmoothnessDirTest::AddHitAtDepth(70,geometry,hitsmap,8);
  SmoothnessDirTest::AddHitAtDepth(80,geometry,hitsmap,9);
  SmoothnessDirTest::AddHitAtDepth(90,geometry,hitsmap,10);
  SmoothnessDirTest::AddHitAtDepth(100,geometry,hitsmap,11);

  ENSURE_DISTANCE(5./100.,
		  SmoothAll(bt,geometry,hitsmap,-10,10),
		  0.001,
		  "Smoothness of evenly spaced hits should be predictable");


}


TEST(orderUnimportant)
{
  I3Particle bt(I3Particle::InfiniteTrack);
  bt.SetPos(0,0,100);
  bt.SetDir(0,0);

  I3Geometry geometry;
  I3RecoPulseSeriesMap hitsmap;

  SmoothnessDirTest::AddHitAtDepth(0,geometry,hitsmap,1);
  SmoothnessDirTest::AddHitAtDepth(10,geometry,hitsmap,6);
  SmoothnessDirTest::AddHitAtDepth(20,geometry,hitsmap,5);
  SmoothnessDirTest::AddHitAtDepth(30,geometry,hitsmap,10);
  SmoothnessDirTest::AddHitAtDepth(40,geometry,hitsmap,2);
  SmoothnessDirTest::AddHitAtDepth(50,geometry,hitsmap,7);
  SmoothnessDirTest::AddHitAtDepth(60,geometry,hitsmap,3);
  SmoothnessDirTest::AddHitAtDepth(70,geometry,hitsmap,8);
  SmoothnessDirTest::AddHitAtDepth(80,geometry,hitsmap,9);
  SmoothnessDirTest::AddHitAtDepth(90,geometry,hitsmap,4);
  SmoothnessDirTest::AddHitAtDepth(100,geometry,hitsmap,11);
  
  ENSURE_DISTANCE(0,
		  SmoothAll(bt,geometry,hitsmap,-10,10),
		  0.001,
		  "order shouldn't matter");
}


TEST(nohits)
{
  I3Particle bt(I3Particle::InfiniteTrack);
  bt.SetPos(0,0,100);
  bt.SetDir(0,0);

  I3Geometry geometry;
  I3RecoPulseSeriesMap hitsmap;

  ENSURE(std::isnan(SmoothAll(bt,geometry,hitsmap,-10,10)),
	 "should get NAN if there are no hits");


}


TEST(onehits)
{
  I3Particle bt(I3Particle::InfiniteTrack);
  bt.SetPos(0,0,100);
  bt.SetDir(0,0);

  I3Geometry geometry;
  I3RecoPulseSeriesMap hitsmap;

  SmoothnessDirTest::AddHitAtDepth(0,geometry,hitsmap,1);

  ENSURE(std::isnan(SmoothAll(bt,geometry,hitsmap,-10,10)),
	 "should get NAN if there are no hits");


}


TEST(twohits)
{
  I3Particle bt(I3Particle::InfiniteTrack);
  bt.SetPos(0,0,100);
  bt.SetDir(0,0);

  I3Geometry geometry;
  I3RecoPulseSeriesMap hitsmap;

  SmoothnessDirTest::AddHitAtDepth(0,geometry,hitsmap,1);
  SmoothnessDirTest::AddHitAtDepth(10,geometry,hitsmap,2);

  ENSURE(std::isnan(SmoothAll(bt,geometry,hitsmap,-10,10)),
	 "should get NAN if there are no hits");


}
