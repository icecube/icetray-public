/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$

    @version $Revision$
    @date $Date$
    @author pretz

    @todo
*/

#include <I3Test.h>

#include "dataclasses/I3Position.h"
#include "dataclasses/I3Direction.h"
#include "dataclasses/I3Constants.h"
#include "dataclasses/external/CompareFloatingPoint.h"
#include <string>
using std::string;
using std::cout;
using std::endl;

TEST_GROUP(I3PositionTest);

/**
 * check that constructor and destructor work
 */
TEST(constructor)
{
  I3PositionPtr ptr(new I3Position());
}

TEST(default_construction)
{
  I3Position p;
  ENSURE(std::isnan(p.GetX()));
  ENSURE(std::isnan(p.GetY()));
  ENSURE(std::isnan(p.GetZ()));
}

TEST(cartesian_construction)
{
  I3Position p1(0,0,-1);
  ENSURE(p1.GetX()==0.0);
  ENSURE(p1.GetY()==0.0);
  ENSURE(p1.GetZ()==-1.0);
  ENSURE(CompareFloatingPoint::Compare(p1.GetR(),1.0));
  ENSURE(CompareFloatingPoint::Compare(p1.GetTheta(),I3Constants::pi));
  
  I3Position p2(0,0,2);
  ENSURE(p2.GetX()==0.0);
  ENSURE(p2.GetY()==0.0);
  ENSURE(p2.GetZ()==2.0);
  ENSURE(CompareFloatingPoint::Compare(p2.GetR(),2.0));
  ENSURE_DISTANCE(p2.GetTheta(), 0.0, 10*std::numeric_limits<double>::epsilon());
  
  I3Position p3(3,0,0);
  ENSURE(p3.GetX()==3.0);
  ENSURE(p3.GetY()==0.0);
  ENSURE(p3.GetZ()==0.0);
  ENSURE(CompareFloatingPoint::Compare(p3.GetR(),3.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetTheta(),I3Constants::pi/2));
  ENSURE(CompareFloatingPoint::Compare(p3.GetPhi(),0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetRho(),3));
  
  I3Position p4(0,4,0);
  ENSURE(p4.GetX()==0.0);
  ENSURE(p4.GetY()==4.0);
  ENSURE(p4.GetZ()==0.0);
  ENSURE(CompareFloatingPoint::Compare(p4.GetR(),4.0));
  ENSURE(CompareFloatingPoint::Compare(p4.GetTheta(),I3Constants::pi/2));
  ENSURE(CompareFloatingPoint::Compare(p4.GetPhi(),I3Constants::pi/2));
  ENSURE(CompareFloatingPoint::Compare(p4.GetRho(),4));
}

TEST(spherical_construction)
{
  I3Position p1(5,I3Constants::pi/2,I3Constants::pi,I3Position::sph);
  ENSURE(CompareFloatingPoint::Compare(p1.GetR(),5.0));
  ENSURE(CompareFloatingPoint::Compare(p1.GetTheta(),I3Constants::pi/2));
  ENSURE(CompareFloatingPoint::Compare(p1.GetPhi(),I3Constants::pi));
  ENSURE(CompareFloatingPoint::Compare(p1.GetX(),-5.0));
  ENSURE_DISTANCE(p1.GetY(), 0., 10*std::numeric_limits<double>::epsilon());
  ENSURE_DISTANCE(p1.GetZ(), 0., 10*std::numeric_limits<double>::epsilon());
  ENSURE(CompareFloatingPoint::Compare(p1.GetRho(),5));
  
  I3Position p2(3,I3Constants::pi/4,I3Constants::pi/4,I3Position::sph);
  ENSURE(CompareFloatingPoint::Compare(p2.GetR(),3.0));
  ENSURE(CompareFloatingPoint::Compare(p2.GetTheta(),I3Constants::pi/4));
  ENSURE(CompareFloatingPoint::Compare(p2.GetPhi(),I3Constants::pi/4));
  ENSURE(CompareFloatingPoint::Compare(p2.GetX(),1.5));
  ENSURE(CompareFloatingPoint::Compare(p2.GetY(),1.5));
  ENSURE(CompareFloatingPoint::Compare(p2.GetZ(),3/sqrt(2)));
  ENSURE(CompareFloatingPoint::Compare(p2.GetRho(),3/sqrt(2)));
}

TEST(cylindrical_construction)
{
  I3Position p1(3,3*I3Constants::pi/4,4,I3Position::cyl);
  ENSURE(CompareFloatingPoint::Compare(p1.GetRho(),3));
  ENSURE(CompareFloatingPoint::Compare(p1.GetPhi(),3*I3Constants::pi/4));
  ENSURE(CompareFloatingPoint::Compare(p1.GetZ(),4));
  ENSURE(CompareFloatingPoint::Compare(p1.GetX(),-3/sqrt(2)));
  ENSURE(CompareFloatingPoint::Compare(p1.GetY(),3/sqrt(2)));
  ENSURE(CompareFloatingPoint::Compare(p1.GetR(),5));
  ENSURE(CompareFloatingPoint::Compare(p1.GetTheta(),atan(3./4)));
}

TEST(i3direction_construction)
{
  I3Direction d(1.1,2.2,3.3);
  I3Position p(d);
  ENSURE(p.GetX()==d.GetX());
  ENSURE(p.GetY()==d.GetY());
  ENSURE(p.GetZ()==d.GetZ());
}

/**
 * Make sure that the changing of coordinate systems works well
 */
TEST(coord_transforms)
{
  const double TOLERANCE{10.*std::numeric_limits<double>::epsilon()};
  
  I3Position p(4,3,0);
  I3Position q(1,2,3);
  q=I3Position(2,2,2,I3Position::car);
  
  ENSURE(CompareFloatingPoint::Compare(p.GetX(),4.0),"p.GetX failed");
  ENSURE(CompareFloatingPoint::Compare(p.GetY(),3.0),"p.GetY failed");
  ENSURE(CompareFloatingPoint::Compare(p.GetZ(),0.0),"p.GetZ failed");
  ENSURE(CompareFloatingPoint::Compare(p.GetR(),5.0),"p.GetR failed");
  ENSURE(CompareFloatingPoint::Compare(p.GetTheta(),I3Constants::pi/2.),"p.GetTheta failed");
  ENSURE(CompareFloatingPoint::Compare(p.GetPhi(),0.64350110879328438680),"p.GetPhi failed");
  ENSURE(CompareFloatingPoint::Compare(p.GetRho(),5.0),"p.GetRho failed");
  
  ENSURE(CompareFloatingPoint::Compare(q.GetX(),2.0),"q.GetX failed");
  ENSURE(CompareFloatingPoint::Compare(q.GetY(),2.0),"q.GetY failed");
  ENSURE(CompareFloatingPoint::Compare(q.GetZ(),2.0),"q.GetZ failed");
  
  ENSURE(CompareFloatingPoint::Compare((p-q).Magnitude(),3.0),"(p-q).Magnitude() failed");
  ENSURE(CompareFloatingPoint::Compare((q-p).Magnitude(),3.0),"(q-p).Magnitude() failed");
  
  cout <<"Rotating p by Pi/4 around z-axis..."<<endl;
  p.RotateZ(I3Constants::pi/4);
  ENSURE(CompareFloatingPoint::Compare(p.GetX(),1./std::sqrt(2)),"p.GetX failed");
  ENSURE(CompareFloatingPoint::Compare(p.GetY(),7./std::sqrt(2)),"p.GetY failed");
  ENSURE(CompareFloatingPoint::Compare(p.GetZ(),0.0),"p.GetZ failed");
  
  cout <<"Rotating p by Pi/2 around y-axis..."<<endl;
  p.RotateY(I3Constants::pi/2);
  std::cout<<p.GetX()<<", "<<p.GetY()<<", "<<p.GetZ()<<endl;
  ENSURE_DISTANCE(p.GetX(), 0.0, TOLERANCE,"p.GetX failed");
  ENSURE_DISTANCE(p.GetY(), 4.949747468305833, TOLERANCE, "p.GetY failed");
  ENSURE_DISTANCE(p.GetZ(), -0.7071067811865479, TOLERANCE, "p.GetZ failed");
  
  cout <<"Rotating p by Pi/2 around x-axis..."<<endl;
  p.RotateX(I3Constants::pi/2);
  ENSURE_DISTANCE(p.GetX(), 0.0, TOLERANCE, "p.GetX failed");
  ENSURE_DISTANCE(p.GetY(), 0.7071067811865482, TOLERANCE, "p.GetY failed");
  ENSURE_DISTANCE(p.GetZ(), 4.949747468305833, TOLERANCE, "p.GetZ failed");
  
  cout <<"Creating position s and setting coordinates in sph..."<<endl;
  I3Position s;
  s=I3Position(1.732050807568877,
	       0.9553166181245092,
	       I3Constants::pi/4.,
	       I3Position::sph);
  ENSURE_DISTANCE(s.GetX(), 1.0, TOLERANCE, "s.GetX failed");
  ENSURE_DISTANCE(s.GetY(), 1.0, TOLERANCE, "s.GetY failed");
  ENSURE_DISTANCE(s.GetZ(), 1.0, TOLERANCE, "s.GetZ failed");
  
  cout <<"Creating position d and setting coordinates in car..."<<endl;
  I3Position d(1,1,1);
  ENSURE_DISTANCE(d.GetR(), 1.732050807568877, TOLERANCE, "d.GetR failed");
  ENSURE_DISTANCE(d.GetTheta(), 0.9553166181245092, TOLERANCE, "d.GetTheta failed");
  ENSURE_DISTANCE(d.GetPhi(), 0.7853981633974483, TOLERANCE, "d.GetPhi failed");
  
  cout <<"Using the = operator on I3Position e=d..."<<endl;
  I3Position e = d;
  ENSURE_DISTANCE(e.GetR(), 1.732050807568877, TOLERANCE, "e.GetR failed");
  ENSURE_DISTANCE(e.GetTheta() , 0.9553166181245092, TOLERANCE, "e.GetTheta failed");
  ENSURE_DISTANCE(e.GetPhi(), I3Constants::pi/4., TOLERANCE, "e.GetPhi failed");
  
  cout <<"Creating position f from position d..."<<endl;
  I3Position f(d);
  ENSURE(CompareFloatingPoint::Compare(f.GetX(),1.0),"f.GetX failed");
  ENSURE(CompareFloatingPoint::Compare(f.GetY(),1.0),"f.GetY failed");
  ENSURE(CompareFloatingPoint::Compare(f.GetZ(),1.0),"f.GetZ failed");
  
  cout <<"Shifting coordinate system of f by s..."<<endl;
  f -= s;
  ENSURE_DISTANCE(f.GetX(), 0.0, TOLERANCE, "shifted f.GetX failed");
  ENSURE_DISTANCE(f.GetY(), 0.0, TOLERANCE, "shifted f.GetY failed");
  ENSURE_DISTANCE(f.GetZ(), 0.0, TOLERANCE, "shifted f.GetZ failed");
  
  cout <<"Shifting coordinate system of f by p..."<<endl;
  f -= p;
  ENSURE_DISTANCE(f.GetX(), 0.0, TOLERANCE, "shifted f.GetX failed");
  ENSURE_DISTANCE(f.GetY(), -0.7071067811865479, TOLERANCE, "shifted f.GetY failed");
  ENSURE_DISTANCE(f.GetZ(), -4.949747468305833, TOLERANCE, "shifted f.GetZ failed");
  ENSURE_DISTANCE(f.GetR(), 5.0, TOLERANCE, "shifted f.GetR failed");
  ENSURE_DISTANCE(f.GetTheta(), 2.99969559898563, TOLERANCE, "shifted f.GetTheta failed");
  ENSURE_DISTANCE(f.GetPhi(), 4.71238898038469, TOLERANCE, "shifted f.GetPhi failed");
  ENSURE_DISTANCE(f.GetRho(), 0.7071067811865464, TOLERANCE, "shifted f.GetRho failed");
  
}

TEST(magnitude)
{
  I3Position p(3,4,5);
  ENSURE(CompareFloatingPoint::Compare(p.Magnitude(),sqrt(50)));
  ENSURE(CompareFloatingPoint::Compare(p.Mag2(),50));
  
  p=I3Position(12,0,0,I3Position::sph);
  ENSURE(CompareFloatingPoint::Compare(p.Magnitude(),12));
  ENSURE(CompareFloatingPoint::Compare(p.Mag2(),144));

  ENSURE(CompareFloatingPoint::Compare(abs(p),p.Magnitude()));
}

TEST(normalize)
{
  I3Position p(3,4,5);
  p.Normalize();
  ENSURE(p == (I3Position(3,4,5)/sqrt(50.)));
}

TEST(unitvector)
{
  I3Position p(3,4,5);
  ENSURE(p.GetUnitVector() == (p / p.Magnitude()));
}

TEST(inversion)
{
  I3Position p(1,-2,3);
  I3Position q = -p;
  ENSURE(q.GetX()==-p.GetX());
  ENSURE(q.GetY()==-p.GetY());
  ENSURE(q.GetZ()==-p.GetZ());
}

TEST(addition)
{
  double x1=1,y1=2,z1=3;
  double x2=4,y2=5,z2=6;
  
  I3Position p1(x1,y1,z1);
  I3Position p2(x2,y2,z2);
  I3Position p3=p1+p2;
  p1+=p2;
  
  ENSURE(p1.GetX()==x1+x2);
  ENSURE(p3.GetX()==x1+x2);
  ENSURE(p1.GetY()==y1+y2);
  ENSURE(p3.GetY()==y1+y2);
  ENSURE(p1.GetZ()==z1+z2);
  ENSURE(p3.GetZ()==z1+z2);
}

TEST(subtraction)
{
  double x1=1,y1=2,z1=3;
  double x2=4,y2=5,z2=6;
  
  I3Position p1(x1,y1,z1);
  I3Position p2(x2,y2,z2);
  I3Position p3=p1-p2;
  p1-=p2;
  p2-=I3Position(p2);
  
  ENSURE(p1.GetX()==x1-x2);
  ENSURE(p2.GetX()==0);
  ENSURE(p3.GetX()==x1-x2);
  ENSURE(p1.GetY()==y1-y2);
  ENSURE(p2.GetY()==0);
  ENSURE(p3.GetY()==y1-y2);
  ENSURE(p1.GetZ()==z1-z2);
  ENSURE(p2.GetZ()==0);
  ENSURE(p3.GetZ()==z1-z2);
}

TEST(dot_product)
{
  const double TOLERANCE{10.*std::numeric_limits<double>::epsilon()};

  //dot products between positions
  I3Position p1(1,0,0);
  I3Position p2(0,2,0);
  double z=p1*p2;
  ENSURE(CompareFloatingPoint::Compare(z,0.0));
  
  p1=I3Position(0,0,1);
  p2=I3Position(0,0,-2);
  z=p1*p2;
  ENSURE(CompareFloatingPoint::Compare(z,-2.0));
  
  p1=I3Position(0,1,0);
  p2=I3Position(2,2,0);
  z=p1*p2;
  ENSURE(CompareFloatingPoint::Compare(z,2));
  
  //----
  //dot products between a position and a direction
  p1=I3Position(1,0,0);
  I3Direction d2(0,2,0);
  z=p1*d2;
  std::cout<<z<<endl;
  ENSURE_DISTANCE(z, 0.0, TOLERANCE);
  
  p1=I3Position(0,0,1);
  d2=I3Direction(0,0,-2);
  z=p1*d2;
  ENSURE(CompareFloatingPoint::Compare(z,-1.0));
  
  p1=I3Position(0,1,0);
  d2=I3Direction(2,2,0);
  z=p1*d2;
  ENSURE(CompareFloatingPoint::Compare(z,1/sqrt(2)));
}

TEST(cross_product)
{
  const double TOLERANCE{10.*std::numeric_limits<double>::epsilon()};

  //cross products between positions
  I3Position p1(1,0,0);
  I3Position p2(0,2,0);
  I3Position p3=p1.Cross(p2);
  ENSURE(CompareFloatingPoint::Compare(p3.GetX(),0.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetY(),0.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetZ(),2.0));
  p3=p2.Cross(p1);
  ENSURE(CompareFloatingPoint::Compare(p3.GetX(),0.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetY(),0.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetZ(),-2.0));
  
  p1=I3Position(0,3,0);
  p2=I3Position(0,0,4);
  p3=p1.Cross(p2);
  ENSURE(CompareFloatingPoint::Compare(p3.GetX(),12.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetY(),0.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetZ(),0.0));
  p3=p2.Cross(p1);
  ENSURE(CompareFloatingPoint::Compare(p3.GetX(),-12.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetY(),0.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetZ(),0.0));
  
  p1=I3Position(0,0,5);
  p2=I3Position(6,0,0);
  p3=p1.Cross(p2);
  ENSURE(CompareFloatingPoint::Compare(p3.GetX(),0.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetY(),30.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetZ(),0.0));
  p3=p2.Cross(p1);
  ENSURE(CompareFloatingPoint::Compare(p3.GetX(),0.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetY(),-30.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetZ(),0.0));
  
  //----
  //cross products between a position and a direction
  
  p1=I3Position(1,0,0);
  I3Direction d2(0,2,0);
  p3=p1.Cross(d2);
  std::cout<<p3.GetX()<<", "<<p3.GetY()<<", "<<p3.GetZ()<<endl;
  ENSURE(CompareFloatingPoint::Compare(p3.GetX(),0.0));
  ENSURE_DISTANCE(p3.GetY(), 0.0, TOLERANCE);
  ENSURE(CompareFloatingPoint::Compare(p3.GetZ(),1.0));

  p3=d2.Cross(p1);
  ENSURE(CompareFloatingPoint::Compare(p3.GetX(),0.0));
  ENSURE_DISTANCE(p3.GetY(), 0.0, TOLERANCE);
  ENSURE(CompareFloatingPoint::Compare(p3.GetZ(),-1.0));
  
  p1=I3Position(0,3,0);
  d2=I3Direction(0,0,4);
  p3=p1.Cross(d2);
  ENSURE(CompareFloatingPoint::Compare(p3.GetX(),3.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetY(),0.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetZ(),0.0));
  p3=d2.Cross(p1);
  ENSURE(CompareFloatingPoint::Compare(p3.GetX(),-3.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetY(),0.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetZ(),0.0));
  
  p1=I3Position(0,0,5);
  d2=I3Direction(6,0,0);
  p3=p1.Cross(d2);
  ENSURE(CompareFloatingPoint::Compare(p3.GetX(),0.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetY(),5.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetZ(),0.0));
  p3=d2.Cross(p1);
  ENSURE(CompareFloatingPoint::Compare(p3.GetX(),0.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetY(),-5.0));
  ENSURE(CompareFloatingPoint::Compare(p3.GetZ(),0.0));
}

TEST(comparison)
{
  I3Position p(1.1,2.2,3.3);
  I3Position q(2.3,3.4,5.6);
  
  I3Position p2(p);
  
  ENSURE(p2==p);
  ENSURE(!(p2==q));
  ENSURE(!(p2!=p));
  ENSURE(p2!=q);
}

TEST(scalar_multiplication)
{
  I3Position p1(1,-1,1);
  I3Position p2=2*p1;
  ENSURE(CompareFloatingPoint::Compare(p2.Magnitude(),2*sqrt(3)));
  ENSURE(CompareFloatingPoint::Compare(p2.GetX(),2));
  ENSURE(CompareFloatingPoint::Compare(p2.GetY(),-2));
  ENSURE(CompareFloatingPoint::Compare(p2.GetZ(),2));
  
  p2=p1*5;
  ENSURE(CompareFloatingPoint::Compare(p2.Magnitude(),5*sqrt(3)));
  ENSURE(CompareFloatingPoint::Compare(p2.GetX(),5));
  ENSURE(CompareFloatingPoint::Compare(p2.GetY(),-5));
  ENSURE(CompareFloatingPoint::Compare(p2.GetZ(),5));
  
  p2=p1/2;
  ENSURE(CompareFloatingPoint::Compare(p2.Magnitude(),.5*sqrt(3)));
  ENSURE(CompareFloatingPoint::Compare(p2.GetX(),.5));
  ENSURE(CompareFloatingPoint::Compare(p2.GetY(),-.5));
  ENSURE(CompareFloatingPoint::Compare(p2.GetZ(),.5));
  
  p1/=sqrt(3);
  ENSURE(CompareFloatingPoint::Compare(p1.Magnitude(),1));
  ENSURE(CompareFloatingPoint::Compare(p1.GetX(),1/sqrt(3)));
  ENSURE(CompareFloatingPoint::Compare(p1.GetY(),-1/sqrt(3)));
  ENSURE(CompareFloatingPoint::Compare(p1.GetZ(),1/sqrt(3)));
  
  p1*=7;
  ENSURE(CompareFloatingPoint::Compare(p1.Magnitude(),7));
  ENSURE(CompareFloatingPoint::Compare(p1.GetX(),7/sqrt(3)));
  ENSURE(CompareFloatingPoint::Compare(p1.GetY(),-7/sqrt(3)));
  ENSURE(CompareFloatingPoint::Compare(p1.GetZ(),7/sqrt(3)));
}

TEST(angular_separation)
{
  I3Position p1(0, 0, 5);
  I3Position p2(0, 3, 0);
  I3Position p3(0, 0, -1);
  I3Position p4(1.5, 0, 0);

  ENSURE(p1.GetAngularSeparation(p1) == 0.);
  ENSURE(p1.GetAngularSeparation(p2) == M_PI/2.);
  ENSURE(p1.GetAngularSeparation(p3) == M_PI);
  ENSURE(p1.GetAngularSeparation(p4) == M_PI/2.);
}