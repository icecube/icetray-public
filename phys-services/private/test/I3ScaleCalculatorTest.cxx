#include <I3Test.h>

#include <phys-services/I3ScaleCalculator.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/physics/I3Particle.h>
#include <phys-services/I3Cuts.h>

#include <algorithm>
#include <boost/foreach.hpp>
#include <iostream>

TEST_GROUP(scaleCalculatorTest);


// some functions we need
std::vector<int > ic40Strings ();
std::vector<double > ic40XPos ();
std::vector<double > ic40YPos ();
const I3Geometry myGeoService ();


TEST(ic40configuration){
  // build up a geometry
  I3GeometryPtr geo (new I3Geometry (myGeoService ()));
  
  // create a IC40 scale module
  I3ScaleCalculator scale (geo, I3ScaleCalculator::IC40, 
			   I3ScaleCalculator::IT_EMPTY);

  // this is what IC40 outer strings should be
  std::vector<int > ic40Outer (ic40Strings ());

  // this is the answer of the scaling class
  std::vector<int > stringNo (scale.GetOuterStrings ());

  // do a output
  std::cout << std::endl;

  std::cout << "IC40 outer strings: ";

  BOOST_FOREACH(const double &val, ic40Outer) {
    std::cout << val << " ";
  }

  std::cout << "IC40 I3Scale outer: " << std::endl; 
  BOOST_FOREACH(const double &val, stringNo) {
    std::cout << val << " ";
  }


  std::cout << std::endl;

  // is the answer right?
  ENSURE ((stringNo.size () == ic40Outer.size ())
	  && equal (stringNo.begin (), stringNo.end (), 
		    ic40Outer.begin()),
	  "Other outer strings from IC expected.");


  // test for the right positions
  std::vector<double > x;
  std::vector<double > y;
  double top, bot;

  scale.CalcOuterStringPositions (x, y, bot, top);

  // top/bot ok?
  ENSURE_EQUAL(bot, -500, "wrong bottom.");
  ENSURE_EQUAL(top, 500, "wrong top.");

  // simple size check
  ENSURE_EQUAL(x.size (), y.size (), 
	       "One coordinate vector too short");
  ENSURE_EQUAL(x.size (), stringNo.size (),
	       "Number of coordinates is not the same"\
	       "as outer strings");

  // x positions right?
  std::vector<double > ic40X (ic40XPos ());
  ENSURE (equal (x.begin (), x.end (), 
		 ic40X.begin ()), 
	  "x-coordinates are wrong.");

  // output
  std::cout << "IC40 x positions: " << std::endl; 
  BOOST_FOREACH(const double &val, ic40X) {
    std::cout << val << " ";
  }

  std::cout << std::endl;

  std::cout << "IC40 I3Scale pos: " << std::endl; 
  BOOST_FOREACH(const double &val, x) {
    std::cout << val << " ";
  }
  
  std::cout << std::endl;
 
  // y position right?
  std::vector<double > ic40Y (ic40YPos ());
  ENSURE (equal (y.begin (), y.end (), 
		 ic40Y.begin ()), 
	  "y-coordinates are wrong.");

  std::cout << "IC40 x positions: " << std::endl; 
  BOOST_FOREACH(const double &val, ic40Y) {
    std::cout << val << " ";
  }

  std::cout << std::endl;

  std::cout << "IC40 I3Scale pos: " << std::endl; 
  BOOST_FOREACH(const double &val, y) {
    std::cout << val << " ";
  }

  std::cout << std::endl;

}

TEST(particleInCscd){

  // build up a geometry
  I3GeometryPtr geo (new I3Geometry (myGeoService ()));
  
  // create a IC40 scale module
  I3ScaleCalculator scale (geo, I3ScaleCalculator::IC40, 
			   I3ScaleCalculator::IT_EMPTY);

  I3Particle p (I3Particle::Cascade);
  p.SetPos (0, 200, 0);
  // the following point gets a wrong scale 
  // (should be smaller 1, but is not) due to a
  // bug in the I3Cuts! 
  //  p=I3Position(450, 0, 0); // favorite poit ;-)
  
  double s = scale.ScaleInIce (p);

  ENSURE (s < 1, "This should be in the detector.");

}

TEST(contained){
  // build up a geometry
  I3GeometryPtr geo (new I3Geometry (myGeoService ()));
  
  // create a IC40 scale module
  I3ScaleCalculator scale (geo, I3ScaleCalculator::IC40, 
			   I3ScaleCalculator::IT_EMPTY);

  I3Particle p (I3Particle::Cascade);
  p.SetPos (450, 0, 0);
  
  ENSURE (scale.VertexIsInside (p), "This should be in the detector.");

}
TEST(particleInMuon){

  // build up a geometry
  I3GeometryPtr geo (new I3Geometry (myGeoService ()));
  
  // create a IC40 scale module
  I3ScaleCalculator scale (geo, I3ScaleCalculator::IC40, 
			   I3ScaleCalculator::IT_EMPTY);

  I3Particle p (I3Particle::InfiniteTrack);
  p.SetPos (0, 200, 0);
  p.SetDir (0, 0);

  double s = scale.ScaleInIce (p);

  ENSURE (s < 1, "This should be in the detector.");

}


std::vector<int > ic40Strings () {

  std::vector<int > ic40Outer;
  ic40Outer.push_back(21);
  ic40Outer.push_back(50);
  ic40Outer.push_back(74);
  ic40Outer.push_back(73);
  ic40Outer.push_back(78);
  ic40Outer.push_back(75);
  ic40Outer.push_back(60);
  ic40Outer.push_back(52);
  ic40Outer.push_back(53);
  ic40Outer.push_back(44);
  ic40Outer.push_back(46);
  ic40Outer.push_back(38);
  
  return ic40Outer;
}

std::vector<double > ic40XPos () {
  std::vector<double > x;
  x.push_back( 400);
  x.push_back( 550);
  x.push_back( 350);
  x.push_back( 200);
  x.push_back(   0);
  x.push_back(-350);
  x.push_back(-400);
  x.push_back(-380);
  x.push_back(-220);
  x.push_back(-200);
  x.push_back(  50);
  x.push_back( 300);
  
  return x;
}

std::vector<double > ic40YPos () {
  std::vector<double > y;
  y.push_back(-200);
  y.push_back( 150);
  y.push_back( 400);
  y.push_back( 400);
  y.push_back( 500);
  y.push_back( 450);
  y.push_back( 200);
  y.push_back( 100);
  y.push_back( 100);
  y.push_back(  50);
  y.push_back( 100);
  y.push_back(   0);
  
  return y;
}

const I3Geometry myGeoService () {

  I3Geometry geo;

  // put the middle strings
    
  geo.omgeo[OMKey (21, 30)].position=I3Position( 400, -200, 0, I3Position::car);
  geo.omgeo[OMKey (50, 30)].position=I3Position( 550,  150, 0, I3Position::car);
  geo.omgeo[OMKey (74, 30)].position=I3Position( 350,  400, 0, I3Position::car);
  geo.omgeo[OMKey (73, 30)].position=I3Position( 200,  400, 0, I3Position::car);
  geo.omgeo[OMKey (78, 30)].position=I3Position(   0,  500, 0, I3Position::car);
  geo.omgeo[OMKey (75, 30)].position=I3Position(-350,  450, 0, I3Position::car);
  geo.omgeo[OMKey (60, 30)].position=I3Position(-400,  200, 0, I3Position::car);
  geo.omgeo[OMKey (52, 30)].position=I3Position(-380,  100, 0, I3Position::car);
  geo.omgeo[OMKey (53, 30)].position=I3Position(-220,  100, 0, I3Position::car);
  geo.omgeo[OMKey (44, 30)].position=I3Position(-200,   50, 0, I3Position::car);
  geo.omgeo[OMKey (46, 30)].position=I3Position(  50,  100, 0, I3Position::car);
  geo.omgeo[OMKey (38, 30)].position=I3Position( 300,    0, 0, I3Position::car);

  geo.omgeo[OMKey (21,  1)].position=I3Position( 400, -200, 500, I3Position::car);
  geo.omgeo[OMKey (50,  1)].position=I3Position( 550,  150, 500, I3Position::car);
  geo.omgeo[OMKey (74,  1)].position=I3Position( 350,  400, 500, I3Position::car);
  geo.omgeo[OMKey (73,  1)].position=I3Position( 200,  400, 500, I3Position::car);
  geo.omgeo[OMKey (78,  1)].position=I3Position(   0,  500, 500, I3Position::car);
  geo.omgeo[OMKey (75,  1)].position=I3Position(-350,  450, 500, I3Position::car);
  geo.omgeo[OMKey (60,  1)].position=I3Position(-400,  200, 500, I3Position::car);
  geo.omgeo[OMKey (52,  1)].position=I3Position(-380,  100, 500, I3Position::car);
  geo.omgeo[OMKey (53,  1)].position=I3Position(-220,  100, 500, I3Position::car);
  geo.omgeo[OMKey (44,  1)].position=I3Position(-200,   50, 500, I3Position::car);
  geo.omgeo[OMKey (46,  1)].position=I3Position(  50,  100, 500, I3Position::car);
  geo.omgeo[OMKey (38,  1)].position=I3Position( 300,    0, 500, I3Position::car);

  geo.omgeo[OMKey (21, 60)].position=I3Position( 400, -200, -500, I3Position::car);
  geo.omgeo[OMKey (50, 60)].position=I3Position( 550,  150, -500, I3Position::car);
  geo.omgeo[OMKey (74, 60)].position=I3Position( 350,  400, -500, I3Position::car);
  geo.omgeo[OMKey (73, 60)].position=I3Position( 200,  400, -500, I3Position::car);
  geo.omgeo[OMKey (78, 60)].position=I3Position(   0,  500, -500, I3Position::car);
  geo.omgeo[OMKey (75, 60)].position=I3Position(-350,  450, -500, I3Position::car);
  geo.omgeo[OMKey (60, 60)].position=I3Position(-400,  200, -500, I3Position::car);
  geo.omgeo[OMKey (52, 60)].position=I3Position(-380,  100, -500, I3Position::car);
  geo.omgeo[OMKey (53, 60)].position=I3Position(-220,  100, -500, I3Position::car);
  geo.omgeo[OMKey (44, 60)].position=I3Position(-200,   50, -500, I3Position::car);
  geo.omgeo[OMKey (46, 60)].position=I3Position(  50,  100, -500, I3Position::car);
  geo.omgeo[OMKey (38, 60)].position=I3Position( 300,    0, -500, I3Position::car);


  return geo;

}
