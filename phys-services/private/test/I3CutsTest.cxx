#include <I3Test.h>

#include "phys-services/I3Calculator.h"
#include "phys-services/I3Cuts.h"
#include "dataclasses/I3Position.h"
#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/physics/I3RecoPulse.h"
#include "dataclasses/geometry/I3Geometry.h"

using I3Calculator::CherenkovTime;
using I3Cuts::ContainmentVolumeSize;
using I3Cuts::CylinderSize;
using I3Cuts::ContainmentAreaSize;
using I3Cuts::CMPolygon;
using I3Cuts::PutPointsInOrder;
using I3Cuts::IntersectionOfLineAndPlane;
using I3Cuts::SmoothDir;
using I3Cuts::SmoothAll;
using I3Cuts::Ldir;
using I3Cuts::Ndir;
using I3Constants::theta_cherenkov;
using I3Constants::c_ice;
using I3Constants::c;

I3Geometry CalcGeom(std::vector<I3Position> pos)
{
  I3Geometry geometry;
  for (unsigned int i=0; i<pos.size(); i++) {
    OMKey om(1,i);
    I3OMGeo geo;
    geo.position = pos[i];
    geometry.omgeo[om] = geo;
  }
  return geometry;
}

I3RecoPulseSeriesMap CalcPulses(std::vector<double> time)
{
  I3RecoPulseSeriesMap hitsmap;
  for (unsigned int i=0; i<time.size(); i++) {
    OMKey om(1,i);
    I3RecoPulseSeries hits;
    I3RecoPulse h;
    h.SetTime(time[i]);
    h.SetCharge(1);
    hits.push_back(h);
    hitsmap[om] = hits;
  }
  return hitsmap;
}


TEST_GROUP(I3Cuts)

TEST(FakeTrack)
{
  I3Particle track(I3Particle::InfiniteTrack);
  track.SetPos(0,0,0,I3Position::car);
  track.SetDir(0,0);
  double ang = 180*I3Units::deg-theta_cherenkov;
  I3Position aa(10,ang,45*I3Units::deg,I3Position::sph);
  I3Position bb(5,ang,-30*I3Units::deg,I3Position::sph);
  I3Position cc(20,ang,45*I3Units::deg,I3Position::sph); cc.SetZ(cc.GetZ()-100);
  I3Position dd(50,ang,180*I3Units::deg,I3Position::sph); dd.SetZ(dd.GetZ()+50);
  double A = 1./(c_ice*sin(theta_cherenkov));
  double ta = sqrt(pow(aa.GetX(),2)+pow(aa.GetY(),2))*A + 7*I3Units::ns;
  double tb = sqrt(pow(bb.GetX(),2)+pow(bb.GetY(),2))*A - 9*I3Units::ns;
  double tc = 100/c + sqrt(pow(cc.GetX(),2)+pow(cc.GetY(),2))*A + 5*I3Units::ns;
  double td = -50/c + sqrt(pow(dd.GetX(),2)+pow(dd.GetY(),2))*A - 3*I3Units::ns;
  ENSURE_DISTANCE(CherenkovTime(track,aa)+7,ta,0.0001);
  ENSURE_DISTANCE(CherenkovTime(track,bb)-9,tb,0.0001);
  ENSURE_DISTANCE(CherenkovTime(track,cc)+5,tc,0.0001);
  ENSURE_DISTANCE(CherenkovTime(track,dd)-3,td,0.0001);
}

TEST(Ndir_DownTrack)
{
  I3Particle track(I3Particle::InfiniteTrack);
  track.SetPos(0,0,0,I3Position::car);
  track.SetDir(0,0);
  track.SetTime(103.5);

  int size = 6;
  double r[] =   {10,   5, 20,   7,   10,  15};
  double phi[] = {45, -30, 15, 134,  100, -15};
  double z[] =   { 0,   0,  0, 100, -250, -50};
  double off[] = {17, -19,  5,  -3,   78,  50};

  double ang = 180*I3Units::deg-theta_cherenkov;
  std::vector<I3Position> pos;
  double A = 1./(c_ice*sin(theta_cherenkov));
  std::vector<double> time;
  for (int i=0; i<size; i++) {
    I3Position p(r[i],ang,phi[i]*I3Units::deg,I3Position::sph); p.SetZ(p.GetZ()-z[i]);
    pos.push_back(p);
    double t = (z[i]/c + sqrt(pow(pos[i].GetX(),2)+pow(pos[i].GetY(),2))*A
		+ track.GetTime() + off[i]*I3Units::ns);
    time.push_back(t);
    ENSURE_DISTANCE(CherenkovTime(track,pos[i])+off[i],
		    time[i]-track.GetTime(), 0.0001,
		    "The time of this hit was somehow calculated wrong.");
  }

  I3Geometry geometry = CalcGeom(pos);
  I3RecoPulseSeriesMap hitsmap = CalcPulses(time);

  double ndir;
  ndir = Ndir(track,geometry,hitsmap,-15.,25.);
  ENSURE_DISTANCE(ndir,3.,0.0001,
		  "Wrong number of direct hits calculated.");

  ndir = Ndir(track,geometry,hitsmap,-15.,15.);
  ENSURE_DISTANCE(ndir,2.,0.0001,
		  "Wrong number of direct hits calculated.");

  ndir = Ndir(track,geometry,hitsmap,-15.,75.);
  ENSURE_DISTANCE(ndir,4.,0.0001,
		  "Wrong number of direct hits calculated.");

  ndir = Ndir(track,geometry,hitsmap,-15.,150.);
  ENSURE_DISTANCE(ndir,5.,0.0001,
		  "Wrong number of direct hits calculated.");
}

TEST(AllCuts_TiltedTrack)
{
  double ang = theta_cherenkov; // Cherenkov angle

  I3Particle track(I3Particle::InfiniteTrack);
  track.SetPos(10,10,0,I3Position::car);
  track.SetDir(180*I3Units::deg-ang,90*I3Units::deg);
  track.SetTime(15);

  std::vector<I3Position> pos;
  pos.push_back(I3Position(10,10,20));
  pos.push_back(I3Position(10,0,100));
  pos.push_back(I3Position(10,50,0));

  std::vector<double> time;
  double t = 20/c_ice + 15 + 17;  time.push_back(t);
  t = (10/sin(ang) + (100-10/tan(ang)))/c_ice + 15 - 9;  time.push_back(t);
  t = 40*(1/(c_ice*tan(ang)) - 1/(c*sin(ang))) + 15 + 3;  time.push_back(t);

  std::string hitseries = "NdirTest";
  I3Geometry geometry = CalcGeom(pos);
  I3RecoPulseSeriesMap hitsmap = CalcPulses(time);

  double ndir = Ndir(track,geometry,hitsmap,-15.,25.);
  ENSURE_DISTANCE(ndir,3.,0.0001,
		  "Wrong number of direct hits calculated.");

  double ldir = Ldir(track,geometry,hitsmap,-15.,15.);
  ENSURE_DISTANCE(ldir,108.40697,0.001,
		  "Wrong Ldir distance calculated.");

  double smooth = SmoothAll(track,geometry,hitsmap);
  ENSURE_DISTANCE(smooth,0.119454,0.0001,
		 "Wrong smoothness calculated.");

  double smoothdir = SmoothDir(track,geometry,hitsmap);
  ENSURE_DISTANCE(smoothdir,0.119454,0.0001,
		 "Wrong smoothnessDir calculated.");
}

TEST(CylinderSize)
{
  // AMANDA numbers
  double H0 = 175;
  double R0 = 100;
  double center = 45;
  //const I3Position detectorcenter(0,0,center);

  // Circle, centered on zero
  double centerx = 0;
  double centery = 0;
  std::vector<double> xcir;
  std::vector<double> ycir;
  int npoints = 500;
  for (int i=0; i<npoints; i++) {
    double th = 360*I3Units::deg/(npoints+1)*i;
    xcir.push_back(centerx + R0*sin(th));
    ycir.push_back(centery + R0*cos(th));
  }

  double cylsiz, cylsiz2;
  I3Particle t(I3Particle::InfiniteTrack);

  // Check the "cylindersize" parameter
  // -----------------------------------
  // Corner-clipper:
  t.SetPos(100,100,2000,I3Position::car);
  t.SetDir(0.005,0);
  cylsiz = CylinderSize(t, H0, R0, center);
  ENSURE_DISTANCE(cylsiz, 1.339, 0.001, "CylinderSize corner-clipper not working");

  // Check it against the "Volume" method
  cylsiz2 = ContainmentVolumeSize(t, xcir, ycir, center+H0, center-H0);
  ENSURE_DISTANCE(cylsiz2, cylsiz, 0.001, "CylinderSize corner-clipper not working");

  // Edge-grazer:
  t.SetPos(50,1000,1000,I3Position::car);
  t.SetDir(45*I3Units::deg,90*I3Units::deg);
  cylsiz = CylinderSize(t, H0, R0, center);
  ENSURE_DISTANCE(cylsiz, 0.500, 0.001, "CylinderSize edge-grazer not working");

  // Check it against the "Volume" method
  cylsiz2 = ContainmentVolumeSize(t, xcir, ycir, center+H0, center-H0);
  ENSURE_DISTANCE(cylsiz2, cylsiz, 0.001, "CylinderSize edge-grazer not working");

}

TEST(Containment_Volume)
{
  // Olga's simple cube and track
  std::vector<double> x;
  std::vector<double> y;
  x.push_back(1);
  x.push_back(1);
  x.push_back(-1);
  x.push_back(-1);
  y.push_back(1);
  y.push_back(-1);
  y.push_back(-1);
  y.push_back(1);

  double c;
  I3Particle t(I3Particle::InfiniteTrack);

  // A corner-clipper track
  t.SetPos(0,0,2);
  t.SetThetaPhi((180-30)*I3Constants::pi/180, 45*I3Constants::pi/180);
  c = ContainmentVolumeSize(t, x, y, 1, -1);
  ENSURE_DISTANCE(c, 0.579796, 0.00001, "ContainmentVolume corner-clipper cube");

  // A side-clipper track
  t.SetPos(0,2,1);
  t.SetThetaPhi((180-80)*I3Constants::pi/180, -50*I3Constants::pi/180);
  c = ContainmentVolumeSize(t, x, y, 1, -1);
  ENSURE_DISTANCE(c, 0.912511, 0.00001, "ContainmentVolume side-clipper cube");

  /* OUT OF ORDER WILL NOT WORK ANYMORE   
  // The same thing again, but this time with the border points
  // out of order.
  x.clear();
  y.clear();
  x.push_back(1);
  x.push_back(-1);
  x.push_back(1);
  x.push_back(-1);
  y.push_back(1);
  y.push_back(-1);
  y.push_back(-1);
  y.push_back(1);
  
  // A corner-clipper track
  t.SetPos(0,0,2);
  t.SetThetaPhi((180-30)*I3Constants::pi/180, 45*I3Constants::pi/180);
  c = ContainmentVolumeSize(t, x, y, 1, -1);
  ENSURE_DISTANCE(c, 0.579796, 0.00001, "Out of order ContainmentVolume corner-clipper cube");

  // A side-clipper track
  t.SetPos(0,2,1);
  t.SetThetaPhi((180-80)*I3Constants::pi/180, -50*I3Constants::pi/180);
  c = ContainmentVolumeSize(t, x, y, 1, -1);
  ENSURE_DISTANCE(c, 0.912511, 0.00001, "Out of order ContainmentVolume side-clipper cube");
  */
}


TEST(LPIntersection)
{
 
  // Intersection of line and plane?
  // Olga's test cube
  I3Position A(0,0,0);
  I3Position B(1,1,1);
  I3Position C(1,-1,1);
  // Olga's test track
  I3Particle t(I3Particle::InfiniteTrack, I3Particle::unknown);
  t.SetPos(0,0,2);
  t.SetThetaPhi((180-30)*I3Constants::pi/180, 45*I3Constants::pi/180);
  I3Position answer = IntersectionOfLineAndPlane(t,A,B,C);
  ENSURE_DISTANCE(answer.GetX(), 0.579796, 0.00001, "IntersectionX not right");
  ENSURE_DISTANCE(answer.GetY(), 0.579796, 0.00001, "IntersectionY not right");
  ENSURE_DISTANCE(answer.GetZ(), 0.579796, 0.00001, "IntersectionZ not right");
  
}

TEST(CMPolygon)
{
  std::vector<double> x;
  std::vector<double> y;
  double xcm, ycm;

  // Simple case: rectangle
  x.push_back(0);
  x.push_back(4);
  x.push_back(4);
  x.push_back(0);
  y.push_back(0);
  y.push_back(0);
  y.push_back(2);
  y.push_back(2);
  CMPolygon(x,y,&xcm,&ycm);
  ENSURE_DISTANCE(xcm, 2.0, 0.0001, "Rectangle CM is wrong (x)");
  ENSURE_DISTANCE(ycm, 1.0, 0.0001, "Rectangle CM is wrong (y)");

  // Another simple case:
  // Circle, centered on a point other than zero
  double R = 200;
  double centerx = 36;
  double centery = 12;
  std::vector<double> xcir;
  std::vector<double> ycir;
  int npoints = 100;
  for (int i=0; i<npoints; i++) {
    double th = 360*I3Units::deg/npoints*i;
    xcir.push_back(centerx + R*sin(th));
    ycir.push_back(centery + R*cos(th));
  }
  CMPolygon(xcir,ycir,&xcm,&ycm);
  ENSURE_DISTANCE(xcm,36,0.001, "Circle CM is wrong (x)");
  ENSURE_DISTANCE(ycm,12,0.001, "Circle CM is wrong (y)");
  
  // The IT-16 Rhombus
  x.clear();
  y.clear();
  x.push_back(437.33);
  x.push_back(171.48);
  x.push_back(365.71);
  x.push_back(600.45);
  y.push_back(-209.85);
  y.push_back(119.39);
  y.push_back(428.2);
  y.push_back(144.06);
  CMPolygon(x,y,&xcm,&ycm);
  ENSURE_DISTANCE(xcm,392.0216,0.0001, "IT-16 Rhombus CM is wrong (x)");
  ENSURE_DISTANCE(ycm,116.4436,0.0001, "IT-16 Rhombus CM is wrong (y)");

  /* OUT OF ORDER WILL NOT WORK ANYMORE 
  // The IT-16 Rhombus with its points out of order
  // Should be the same answer as above!
  x.clear();
  y.clear();
  x.push_back(437.33);
  x.push_back(365.71);
  x.push_back(171.48);
  x.push_back(600.45);
  y.push_back(-209.85);
  y.push_back(428.2);
  y.push_back(119.39);
  y.push_back(144.06);
  CMPolygon(x,y,&xcm,&ycm);
  ENSURE_DISTANCE(xcm,392.0216,0.0001, "IT-16 out-of-order Rhombus CM is wrong (x)");
  ENSURE_DISTANCE(ycm,116.4436,0.0001, "IT-16 out-of-order Rhombus CM is wrong (y)"); */
}

TEST(IC40bug)
{

  std::vector<double> x;
  std::vector<double> y;
  double xcm, ycm;

  // Sebastian's "simplified IC-40"
  x.clear();
  y.clear();
  x.push_back( 400);
  x.push_back( 550);
  x.push_back(   0);
  x.push_back(-350);
  x.push_back(-400);
  x.push_back(-200);
  x.push_back( 300);

  y.push_back(-200);
  y.push_back( 150);
  y.push_back( 500);
  y.push_back( 450);
  y.push_back( 200);
  y.push_back(  50);
  y.push_back(   0);
  CMPolygon(x,y,&xcm,&ycm);
  // NEW
  ENSURE_DISTANCE(xcm,56.0494,0.0001, "Simplified IC-40 CM is wrong (x)");
  ENSURE_DISTANCE(ycm,209.50617,0.0001, "Simplified IC-40 CM is wrong (y)");
  // OLD and buggy
  //ENSURE_DISTANCE(xcm,37.7068,0.0001, "Simplified IC-40 CM is wrong (x)");
  //ENSURE_DISTANCE(ycm,195.39007,0.0001, "Simplified IC-40 CM is wrong (y)");

  // Some ContainmentAreaSize tests 
  double c;
  I3Particle t(I3Particle::InfiniteTrack, I3Particle::unknown);
  t.SetThetaPhi((180-30)*I3Constants::pi/180, 45*I3Constants::pi/180);

  t.SetPos(360,-80,0);
  c = ContainmentAreaSize(t, x, y, 0);
  ENSURE_DISTANCE(c, 0.856319, 0.000001, "Simplified IC-40 (point 1) Area didn't work");

  t.SetPos(330,-80,0);
  c = ContainmentAreaSize(t, x, y, 0);
  ENSURE_DISTANCE(c, 1.071840, 0.000001, "Simplified IC-40 (point 2) Area didn't work");

  t.SetPos(330,-80,0);
  c = ContainmentAreaSize(t, x, y, 0);
  ENSURE_DISTANCE(c, 1.071840, 0.000001, "Simplified IC-40 (point 3) Area didn't work");

  t.SetPos(275,-10,0);
  c = ContainmentAreaSize(t, x, y, 0);
  ENSURE_DISTANCE(c, 1.067527, 0.000001, "Simplified IC-40 (point 4) Area didn't work");

  t.SetPos(250,20,0);
  c = ContainmentAreaSize(t, x, y, 0);
  ENSURE_DISTANCE(c, 0.918968, 0.000001, "Simplified IC-40 (point 5) Area didn't work");

  t.SetPos(420,-120,0);
  c = ContainmentAreaSize(t, x, y, 0);
  ENSURE_DISTANCE(c, 0.972499, 0.000001, "Simplified IC-40 (point 6) Area didn't work");

  t.SetPos(450,-150,0);
  c = ContainmentAreaSize(t, x, y, 0);
  ENSURE_DISTANCE(c, 1.055003, 0.000001, "Simplified IC-40 (point 7) Area didn't work");




  /*
  // The Real IC-40
  x.clear();
  y.clear();
  //string 21
  x.push_back(443.80);
  y.push_back(-194.53);
  //string 50
  x.push_back(576.37);
  y.push_back(170.92);
  //string 74
  x.push_back(338.44);
  y.push_back(463.72);
  //string 73
  x.push_back(224.58);
  y.push_back(432.35);
  //string 78
  x.push_back(22.11);
  y.push_back(509.5);
  //string 75
  x.push_back(-347.88);
  y.push_back(451.52);
  //string 60
  x.push_back(-437.04);
  y.push_back(217.80);
  //string 52
  x.push_back(-358.44);
  y.push_back(120.56);
  //string 53
  x.push_back(-234.95);
  y.push_back(140.44);
  //string 44
  x.push_back(-156.23);
  y.push_back(43.37);
  //string 46
  x.push_back(90.49);
  y.push_back(82.35);

  // First, the smoothed version with no string 38
  CMPolygon(x,y,&xcm,&ycm);
  ENSURE_DISTANCE(xcm,99.0742,0.0001, "Real IC-40 CM is wrong (x)");
  ENSURE_DISTANCE(ycm,223.76308,0.0001, "Real IC-40 CM is wrong (y)");

  //string 38
  x.push_back(292.90);
  y.push_back(5.20);
  CMPolygon(x,y,&xcm,&ycm);
#if IC40_CONTAINMENT_BUG_HAS_BEEN_FIXED
  //NEW
  ENSURE_DISTANCE(xcm,92.4584,0.0001, "Real IC-40 CM is wrong (x)");
  ENSURE_DISTANCE(ycm,233.4785,0.0001, "Real IC-40 CM is wrong (y)");
#else
  //OLD
  ENSURE_DISTANCE(xcm,60.4606,0.0001, "Real IC-40 CM is wrong (x)");
  ENSURE_DISTANCE(ycm,249.9794,0.0001, "Real IC-40 CM is wrong (y)");
#endif
  */
}


TEST(Containment_Square)
{

  // Top square from Olga's test cube
  std::vector<double> x;  
  x.push_back(1);
  x.push_back(1);
  x.push_back(-1);
  x.push_back(-1);
  std::vector<double> y;  
  y.push_back(1);
  y.push_back(-1);
  y.push_back(-1);
  y.push_back(1);
  double z=0.9520;  // this is a problematic z for some reason
  // Update: this function now REQUIRES the points to be in counterclockwise order.
  // I need to rearrange them.  
  PutPointsInOrder(&x,&y,0,0,0);

  // Olga's test track
  I3Particle t(I3Particle::InfiniteTrack, I3Particle::unknown);
  t.SetPos(0,0,2);
  t.SetThetaPhi((180-30)*I3Constants::pi/180, 45*I3Constants::pi/180);
  double c;

  c = ContainmentAreaSize(t, x, y, z);
  ENSURE_DISTANCE(c, 0.42784, 0.0001, "Square C didn't work");

}


TEST(Containment_Rectangle)
{

  // Olga's first test-case... a rectangle like this:
  //   (0,2)             (4,2)
  //     x   x   x   x   x
  //     x   x   x   x   x
  //     x   x   x   x   x
  //   (0,0)             (4,2)
  // One-dimensional case
  std::vector<double> x;  // = (0,4,4,0);
  x.push_back(0);
  x.push_back(4);
  x.push_back(4);
  x.push_back(0);
  std::vector<double> y;  // = (0,0,2,2);
  y.push_back(0);
  y.push_back(0);
  y.push_back(2);
  y.push_back(2);
  double z=0;
  I3Particle seed;
  seed.SetShape(I3Particle::InfiniteTrack);
  seed.SetFitStatus(I3Particle::OK);
  double c;

  // The "point" is at (1.0,1.5)
  // Actually, just a little bit off so it doesn't land exactly...
  seed.SetPos(1.01, 1.5, 0);
  seed.SetDir(0, 0);
  c = ContainmentAreaSize(seed, x, y, z);
  ENSURE_DISTANCE(c, 0.5, 0.0001, "Rectangle C didn't work");

  // The "point" is at (1.0,1.5)
  // This time make it land exactly (this is a special case)
  seed.SetPos(1.0, 1.5, 0);
  seed.SetDir(0, 0);
  c = ContainmentAreaSize(seed, x, y, z);
  ENSURE_DISTANCE(c, 0.5, 0.0001, "Rectangle C didn't work");

}


TEST(Containment_IceTopRhombus)
{

  // This time, input the four "corners" of the rhombus of IceTop-16
  std::vector<double> x;  // = (0,4,4,0);
  x.push_back(437.33);
  x.push_back(171.48);
  x.push_back(365.71);
  x.push_back(600.45);
  std::vector<double> y;  // = (0,0,2,2);
  y.push_back(-209.85);
  y.push_back(119.39);
  y.push_back(428.2);
  y.push_back(144.06);
  // Update: this function now REQUIRES the points to be in counterclockwise order.
  // I need to rearrange them.  Use approximate cm of (393.74, 120.45)
  PutPointsInOrder(&x,&y,393.74,120.45,0);
  double z=0;
  I3Particle seed;
  seed.SetShape(I3Particle::InfiniteTrack);
  seed.SetFitStatus(I3Particle::OK);
  double c;

  // The "point"
  seed.SetPos(300, 200, 0);
  seed.SetDir(0, 0);
  c = ContainmentAreaSize(seed, x, y, z);
  //ENSURE_DISTANCE(c, 0.64, 0.005, "Rhombus C didn't work");
  ENSURE_DISTANCE(c, 0.6501, 0.0001, "Rhombus C didn't work");

}

TEST(Exactly_Vertical_Track)
{
  // Olga's simple cube: -1 to 1 on all sides
  std::vector<double> x;
  std::vector<double> y;
  x.push_back(1);
  x.push_back(1);
  x.push_back(-1);
  x.push_back(-1);
  y.push_back(1);
  y.push_back(-1);
  y.push_back(-1);
  y.push_back(1);

  double c;
  I3Particle t(I3Particle::InfiniteTrack);

  // Make sure that this "empty" track returns zero 
  t.SetPos(0,0,0);
  t.SetThetaPhi(0, 0);
  c = ContainmentVolumeSize(t, x, y, 1, -1);
  ENSURE_DISTANCE(c, 0.0, 0.00001, "ContainmentVolume zero track");

  // An exactly vertical track, outside 
  t.SetPos(0,2,0);
  t.SetThetaPhi(0, 0);
  c = ContainmentVolumeSize(t, x, y, 1, -1);
  ENSURE_DISTANCE(c, 2.0, 0.00001, "ContainmentVolume vertical track");

  // An exactly horizontal track, outside
  t.SetPos(0,0,2);
  t.SetThetaPhi(I3Constants::pi/2, 0);
  c = ContainmentVolumeSize(t, x, y, 1, -1);
  ENSURE_DISTANCE(c, 2.0, 0.00001, "ContainmentVolume horizontal track");
  
}
