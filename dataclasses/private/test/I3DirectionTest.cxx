/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$

    @version $Revision$
    @date $Date$
    @author dule
*/

#include <I3Test.h>

#include "dataclasses/I3Direction.h"
#include "dataclasses/I3Position.h"
#include "icetray/I3Units.h"
#include <string>
#include <cmath>
using std::string;
using std::cout;
using std::endl;
using namespace I3Units;

TEST_GROUP(I3DirectionTest);

/**
 * check that constructor and destructor work
 */
TEST(const_dest)
{
  I3DirectionPtr ptr(new I3Direction());
}

TEST(default_construction)
{
  I3Direction d;
  ENSURE(std::isnan(d.GetZenith()));
  ENSURE(std::isnan(d.GetAzimuth()));
}

TEST(spherical_construction)
{
  I3Direction d;
  
  double zen=0;
  double azi=0;
  d=I3Direction(zen,azi);
  ENSURE(d.GetZenith()==zen);
  ENSURE(d.GetAzimuth()==azi);
  ENSURE_DISTANCE(d.GetX(),0.0,0.0001);
  ENSURE_DISTANCE(d.GetY(),0.0,0.0001);
  ENSURE_DISTANCE(d.GetZ(),-1.0,0.0001);
  
  zen=I3Constants::pi/2;
  azi=3*I3Constants::pi/2;
  d=I3Direction(zen,azi);
  ENSURE(d.GetZenith()==zen);
  ENSURE(d.GetAzimuth()==azi);
  ENSURE_DISTANCE(d.GetX(),0.0,0.0001);
  ENSURE_DISTANCE(d.GetY(),1.0,0.0001);
  ENSURE_DISTANCE(d.GetZ(),0.0,0.0001);
}

TEST(cartesian_construction)
{
  I3Direction d1(0,0,-1);
  ENSURE_DISTANCE(d1.GetX(),0.0,.0001);
  ENSURE_DISTANCE(d1.GetY(),0.0,.0001);
  ENSURE_DISTANCE(d1.GetZ(),-1.0,.0001);
  ENSURE_DISTANCE(d1.GetZenith(),0,0.0001);
  
  I3Direction d2(0,0,1);
  ENSURE_DISTANCE(d2.GetX(),0.0,.0001);
  ENSURE_DISTANCE(d2.GetY(),0.0,.0001);
  ENSURE_DISTANCE(d2.GetZ(),1.0,.0001);
  ENSURE_DISTANCE(d2.GetZenith(),I3Constants::pi,0.0001);
  
  I3Direction d3(1,0,0);
  ENSURE_DISTANCE(d3.GetX(),1.0,.0001);
  ENSURE_DISTANCE(d3.GetY(),0.0,.0001);
  ENSURE_DISTANCE(d3.GetZ(),0.0,.0001);
  ENSURE_DISTANCE(d3.GetZenith(),I3Constants::pi/2,0.0001);
  ENSURE_DISTANCE(d3.GetAzimuth(),I3Constants::pi,0.0001);
  
  I3Direction d4(0,1,0);
  ENSURE_DISTANCE(d4.GetX(),0.0,.0001);
  ENSURE_DISTANCE(d4.GetY(),1.0,.0001);
  ENSURE_DISTANCE(d4.GetZ(),0.0,.0001);
  ENSURE_DISTANCE(d4.GetZenith(),I3Constants::pi/2,0.0001);
  ENSURE_DISTANCE(d4.GetAzimuth(),3*I3Constants::pi/2,0.0001);
}

TEST(i3position_construction)
{
  I3Position p(1.1,2.2,3.3);
  double norm=p.Magnitude();
  I3Direction d(p);
  ENSURE_DISTANCE(d.GetX(),p.GetX()/norm,.0001);
  ENSURE_DISTANCE(d.GetY(),p.GetY()/norm,.0001);
  ENSURE_DISTANCE(d.GetZ(),p.GetZ()/norm,.0001);
}

/**
 * Make sure that the changing of coordinate systems works well
 */
TEST(coord_change)
{
  cout <<"Creating I3Direction..."<<endl;
  I3Direction dir;
  cout <<"Setting dir: zen=(180-54.74)deg, azi=-135deg (i.e.1,1,1)..."<<endl;
  dir = I3Direction((180-54.73561)*deg, -135*deg);
  ENSURE_DISTANCE(dir.GetZenith()/deg,125.264,0.001,"dir.GetZenith failed");
  ENSURE_DISTANCE(dir.GetAzimuth()/deg,-135.0,0.001,"dir.GetAzimuth failed");
  ENSURE_DISTANCE(dir.GetX(),0.57735,0.0001,"dir.GetX failed");
  ENSURE_DISTANCE(dir.GetY(),0.57735,0.0001,"dir.GetY failed");
  ENSURE_DISTANCE(dir.GetZ(),0.57735,0.0001,"dir.GetZ failed");
  
  cout <<"Creating another I3Direction..."<<endl;
  I3Direction dir2;
  dir2 = I3Direction(1.0,0.0,1.0);  // Not properly normalized
  //Recall Azimuth/Zenith are the direction back to sourcethat originates a particle
  //  with this direction.
  ENSURE_DISTANCE(dir2.GetZenith()/deg,(180.0-45.0),0.001,"dir.GetZenith failed");
  ENSURE_DISTANCE(dir2.GetAzimuth()/deg,180.0,0.001,"dir.GetAzimuth failed");
  ENSURE_DISTANCE(dir2.CalcTheta()/deg,(45.0),0.001,"dir.CalcTheta failed");
  ENSURE_DISTANCE(dir2.CalcPhi()/deg,0.0,0.001,"dir.CalcPhi failed");
  //Make sure the GetX, etc are properly renormalized.
  ENSURE_DISTANCE(dir2.GetX(),0.7071,0.0001,"dir.GetX failed");
  ENSURE_DISTANCE(dir2.GetY(),0.,0.0001,"dir.GetY failed");
  ENSURE_DISTANCE(dir2.GetZ(),0.7071,0.0001,"dir.GetZ failed");
  //Now change the direction, make sure everthing is updated properly
  dir2 = I3Direction(1.0,1.0,1.0);  // Not properly normalized
  ENSURE_DISTANCE(dir2.GetZenith()/deg,(180.0-54.736),0.001,"dir.GetZenith failed");
  ENSURE_DISTANCE(dir2.GetAzimuth()/deg,(225.0),0.001,"dir.GetAzimuth failed");
  ENSURE_DISTANCE(dir2.CalcTheta()/deg,(54.736),0.001,"dir.CalcTheta failed");
  ENSURE_DISTANCE(dir2.CalcPhi()/deg,45.0,0.001,"dir.GetPhi failed");
  //Make sure the GetX, etc are properly renormalized.
  ENSURE_DISTANCE(dir2.GetX(),0.57735,0.0001,"dir.GetX failed");
  ENSURE_DISTANCE(dir2.GetY(),0.57735,0.0001,"dir.GetY failed");
  ENSURE_DISTANCE(dir2.GetZ(),0.57735,0.0001,"dir.GetZ failed");

  cout <<"Setting dir: x=1,y=1,z=0..."<<endl;
  dir = I3Direction(1,1,0);
  ENSURE_DISTANCE(dir.GetZenith()/deg,90.0,0.001,"dir.GetZenith failed");
  ENSURE_DISTANCE(dir.GetAzimuth()/deg,225.0,0.001,"dir.GetAzimuth failed");
  ENSURE_DISTANCE(dir.GetX(),0.7071,0.001,"dir.X failed");
  ENSURE_DISTANCE(dir.GetY(),0.7071,0.001,"dir.Y failed");
  ENSURE_DISTANCE(dir.GetZ(),0.0,0.001,"dir.Z failed");

  cout <<"Setting values for dir in zen=0,azi=0 (i.e. down)..."<<endl;
  dir = I3Direction(0,0);
  ENSURE_DISTANCE(dir.GetX(),0.0,0.0001,"dir.GetX failed");
  ENSURE_DISTANCE(dir.GetY(),0.0,0.0001,"dir.GetY failed");
  ENSURE_DISTANCE(dir.GetZ(),-1.0,0.0001,"dir.GetZ failed");
  
  cout <<"Rotating dir around x-axis by +90deg... ==> +y-axis"<<endl;
  dir.RotateX(90*deg);
  ENSURE_DISTANCE(dir.GetZenith()/deg,90.0,0.001,"dir.GetZenith failed");
  ENSURE_DISTANCE(dir.GetAzimuth()/deg,270.0,0.001,"dir.GetAzimuth failed");
  ENSURE_DISTANCE(dir.GetX(),0.0,0.0001,"dir.GetX failed");
  ENSURE_DISTANCE(dir.GetY(),1.0,0.0001,"dir.GetY failed");
  ENSURE_DISTANCE(dir.GetZ(),0.0,0.0001,"dir.GetZ failed");
  
  cout <<"Rotating dir around z-axis by -45deg...==> x=1,y=1,z=0"<<endl;
  dir.RotateZ(-45*deg);
  ENSURE_DISTANCE(dir.GetZenith()/deg,90.0,0.001,"dir.GetZenith failed");
  ENSURE_DISTANCE(dir.GetAzimuth()/deg,225.0,0.001,"dir.GetAzimuth failed");
  ENSURE_DISTANCE(dir.GetX(),0.707107,0.0001,"dir.GetX failed");
  ENSURE_DISTANCE(dir.GetY(),0.707107,0.0001,"dir.GetY failed");
  ENSURE_DISTANCE(dir.GetZ(),0.0,0.0001,"dir.GetZ failed");
  
  cout <<"Rotating dir around y-axis by -90deg... ==> x=0,y=1,z=1"<<endl;
  dir.RotateY(-90*deg);
  ENSURE_DISTANCE(dir.GetZenith()/deg,135.0,0.001,"dir.GetZenith failed");
  ENSURE_DISTANCE(dir.GetAzimuth()/deg,270.0,0.001,"dir.GetAzimuth failed");
  ENSURE_DISTANCE(dir.GetX(),0.0,0.0001,"dir.GetX failed");
  ENSURE_DISTANCE(dir.GetY(),0.707107,0.0001,"dir.GetY failed");
  ENSURE_DISTANCE(dir.GetZ(),0.707107,0.0001,"dir.GetZ failed");
  
}

TEST(setdir)
{
  I3Direction d(90*deg,-90*deg);
  ENSURE_DISTANCE(d.GetX(),0,0.0001);
  ENSURE_DISTANCE(d.GetY(),1,0.0001);
  ENSURE_DISTANCE(d.GetZ(),0,0.0001);
  ENSURE_DISTANCE(d.GetZenith(),90*deg,0.0001);
  ENSURE_DISTANCE(d.GetAzimuth(),-90*deg,0.0001);

  d = I3Direction(1,1,1);
  ENSURE_DISTANCE(d.GetX(),1./sqrt(3.),0.0001);
  ENSURE_DISTANCE(d.GetY(),1./sqrt(3.),0.0001);
  ENSURE_DISTANCE(d.GetZ(),1./sqrt(3.),0.0001);
  ENSURE_DISTANCE(d.GetZenith(),2.18627,0.0001);
  ENSURE_DISTANCE(d.GetAzimuth(),225*deg,0.0001);

  d = I3Direction(2.18627,-135*deg);
  ENSURE_DISTANCE(d.GetX(),1./sqrt(3.),0.0001);
  ENSURE_DISTANCE(d.GetY(),1./sqrt(3.),0.0001);
  ENSURE_DISTANCE(d.GetZ(),1./sqrt(3.),0.0001);
  ENSURE_DISTANCE(d.GetZenith(),2.18627,0.0001);
  ENSURE_DISTANCE(d.GetAzimuth(),-135*deg,0.0001);
}

TEST(inversion)
{
  I3Direction d1(1,2,3);
  I3Direction d2=-d1;
  ENSURE_DISTANCE(-d1.GetX(),d2.GetX(),1e-4);
  ENSURE_DISTANCE(-d1.GetY(),d2.GetY(),1e-4);
  ENSURE_DISTANCE(-d1.GetZ(),d2.GetZ(),1e-4);
}

TEST(cross_product)
{
  //cross products between directions
  I3Direction d1(1,0,0);
  I3Direction d2(0,2,0);
  I3Direction d3=d1.Cross(d2);
  ENSURE_DISTANCE(d3.GetX(),0.0,0.00001);
  ENSURE_DISTANCE(d3.GetY(),0.0,0.00001);
  ENSURE_DISTANCE(d3.GetZ(),1.0,0.00001);
  d3=d2.Cross(d1);
  ENSURE_DISTANCE(d3.GetX(),0.0,0.00001);
  ENSURE_DISTANCE(d3.GetY(),0.0,0.00001);
  ENSURE_DISTANCE(d3.GetZ(),-1.0,0.00001);
  
  d1=I3Direction(0,3,0);
  d2=I3Direction(0,0,4);
  d3=d1.Cross(d2);
  ENSURE_DISTANCE(d3.GetX(),1.0,0.00001);
  ENSURE_DISTANCE(d3.GetY(),0.0,0.00001);
  ENSURE_DISTANCE(d3.GetZ(),0.0,0.00001);
  d3=d2.Cross(d1);
  ENSURE_DISTANCE(d3.GetX(),-1.0,0.00001);
  ENSURE_DISTANCE(d3.GetY(),0.0,0.00001);
  ENSURE_DISTANCE(d3.GetZ(),0.0,0.00001);
  
  d1=I3Direction(0,0,5);
  d2=I3Direction(6,0,0);
  d3=d1.Cross(d2);
  ENSURE_DISTANCE(d3.GetX(),0.0,0.00001);
  ENSURE_DISTANCE(d3.GetY(),1.0,0.00001);
  ENSURE_DISTANCE(d3.GetZ(),0.0,0.00001);
  d3=d2.Cross(d1);
  ENSURE_DISTANCE(d3.GetX(),0.0,0.00001);
  ENSURE_DISTANCE(d3.GetY(),-1.0,0.00001);
  ENSURE_DISTANCE(d3.GetZ(),0.0,0.00001);
  
  //----
  //cross products between a direction and a position
  
  d1=I3Direction(1,0,0);
  I3Position p2(0,2,0);
  I3Position p3=d1.Cross(p2);
  ENSURE_DISTANCE(p3.GetX(),0.0,0.00001);
  ENSURE_DISTANCE(p3.GetY(),0.0,0.00001);
  ENSURE_DISTANCE(p3.GetZ(),2.0,0.00001);
  p3=p2.Cross(d1);
  ENSURE_DISTANCE(p3.GetX(),0.0,0.00001);
  ENSURE_DISTANCE(p3.GetY(),0.0,0.00001);
  ENSURE_DISTANCE(p3.GetZ(),-2.0,0.00001);
  
  d1=I3Direction(0,3,0);
  p2=I3Position(0,0,4);
  p3=d1.Cross(p2);
  ENSURE_DISTANCE(p3.GetX(),4.0,0.00001);
  ENSURE_DISTANCE(p3.GetY(),0.0,0.00001);
  ENSURE_DISTANCE(p3.GetZ(),0.0,0.00001);
  p3=p2.Cross(d1);
  ENSURE_DISTANCE(p3.GetX(),-4.0,0.00001);
  ENSURE_DISTANCE(p3.GetY(),0.0,0.00001);
  ENSURE_DISTANCE(p3.GetZ(),0.0,0.00001);
  
  d1=I3Direction(0,0,5);
  p2=I3Position(6,0,0);
  p3=d1.Cross(p2);
  ENSURE_DISTANCE(p3.GetX(),0.0,0.00001);
  ENSURE_DISTANCE(p3.GetY(),6.0,0.00001);
  ENSURE_DISTANCE(p3.GetZ(),0.0,0.00001);
  p3=p2.Cross(d1);
  ENSURE_DISTANCE(p3.GetX(),0.0,0.00001);
  ENSURE_DISTANCE(p3.GetY(),-6.0,0.00001);
  ENSURE_DISTANCE(p3.GetZ(),0.0,0.00001);
}

TEST(dot_product)
{
  I3Direction d(1,2,3);
  I3Direction f(0,1,2);
  double z = d*f;

  ENSURE((z < 0.956183) && (z > 0.9561828));
  
  //dot products between directions
  I3Direction d1(1,0,0);
  I3Direction d2(0,2,0);
  z=d1*d2;
  ENSURE_DISTANCE(z,0.0,0.00001);
  
  d1=I3Direction(0,0,1);
  d2=I3Direction(0,0,-2);
  z=d1*d2;
  ENSURE_DISTANCE(z,-1.0,0.00001);
  
  d1=I3Direction(0,1,0);
  d2=I3Direction(2,2,0);
  z=d1*d2;
  ENSURE_DISTANCE(z,1/sqrt(2),0.00001);
  
  //----
  //dot products between a direction and a position
  d1=I3Direction(1,0,0);
  I3Position p2(0,2,0);
  z=d1*p2;
  ENSURE_DISTANCE(z,0.0,0.00001);
  
  d1=I3Direction(0,0,1);
  p2=I3Position(0,0,-2);
  z=d1*p2;
  ENSURE_DISTANCE(z,-2.0,0.00001);
  
  d1=I3Direction(0,1,0);
  p2=I3Position(2,2,0);
  z=d1*p2;
  ENSURE_DISTANCE(z,2,0.00001);
}

TEST(comparison)
{
  I3Direction d1(0.5,1.5);
  I3Direction d2(1.5,2.5);
  
  I3Direction d3(d1);
  
  ENSURE(d3==d1);
  ENSURE(!(d3==d2));
  ENSURE(!(d3!=d1));
  ENSURE(d3!=d2);
}

TEST(scalar_multiplication)
{
  I3Direction d(1,-1,1);
  I3Position p=2*d;
  ENSURE_DISTANCE(p.Magnitude(),2,.0001);
  ENSURE_DISTANCE(p.GetX(),2/sqrt(3),.0001);
  ENSURE_DISTANCE(p.GetY(),-2/sqrt(3),.0001);
  ENSURE_DISTANCE(p.GetZ(),2/sqrt(3),.0001);
  
  p=d*5;
  ENSURE_DISTANCE(p.Magnitude(),5,.0001);
  ENSURE_DISTANCE(p.GetX(),5/sqrt(3),.0001);
  ENSURE_DISTANCE(p.GetY(),-5/sqrt(3),.0001);
  ENSURE_DISTANCE(p.GetZ(),5/sqrt(3),.0001);
  
  p=d/2;
  ENSURE_DISTANCE(p.Magnitude(),.5,.0001);
  ENSURE_DISTANCE(p.GetX(),.5/sqrt(3),.0001);
  ENSURE_DISTANCE(p.GetY(),-.5/sqrt(3),.0001);
  ENSURE_DISTANCE(p.GetZ(),.5/sqrt(3),.0001);
}

TEST(angle_computation)
{
  I3Direction a(1,-1,0);
  ENSURE_DISTANCE(a.Angle(a), 0., .0001);
  ENSURE_DISTANCE(a.Angle(I3Direction(-1,1,0)), M_PI, .0001);
}