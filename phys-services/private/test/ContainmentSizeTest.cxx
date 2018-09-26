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

// a code copy for debugging
// (is not in use for the test)
double myScaleFunc (double px, double py,
		    std::vector<double> x,
		    std::vector<double> y);


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


  // do the calculation with own function
  // just for debugging
  // double s = myScaleFunc (px, py, x, y);


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


// the algorithm with some cout for debugging
// this should be removed later
double myScaleFunc (double px, double py,
		    std::vector<double> x,
		    std::vector<double> y)
{

  using namespace I3Cuts;

  // Error-checking... need at least three strings to have an area
  if (x.size () < 3) {
    std::cout << "ContainmentArea of zero/1/2 strings: will be NAN"
	      << std::endl;
    return NAN;
  }

  // INSERT OLGA'S ALGORITHM HERE!
  // First, compute the center of mass
  double xcm, ycm;
  CMPolygon(x, y, &xcm, &ycm);

  std::cout << "center pos: " << xcm 
	    << ", " << ycm << std::endl;

  double xprime = px;
  double yprime = py;

  // Compute the angle of point P:
  I3Direction d(xprime-xcm,yprime-ycm,0);
  double pang = d.CalcPhi();

  std::cout << "pang: " << pang << std::endl;
 
  //  double pang = angle(xcm,xprime,ycm,yprime);
  int n_more = -1;
  double more = 9999;
  int n_less = -1;
  double less = -9999;

  // Now, compute angles for each corner point,
  // and figure out which two bracket the point
  if (x.size() != y.size()) log_fatal("X and Y are not the same size");
  std::vector<double> ang;
  ang.resize(x.size());

  std::cout << "angle calc ------" << std::endl;
  
  for (unsigned int i=0; i<x.size(); i++) {
    I3Direction dd(x[i]-xcm,y[i]-ycm,0);
    ang[i] = dd.CalcPhi();
    //ang[i] = angle(xcm, x[i], ycm, y[i]);

    // Compute the difference
    double angdiff = pang-ang[i];
    if (angdiff<-I3Constants::pi) angdiff += 2*I3Constants::pi;
    if (angdiff>I3Constants::pi) angdiff -= 2*I3Constants::pi;

    // Is this the one?
    log_debug ("This (%f, %f) ang = %f, anglediff = %e",
            x[i], y[i], ang[i], angdiff);

    std::cout << "This (" << x[i] << ", " << y[i] 
	      << ") ang = " << ang[i] 
	      << " , angdiff = " << angdiff << std::endl;

    const double SMALLNUMBER = 1.0e-15;
    if (std::abs(angdiff)<SMALLNUMBER) { // we found an exact match!
      less = 0; n_less = i;
      more = 0; n_more = i;
      std::cout << "Match!" << std::endl;
    }
    else if (angdiff<0 && angdiff>less) { // we found a new less
      less = angdiff;
      n_less = i;
      std::cout << "New less!" << std::endl; 
    }
    else if (angdiff>0 && angdiff<more) { // we found a new more
      more = angdiff;
      n_more = i;
      std::cout << "New more!" << std::endl;
    }

  }
 
  double xi, yi;
  if (less==0 && more==0) {
    log_debug("An exact match was found... %i", n_more);
    xi = x[n_more];
    yi = y[n_more];

  } else {
    log_trace("PAngle = %f, closest above = %f (%i), closest below = %f (%i)",
           pang,
           more, n_more,
           less, n_less);
   
    // Use Olga's notation
    double x1 = x[n_less]; double y1 = y[n_less];
    double x2 = x[n_more]; double y2 = y[n_more];

    // Compute intersection point
    IntersectionOfTwoLines(x1,y1,x2,y2,xcm,ycm,xprime,yprime,&xi,&yi);

    log_debug("point 1: (%f, %f)", x1, y1);
    log_debug("point 2: (%f, %f)", x2, y2);
    log_debug("CM     : (%f, %f)", xcm, ycm);
    log_debug("the point: (%f, %f)", xprime, yprime);
    log_debug("intersection: (%f, %f)", xi, yi);
  }

  // Compute ratio of distances
  double dprime = sqrt((xprime-xcm)*(xprime-xcm) + (yprime-ycm)*(yprime-ycm));
  double di = sqrt((xi-xcm)*(xi-xcm) + (yi-ycm)*(yi-ycm));
  log_debug("dprime = %f, di = %f", dprime, di);

  return dprime/di;

}

