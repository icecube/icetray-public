#include <I3Test.h>

#include "phys-services/I3Cuts.h"
#include "dataclasses/physics/I3Particle.h"

#include <algorithm>
#include <boost/foreach.hpp>
#include <iostream>

TEST_GROUP(ContainmentSizeTest);


// some functions we need
std::vector<double > myXPos ();
std::vector<double > myYPos ();


TEST(ContainmentAreaSize){
  // make a first linebreak
  std::cout << std::endl;

  // define a simple detector --------------------
  const std::vector<double > x (myXPos ());
  const std::vector<double > y (myYPos ());

  // print the cornerpoints
  std::cout << "x: " << std::endl; 
  BOOST_FOREACH(const double &val, x) {
            std::cout << val << ", ";
  }
  std::cout << std::endl;

  std::cout << "y: " << std::endl; 
  BOOST_FOREACH(const double &val, y) {
            std::cout << val << ", ";
  }
  std::cout << std::endl;


  // define the particle -------------------------
  const double px = 450;
  const double py = 0;
  const double detectorPlane = 0;

  I3Particle p (I3Particle::InfiniteTrack);

  p.SetPos (px, py, detectorPlane);

  p.SetDir (0, 0); // vertical track

  std::cout << "particle: (x, y) = (" 
	    << px << ", " << py << ")" << std::endl;

  // calculate the scaling factor ----------------
  const double s 
    = I3Cuts::ContainmentAreaSize (p, x, y, detectorPlane);


  std::cout << "scale factor: " << s << std::endl;
  ENSURE (s < 1, "This should be in the detector.");
}


TEST(ContainmentVolumeSize){
  // make a first linebreak
  std::cout << std::endl;

  // define a simple detector --------------------
  const std::vector<double > x (myXPos ());
  const std::vector<double > y (myYPos ());

  // print the cornerpoints
  std::cout << "x: " << std::endl; 
  BOOST_FOREACH(const double &val, x) {
            std::cout << val << ", ";
  }
  std::cout << std::endl;

  std::cout << "y: " << std::endl; 
  BOOST_FOREACH(const double &val, y) {
            std::cout << val << ", ";
  }
  std::cout << std::endl;


  // define the particle -------------------------
  const double px = 450;
  const double py = 0;
  const double detectorTop = 500;
  const double detectorBot = -500;
  const double detectorMid = (detectorTop + detectorBot)/2;

  I3Particle p (I3Particle::InfiniteTrack);

  p.SetPos (px, py, detectorMid);
  p.SetDir (0, 0); // vertical track

  std::cout << "particle: (x, y) = (" 
	    << px << ", " << py << ")" << std::endl;

  // calculate the scaling factor ----------------
  const double s 
    = I3Cuts::ContainmentVolumeSize (p, x, y, 
				     detectorTop, detectorBot);

  std::cout << "scale factor: " << s << std::endl;
  ENSURE (s < 1, "This should be in the detector.");
}


// x-xoordnates for a simplified ic-40 geo
std::vector<double > myXPos () {
  std::vector<double > x;
  x.push_back( 400);
  x.push_back( 550);
  //  x.push_back( 350);
  //  x.push_back( 200);
  x.push_back(   0);
  x.push_back(-350);
  x.push_back(-400);
  //  x.push_back(-380);
  //  x.push_back(-220);
  x.push_back(-200);
  //  x.push_back(  50);
  x.push_back( 300);
  
  return x;
}

// y-xoordnates for a simplified ic-40 geo
std::vector<double > myYPos () {
  std::vector<double > y;
  y.push_back(-200);
  y.push_back( 150);
  //  y.push_back( 400);
  //  y.push_back( 400);
  y.push_back( 500);
  y.push_back( 450);
  y.push_back( 200);
  //  y.push_back( 100);
  //  y.push_back( 100);
  y.push_back(  50);
  //  y.push_back( 100);
  y.push_back(   0);
  
  return y;
}


