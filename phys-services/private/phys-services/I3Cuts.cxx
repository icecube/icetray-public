#include "phys-services/I3Cuts.h"
#include "dataclasses/I3Constants.h"
#include "phys-services/I3Calculator.h"
#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/physics/I3RecoPulse.h"
#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/I3Position.h"

#include <vector>
#include <cassert>
#include <cmath>
#include <float.h>

using namespace I3Constants;
using namespace I3Calculator;

static void
crossproduct(double c[3], double a[3], double b[3])
{
	c[0] = a[1]*b[2] - a[2]*b[1];
	c[1] = a[2]*b[0] - a[0]*b[2];
	c[2] = a[0]*b[1] - a[1]*b[0];
}

static double
dotproduct(double a[3], double b[3])
{
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

static void
rotate(double a[3], double delphi, int axis)
{
	double phi, r;
	int x, y;

	x = (axis + 1) % 3;
	y = (axis + 2) % 3;
	
	r = hypot(a[x],a[y]);
	phi = atan2(a[y],a[x]) + delphi;

	a[x] = r*cos(phi);
	a[y] = r*sin(phi);
}

//--------------------------------------------------------------
void CutsCalcImpl(const I3Particle& track, const I3Geometry& geometry, 
		  const I3RecoPulseSeriesMap& hitmap,
		  const double t1, const double t2,int& Nchan, int& Nhit, int& Nstring,
		  int& Ndir, double& Ldir, double& Sdir, double& Sall)
{
  int StringNumber[120] = {0};
  Ndir = 0;
  Nhit = 0;
  Nstring = 0;
  std::vector<double> lengthAll;
  std::vector<double> lengthDir;
  double min = 999999;
  double max = -999999;
  I3RecoPulseSeriesMap::const_iterator hits_i;
  for (hits_i=hitmap.begin(); hits_i!=hitmap.end(); hits_i++) {
    const I3RecoPulseSeries& hits = hits_i->second;
    OMKey omkey = hits_i->first;
    I3OMGeoMap::const_iterator geom = geometry.omgeo.find(omkey);
    if (geom==geometry.omgeo.end()) {
      log_trace("Didn't find the current OMKey in Geometry");
      continue;
    }

    StringNumber[(int) 19+omkey.GetString()] = 1; // "19+" includes AMANDA std::strings
    const I3Position& ompos = geom->second.position;

    I3RecoPulseSeries::const_iterator hit;
    for (hit=hits.begin(); hit!=hits.end(); hit++) {
      
      double Tres = TimeResidual(track, ompos, hit->GetTime());
      log_trace("residual: %f",Tres);

      // calculate projections of hits onto track...
      Nhit++; // keep track of total hits
      // Replace this with the function version
      double dist = DistanceAlongTrack(track, ompos);
      log_trace("dist: %f",dist);
      lengthAll.push_back(dist);        // set up for SmoothnessAll calculation

      // this is a direct hit...
      if (Tres>t1 && Tres<t2) {
	Ndir+=1;                     // add direct hits
	if (dist<min) min = dist;    // set minimum for "event length"
	if (dist>max) max = dist;    // set maximum for "event length"
	lengthDir.push_back(dist);  // set up for SmoothnessDir calculation
      }

    } // end loop over hitseries

  } // end loop over hitseriesmap

  //calculate Nstring
  for (int i = 0; i < 120; i++) {
    if (StringNumber[i])
      ++Nstring;
  }

    // calculate SmoothnessAll...
  {
    sort(lengthAll.begin(),lengthAll.end());
    for (unsigned int i=0; i<lengthAll.size(); i++) 
      log_trace("lengthAll[%i]=%f",i,lengthAll[i]);
    int N = lengthAll.size()-1;
    Sall = 0;
    for (unsigned int j=1; j<lengthAll.size(); j++) {
      double lj = lengthAll[j]-lengthAll[0];
      double lN = lengthAll[N]-lengthAll[0];
      double S = (double)j/(double)N - lj/lN;
      log_trace("j: %i  N: %i  S: %f",j,N,S);
      if (fabs(S)>fabs(Sall)) Sall = S;
    }
    // calculation is meaningless for less than 3 hits
    if(lengthAll.size() <= 2) Sall = NAN;
  }

  // calculate SmoothnessDir...
  {
    sort(lengthDir.begin(),lengthDir.end());
    for (unsigned int i=0; i<lengthDir.size(); i++) 
      log_trace("lengthDir[%i]=%f",i,lengthDir[i]);
    int N = lengthDir.size()-1;
    Sdir = 0;
    for (unsigned int j=1; j<lengthDir.size(); j++) {
      double lj = lengthDir[j]-lengthDir[0];
      double lN = lengthDir[N]-lengthDir[0];
      double S = (double)j/(double)N - lj/lN;
      log_trace("j: %i  N: %i  S: %f",j,N,S);
      if (fabs(S)>fabs(Sdir)) Sdir = S;
    }
    // calculation is meaningless for less than 3 direct hits
    if(lengthDir.size() <= 2) Sdir = NAN;
  }

  Ldir = max-min; // length of event
  if (lengthDir.size()==0) Ldir = NAN;
  Nchan = hitmap.size();
  log_debug("-----> Nchan: %i",Nchan);
  log_debug("-----> Ndir: %i",Ndir);
  log_debug("-----> Nhit: %i",Nhit);
  log_debug("-----> Nstring: %i",Nstring);
  log_debug("-----> Ldir: %f",Ldir);
  log_debug("-----> Sall: %f",Sall);
  log_debug("-----> Sdir: %f",Sdir);
  return;
}

//--------------------------------------------------------------
void CascadeCutsCalcImpl(const I3Particle& vertex, const I3Geometry& geometry, 
		  const I3RecoPulseSeriesMap& hitmap,
		  const double t1, const double t2,int& Nchan, int& Nhit, int& N_1hit, int& Nstring,
		  int& Ndir, int& Nearly, int& Nlate)
{
  int StringNumber[120] = {0};
  Nhit    = 0;
  N_1hit  = 0;
  Ndir    = 0;
  Nearly  = 0;
  Nlate   = 0;
  Nstring = 0;
  I3RecoPulseSeriesMap::const_iterator hits_i;
  for (hits_i=hitmap.begin(); hits_i!=hitmap.end(); hits_i++) {
    const I3RecoPulseSeries& hits = hits_i->second;
    OMKey omkey = hits_i->first;

    if(hits.size()==1){
      log_trace("found a DOM with a single hit...");
      N_1hit++;
    }

    I3OMGeoMap::const_iterator geom = geometry.omgeo.find(omkey);
    if (geom==geometry.omgeo.end()) {
      log_trace("Didn't find the current OMKey in Geometry");
      continue;
    }

    StringNumber[(int) 19+omkey.GetString()] = 1; // "19+" includes AMANDA std::strings
    const I3Position& ompos = geom->second.position;

    I3RecoPulseSeries::const_iterator hit;
    for (hit=hits.begin(); hit!=hits.end(); hit++) {
      
      //TimeResidual function checks if the input particle is a cascade or track and then
      //handles the residual calculation appropriately
      double Tres = TimeResidual(vertex, ompos, hit->GetTime());
      log_trace("cascade time: %f",vertex.GetTime());
      log_trace("    hit time: %f",hit->GetTime());
      log_trace("    residual: %f",Tres);

      Nhit++; // keep track of total hits

      // this is an early hit...
      if (Tres<t1) {
	Nearly+=1;                     // add early hits
      }

      // this is a direct hit...
      if (Tres>t1 && Tres<t2) {
	Ndir+=1;                     // add direct hits
      }

      // this is a late hit...
      if (Tres>t2) {
	Nlate+=1;                     // add late hits
      }

    } // end loop over hitseries

  } // end loop over hitseriesmap

  //calculate Nstring
  for (int i = 0; i < 120; i++) {
    if (StringNumber[i])
      ++Nstring;
  }

  Nchan = hitmap.size();
  log_debug("-----> Nchan: %i",Nchan);
  log_debug("-----> Nhit: %i",Nhit);
  log_debug("-----> N_1hit: %i",N_1hit);
  log_debug("-----> Nstring: %i",Nstring);
  log_debug("-----> Ndir: %i",Ndir);
  log_debug("-----> Nearly: %i",Nearly);
  log_debug("-----> Nlate: %i", Nlate);
  return;
}


//--------------------------------------------------------------
I3Position COGImpl(const I3Geometry& geometry,
		   const I3RecoPulseSeriesMap& hitmap)
{
  double ampWeight=1;
  double cog[3];
  cog[0]=0.0;
  cog[1]=0.0;
  cog[2]=0.0;
  double ampsum=0.0;
  
  // I need to loop over all hit OMs, first to calculate the center of 
  // gravity of the hits and then to get the tensor of inertia.  
 
  I3RecoPulseSeriesMap::const_iterator iter;
  iter = hitmap.begin();
  while(iter != hitmap.end()) {
    
    const I3RecoPulseSeries& pulsevect = iter->second;

    if(pulsevect.empty()) {
      iter++;
      log_debug("empty RecoPulseSeries!");
      continue;
    }
    
    for (unsigned i=0; i < pulsevect.size(); i++) {
      I3RecoPulse pulse = pulsevect[i];
      double amp_tmp;
      if(std::isnan(pulse.GetCharge()) > 0 || std::isinf(pulse.GetCharge())> 0) {
	log_warn("Got a nan or inf pulse charge.  Setting it to 0 instead.  Something could be screwy with a DOM calibration!!");
  	amp_tmp=0;
      }
      else {
	amp_tmp = pulse.GetCharge();
      }

      double amp = pow(amp_tmp,ampWeight);
      ampsum+=amp;
    
      const OMKey omkey = iter->first;
      I3OMGeoMap::const_iterator geom = geometry.omgeo.find(omkey);
      if (geom==geometry.omgeo.end()) {
	log_trace("Didn't find the current OMKey in Geometry");
	continue;
      }
      const I3Position& ompos = geom->second.position;

      // calculate the center of gravity             
      cog[0] += amp*ompos.GetX();
      cog[1] += amp*ompos.GetY();
      cog[2] += amp*ompos.GetZ();
    }
    iter++;
  }
  
  if (ampsum==0) ampsum=1.0;
  I3Position cogPosition(cog[0]/ampsum, cog[1]/ampsum, cog[2]/ampsum);
  return cogPosition;
}


//--------------------------------------------------------------
I3Position I3Cuts::COG(const I3Geometry& geometry,
		       const I3RecoPulseSeriesMap& pulsemap)
{
  I3Position cog(COGImpl(geometry, pulsemap));
  return cog;
}


//--------------------------------------------------------------
void I3Cuts::CutsCalc(const I3Particle& track, const I3Geometry& geometry, 
		      const I3RecoPulseSeriesMap& pulsemap,
		      const double t1, const double t2,int& Nchan, int& Nhit, int& Nstring,
		      int& Ndir, double& Ldir, double& Sdir, double& Sall)
{
  CutsCalcImpl
    (track, geometry, pulsemap, t1, t2, Nchan, Nhit, Nstring, Ndir, Ldir, Sdir, Sall);
}

//--------------------------------------------------------------
void I3Cuts::CascadeCutsCalc(const I3Particle& vertex, const I3Geometry& geometry, 
		      const I3RecoPulseSeriesMap& pulsemap,
		      const double t1, const double t2,int& Nchan, int& Nhit, int& N_1hit, int& Nstring,
		      int& Ndir, int& Nearly, int& Nlate)
{
  CascadeCutsCalcImpl
    (vertex, geometry, pulsemap, t1, t2, Nchan, Nhit, N_1hit, Nstring, Ndir, Nearly, Nlate);
}


//--------------------------------------------------------------
int I3Cuts::Nchan(const I3Particle& track, const I3Geometry& geom, 
		  const I3RecoPulseSeriesMap& hitmap,
		  double t1, double t2)
{
  int Nchan, Nhit, Nstring, Ndir;
  double Ldir, Sdir, Sall;
  CutsCalcImpl
    (track, geom, hitmap, t1, t2, Nchan, Nhit, Nstring, Ndir, Ldir, Sdir, Sall);
  return Nchan;
}



//--------------------------------------------------------------
int I3Cuts::Nhit(const I3Particle& track, const I3Geometry& geom, 
		 const I3RecoPulseSeriesMap& hitmap,
		 double t1, double t2)
{
  int Nchan, Nhit, Nstring, Ndir;
  double Ldir, Sdir, Sall;
  CutsCalcImpl
    (track, geom, hitmap, t1, t2, Nchan, Nhit, Nstring, Ndir, Ldir, Sdir, Sall);
  return Nhit;
}

//--------------------------------------------------------------
int I3Cuts::N_1hit(const I3Particle& vertex, const I3Geometry& geom, 
		 const I3RecoPulseSeriesMap& hitmap,
		 double t1, double t2)
{
  int Nchan, Nhit, N_1hit, Nstring, Ndir, Nearly, Nlate;
  CascadeCutsCalcImpl
    (vertex, geom, hitmap, t1, t2, Nchan, Nhit, N_1hit, Nstring, Ndir, Nearly, Nlate);
  return N_1hit;
}

//--------------------------------------------------------------
int I3Cuts::Nstring(const I3Particle& track, const I3Geometry& geom, 
		 const I3RecoPulseSeriesMap& hitmap,
		 double t1, double t2)
{
  int Nchan, Nhit, Nstring, Ndir;
  double Ldir, Sdir, Sall;
  CutsCalcImpl
    (track, geom, hitmap, t1, t2, Nchan, Nhit, Nstring, Ndir, Ldir, Sdir, Sall);
  return Nstring;
}

//--------------------------------------------------------------
int I3Cuts::Ndir(const I3Particle& track, const I3Geometry& geom, 
		 const I3RecoPulseSeriesMap& hitmap,
		 double t1, double t2)
{
  int Nchan, Nhit, Nstring, Ndir;
  double Ldir, Sdir, Sall;
  CutsCalcImpl
    (track, geom, hitmap, t1, t2, Nchan, Nhit, Nstring, Ndir, Ldir, Sdir, Sall);
  return Ndir;
}

//--------------------------------------------------------------
int I3Cuts::CascadeNdir(const I3Particle& vertex, const I3Geometry& geom, 
		 const I3RecoPulseSeriesMap& hitmap,
		 double t1, double t2)
{
  int Nchan, Nhit, N_1hit, Nstring, Ndir, Nearly, Nlate;
  CascadeCutsCalcImpl
    (vertex, geom, hitmap, t1, t2, Nchan, Nhit, N_1hit, Nstring, Ndir, Nearly, Nlate);
  return Ndir;
}

//--------------------------------------------------------------
int I3Cuts::Nearly(const I3Particle& vertex, const I3Geometry& geom, 
		 const I3RecoPulseSeriesMap& hitmap,
		 double t1, double t2)
{
  int Nchan, Nhit, N_1hit, Nstring, Ndir, Nearly, Nlate;
  CascadeCutsCalcImpl
    (vertex, geom, hitmap, t1, t2, Nchan, Nhit, N_1hit, Nstring, Ndir, Nearly, Nlate);
  return Nearly;
}

//--------------------------------------------------------------
int I3Cuts::Nlate(const I3Particle& vertex, const I3Geometry& geom, 
		 const I3RecoPulseSeriesMap& hitmap,
		 double t1, double t2)
{
  int Nchan, Nhit, N_1hit, Nstring, Ndir, Nearly, Nlate;
  CascadeCutsCalcImpl
    (vertex, geom, hitmap, t1, t2, Nchan, Nhit, N_1hit, Nstring, Ndir, Nearly, Nlate);
  return Nlate;
}


//--------------------------------------------------------------
double I3Cuts::Ldir(const I3Particle& track, const I3Geometry& geom, 
		    const I3RecoPulseSeriesMap& hitmap,
		    double t1, double t2)
{
  int Nchan, Nhit, Nstring, Ndir;
  double Ldir, Sdir, Sall;
  CutsCalcImpl
    (track, geom, hitmap, t1, t2, Nchan, Nhit, Nstring, Ndir, Ldir, Sdir, Sall);
  return Ldir;
}


//--------------------------------------------------------------
double I3Cuts::SmoothAll(const I3Particle& track, const I3Geometry& geom, 
			 const I3RecoPulseSeriesMap& hitmap,
			 double t1, double t2)
{
  int Nchan, Nhit, Nstring, Ndir;
  double Ldir, Sdir, Sall;
  CutsCalcImpl
    (track, geom, hitmap, t1, t2, Nchan, Nhit, Nstring, Ndir, Ldir, Sdir, Sall);
  return Sall;
}


//--------------------------------------------------------------
double I3Cuts::SmoothDir(const I3Particle& track, const I3Geometry& geom, 
			 const I3RecoPulseSeriesMap& hitmap,
			 double t1, double t2)
{
  int Nchan, Nhit, Nstring, Ndir;
  double Ldir, Sdir, Sall;
  CutsCalcImpl
    (track, geom, hitmap, t1, t2, Nchan, Nhit, Nstring, Ndir, Ldir, Sdir, Sall);
  return Sdir;
}

//--------------------------------------------------------------
// Computes the size of the "cylinder of closest approach", as defined
// in Kath's thesis appendix A
double I3Cuts::CylinderSize(const I3Particle& track, 
			    //I3Geometry& geom, 
			    //I3OMResponseMap& ommap, 
			    double H0, double R0, double center) {

  I3Direction v = track.GetDir();
  I3Position p = track.GetPos();

  // X, Y, and Z
  double xx = p.GetX();
  double yy = p.GetY();
  double zz = p.GetZ();

  // Theta and Phi must be in RADIANS
  //double px = -cos(phi)*sin(theta);
  //double py = -sin(phi)*sin(theta);
  //double pz = -cos(theta);
  double px = v.GetX();
  double py = v.GetY();
  double pz = v.GetZ();

  double k, r;
  double aa, bb, cc, underroot, root1, root2, z1, z2;
  double bestcorner, bestcyl, zcyl, zprime;

  // Typical values for AMANDA-B10:
  // H0 = 175,
  // R0 = 100,
  // center = 45

  k = R0/H0;

  //###########################################
  // smallest cylindrical volume of radius r and total height h around 
  // (0,0,center), which is proportional to R0, H0 and contains the track 

  // 1) Solve for top/bottom clippers

  zprime = zz - center;

  aa = px*px+py*py-k*k*pz*pz;
  bb = 2*(xx*px + yy*py -k*k*zprime*pz);
  cc = xx*xx + yy*yy-k*k*zprime*zprime;
  underroot = bb*bb - 4*aa*cc;

  if (underroot >= 0.) {
    root1 = 0.5*(-bb + sqrt(underroot))/aa;
    root2 = 0.5*(-bb - sqrt(underroot))/aa;
    z1 = fabs(zprime + root1*pz);
    z2 = fabs(zprime + root2*pz);

    if (z2<z1) {
      bestcorner = z2/H0;
    } else {
      bestcorner = z1/H0;
    }

  } else {
    bestcorner = NAN;
  }

  // 2) Solve for cylinder-clipper
  if ((px==0) && (py==0)) {
    //  it's straight vertical
    r = hypot(xx,yy);
    bestcyl = r/R0;
  } else {
    // not straight vertical
    r = (xx*xx + yy*yy -
         ((px*xx + py*yy)*(px*xx + py*yy))/(px*px + py*py));
    if (r > 0) {
      r = sqrt(r);
    } else {
      r = 0.;
    }

    // But wait!  It must have at least the proper r/h ratio
    zcyl = fabs(zprime + (-(px*xx+py*yy)/(px*px+py*py))*pz);
    if (r/zcyl >= k) {
      bestcyl = r/R0;
    } else {
      bestcyl = NAN;
    }
  }

  // Which is smaller?
  
  if ((bestcorner<bestcyl)||std::isnan(bestcyl)) {
    return bestcorner;
  } else {
    return bestcyl;
  }
  
}  // end cylindersize function

//------------------------------------
// Generalized version of "CylinderSize" for a general in-ice
// array shape
double I3Cuts::ContainmentVolumeSize(const I3Particle& track, 
				     std::vector<double> x, 
				     std::vector<double> y, 
				     double zhigh, 
				     double zlow) {
  double bestanswer = NAN;

  // Error-checking... need at least three std::strings 
  if (x.size()<3) { 
  log_warn("ContainmentVolume of zero/1/2 strings: will be NAN"); 
    return NAN;
  }

  // First, compute the center of mass
  double xcm, ycm, zcm;
  CMPolygon(x, y, &xcm, &ycm);
  zcm = (zhigh+zlow)/2;
  I3Position CM(xcm,ycm,zcm);

  // Error-catching: what if the track goes right through the center of mass? 
  if (I3Calculator::IsOnTrack(track,CM,DBL_EPSILON)) return 0; 

  //---- BUG 2/16/09:
  //---- With IC-40, we have no choice but to assume they are in order! 
  // do NOT do this!
  //PutPointsInOrder(&x,&y,xcm,ycm,0);

  if (x.size() != y.size()) log_fatal("X and Y are not the same size");
  int n = x.size();

  // We're going to loop through each "wall" of the structure, extending
  // out from the center.

  
  // Set up pairs of points which make "walls"
  I3Position B;
  I3Position C;
  std::vector<I3Position> highs1;
  std::vector<I3Position> highs2;
  std::vector<I3Position> lows1;
  std::vector<I3Position> lows2;
  std::vector<I3Position> verts1;
  std::vector<I3Position> verts2;
  // The high pairs
  for (int i=0; i<n-1; i++) {
    B=I3Position(x[i],y[i],zhigh,I3Position::car);
    C=I3Position(x[i+1],y[i+1],zhigh,I3Position::car);
    highs1.push_back(B);
    highs2.push_back(C);
  }
  // The last-to-first pair which was left out (upper and lower) 
  B=I3Position(x[n-1],y[n-1],zhigh,I3Position::car);
  C=I3Position(x[0],y[0],zhigh,I3Position::car); 
  highs1.push_back(B); 
  highs2.push_back(C); 
  // The low pairs
  for (int i=0; i<n-1; i++) {
    B=I3Position(x[i],y[i],zlow,I3Position::car);
    C=I3Position(x[i+1],y[i+1],zlow,I3Position::car);
    lows1.push_back(B);
    lows2.push_back(C);
  }
  // The last-to-first pair which was left out (upper and lower) 
  B=I3Position(x[n-1],y[n-1],zlow,I3Position::car);
  C=I3Position(x[0],y[0],zlow,I3Position::car); 
  lows1.push_back(B); 
  lows2.push_back(C); 
  // The vertical pairs
  for (int i=0; i<n-1; i++) {
    B=I3Position(x[i],y[i],zhigh,I3Position::car);
    C=I3Position(x[i],y[i],zlow,I3Position::car);
    verts1.push_back(B);
    verts2.push_back(C);
  }
  // The last vertical pair which was left out
  B=I3Position(x[n-1],y[n-1],zhigh,I3Position::car);
  C=I3Position(x[n-1],y[n-1],zlow,I3Position::car);
  verts1.push_back(B);
  verts2.push_back(C);
  
  // The structure of pairs
  log_debug("Number of highs: %zu", highs1.size()); 
  log_debug("Number of lows: %zu", lows1.size()); 
  log_debug("Number of verts: %zu", verts1.size()); 
  std::vector<std::vector<I3Position> > pairs1;
  std::vector<std::vector<I3Position> > pairs2;
  pairs1.push_back(highs1);
  pairs1.push_back(lows1);
  pairs1.push_back(verts1);
  pairs2.push_back(highs2);
  pairs2.push_back(lows2);
  pairs2.push_back(verts2);


  // Now, compute intersection points for each "wall",
  // and figure out which one has the tracking going
  // "through the goalposts"

  // Loop over the high series, low series, and vertical series
  for (unsigned int j=0; j<pairs1.size(); j++) {

  std::vector<double> cvector;  // collection of odd number of segments intersected

  for (unsigned int i=0; i<pairs1[j].size(); i++) {
    //for (int i=0; i<n-1; i++) {
    
    // First, the high pairs
    I3Position B(pairs1[j][i]);
    I3Position C(pairs2[j][i]);


    I3Position P = IntersectionOfLineAndPlane(track,CM,B,C);
    // Error catching: if there is no point "P", then the track  
    // is exactly parallel to the plane being tested.   
    // For such a track the "best" size is infinitely far away. 
    // So it's okay to just skip it; the best size will come from 
    // some other plane.  So skip any planes for which this occurs. 
    if (!std::isnan(P.GetX())&&!std::isnan(P.GetY())&&!std::isnan(P.GetZ())) { 

    // Reset coordinate system to CM
    B -= CM;
    C -= CM;
    P -= CM;
    // Is it between the goalposts?
    double vB[3] = {B.GetX(), B.GetY(), B.GetZ()};
    double vC[3] = {C.GetX(), C.GetY(), C.GetZ()};
    double vP[3] = {P.GetX(), P.GetY(), P.GetZ()};
    double vn[3];

    crossproduct(vn,vB,vC);
    log_trace("Pre-rotation: (%f, %f, %f) (%f, %f, %f) (%f, %f, %f)\n",
             vB[0], vB[1], vB[2],
             vC[0], vC[1], vC[2],
             vP[0], vP[1], vP[2]
             );
    // First rotate so that n-hat lines up with the x-axis
    double nphi = atan2(vn[1],vn[0]);
    rotate(vB,-nphi,2);
    rotate(vC,-nphi,2);
    rotate(vP,-nphi,2);
    // Now rotate so that n-hat goes straight up the z-axis
    double ntheta = M_PI_2 - atan2(vn[2],hypot(vn[0],vn[1]));
    rotate(vB,-ntheta,1);
    rotate(vC,-ntheta,1);
    rotate(vP,-ntheta,1);
    // Now put B on the x-axis
    double bphi = atan2(vB[1],vB[0]);
    rotate(vB,-bphi,2);
    rotate(vC,-bphi,2);
    rotate(vP,-bphi,2);
    log_trace("Rotated: (%f, %f, %f) (%f, %f, %f) (%f, %f, %f)\n",
	     vB[0], vB[1], vB[2],
	     vC[0], vC[1], vC[2],
	     vP[0], vP[1], vP[2]
	     );

    double theta_wall = atan2(vC[1],vC[0]);
    double theta_P = atan2(vP[1],vP[0]);

    // Fix "-0" precision problem
    // XXX: Note that this code is dangerous! It depends on arbitrary
    //   constants, and can have large effects on output. The precision
    //   of atan2() depends on compiler/CPU/operating system. Later logic
    //   should be fixed instead.
    if (fabs(theta_wall)<5.*DBL_EPSILON) theta_wall=0;
    if (fabs(theta_P)<5.*DBL_EPSILON) theta_P=0;

    // Make them all positive angles
    //if (theta_wall<0) theta_wall += 2*I3Constants::pi;
    //if (theta_P<0) theta_P += 2*I3Constants::pi;
    
    // Compute the difference
    log_debug(" %d          ang_wall = %f, ang_P = %f", 
	     i, theta_wall, theta_P);
    if (CCW(0,theta_P,1) &&
        CCW(theta_P,theta_wall,0) &&
        CCW(0,theta_wall,0)) {
      log_debug("I found a (regular) pair of angles! %d",i);
      double c = TriangleExpansionFactor(0,0, vB[0], vB[1], vC[0], vC[1], vP[0], vP[1]);
      log_debug("This c = %f",c);
      cvector.push_back(c);
    }    
    if (!CCW(0,theta_P,1) &&
        !CCW(theta_P,theta_wall,0) &&
        !CCW(0,theta_wall,0)) {
      log_debug("I found a (backwards) pair of angles! %d",i);
      // Calculated the "INVERTED C"
      double c = TriangleExpansionFactor(0,0, vB[0], vB[1], vC[0], vC[1], vP[0], vP[1]);
      log_debug("This c = %f",c);
      //log_debug("Flipping it to: %f", 2-c);
      //cvector.push_back(2-c);
      cvector.push_back(c);
    }    

    /*
    if (theta_wall>=theta_P && theta_P>0) { // we found an exact match!
      log_debug("Found it! %d", i);

      // We got the right wall, now compute C = dprime/di:
      // "dprime" is the distance from the origin to the intersection point
      I3Position origin(0,0,0);
      double dprime = P.CalcDistance(origin);
      // "di" is the point along lineP
      // which intersects the line from B to C.
      double dix, diy;
      IntersectionOfTwoLines(0,0,vP[0],vP[1],
			     vB[0],vB[1],vC[0],vC[1],
			     &dix, &diy);
      double di = hypot(dix,diy);

      // Hooray!  We did it!  Is it the best one?
      if (std::isnan(bestanswer) || dprime/di < bestanswer)
	bestanswer = dprime/di;
      log_debug("New Answer: %f/%f = %f", dprime, di, bestanswer);
    
    } // end if we found the right wall
    */


  } else { log_debug("This track is parallel to the plane!  Ignoring."); } 
    // end the error-catching 
  } //end loop over the walls


  // Construct the best C
  double best_this_series = 9999999;
  for (unsigned ic=0; ic<cvector.size(); ic++) {
    double c = cvector[ic];
    if (c<best_this_series) best_this_series = c;
    //if (best_this_series>1 && c<best_this_series) best_this_series = c;
    //if (best_this_series<1 && c>best_this_series) best_this_series = c;
  }


  log_debug("THIS SERIES BEST C: %f", best_this_series);
  if (best_this_series<bestanswer || std::isnan(bestanswer)) bestanswer = best_this_series;

  }  //end loop over the three series

  return bestanswer;

}


//------------------------------------
// 2-dimensional version of containment size, for a general icetop
// array shape
double I3Cuts::ContainmentAreaSize(const I3Particle& track, 
				   std::vector<double> x, 
				   std::vector<double> y, 
				   double z) 
{

  // Error-checking... need at least three std::strings to have an area 
  unsigned xsize = x.size();
  if (x.size()<3) { 
    log_warn("ContainmentArea of zero/1/2 strings: will be NAN"); 
    return NAN;
  }

  // First, compute the center of mass
  double xcm, ycm;
  CMPolygon(x, y, &xcm, &ycm);

  // Find the (x,y) of the point at some depth
  double dist;
  if (track.GetZenith()==I3Constants::pi/2) { // exactly horizontal track! 
    log_error("ContainmentAreaSize can't handle perfectly horizontal tracks");
    dist = NAN;  ///WHAT TO DO??
  } else 
    dist = (track.GetZ()-z)/cos(track.GetZenith());
  I3Position p = track.GetPos() + dist*track.GetDir();
  double xprime = p.GetX();
  double yprime = p.GetY();
  //double xprime = track.GetX();
  //double yprime = track.GetY();

  if (std::isnan(xprime) || std::isnan(yprime)) {
    log_warn("Input core was NAN. Will return a NAN.");
    return NAN;
  }

  // Compute the angle of point P:
  I3Direction d(xprime-xcm,yprime-ycm,0);
  double pang = d.CalcPhi();
  //  double pang = angle(xcm,xprime,ycm,yprime);
  log_trace("This point's ang = %f", pang);

  double cvector[9];  // collection of odd number of segments intersected
  int nc = 0;
  
  // Now, compute angles for each corner point,
  // and figure out which two bracket the point
  //int xsize = 7;
  double ang[100];
  
  for (unsigned int i=0; i<xsize; i++) {
    I3Direction dd(x[i]-xcm,y[i]-ycm,0);
    ang[i] = dd.CalcPhi();
  }
  for (unsigned int i=0; i<xsize; i++) {
    unsigned int inext = i+1;
    if (inext==xsize) inext = 0;
    if (CCW(ang[i],pang,1) && 
	CCW(pang,ang[inext],0) && 
	CCW(ang[i],ang[inext],0)) {
      log_debug("I found a (regular) pair of angles! %d->%d",i,inext);
      double c = TriangleExpansionFactor(xcm,ycm,
					 x[i], y[i],
					 x[inext], y[inext],
					 xprime, yprime);
      cvector[nc] = c;
      nc++;
      
    }
    if (!CCW(ang[i],pang,1) && 
	!CCW(pang,ang[inext],0) && 
	!CCW(ang[i],ang[inext],0)) {
      log_debug("I found a (backwards) pair of angles! %d->%d",i,inext);
      
      // Calculated the "INVERTED C"
      double c = TriangleExpansionFactor(xcm,ycm,
					 x[inext], y[inext],
					 x[i], y[i],
					 xprime, yprime);
      log_debug("Flipping it to: %f", 2-c);
      cvector[nc] = 2-c;
      nc++;
    }
  }
  
  // Now figure out the final answer from the C's!
  double best_c_so_far = 9999999;
  if (nc % 2 == 0) log_fatal("Number of triangles should not be even! %d",nc);
  if (nc==1) best_c_so_far = cvector[0]; 
  else {
    // Is it inside or outside?  Find out by multiplying insides/outsides
    double product = 1;
    for (int ic=0; ic<nc; ic++) product *= cvector[ic]-1;
    bool inside = product>0;
    if (inside) log_trace("This one is inside. %d\n",inside);
    else  log_trace("This one is outside. %d\n",inside);

    // Construct the best C
    for (int ic=0; ic<nc; ic++) {
      double c = cvector[ic];
      bool cinside = c<1;
      if (cinside == inside) { 
	if (best_c_so_far>1 && c<best_c_so_far) best_c_so_far = c;
	if (best_c_so_far<1 && c>best_c_so_far) best_c_so_far = c;
      }
    }
  }

  log_debug("RETURNING: %f\n", best_c_so_far);
  return best_c_so_far;
  
}

////// HELPER FUNCTIONS FOR GEOMETRIC STUFF ///////

//------------------------------------
// Intersection point of two lines (in 2-d)
void I3Cuts::IntersectionOfTwoLines(double x1, double y1, double x2, double y2,
				    double x3, double y3, double x4, double y4,
				    double *xi, double *yi) {
  // An alternative method I found on the web, less prone to divide-by-zeros
  // From: http://www.pdas.com/lineint.htm
  //  ...based on: http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/
  double a1 = y2-y1;
  double b1 = x1-x2;
  double c1 = x2*y1 - x1*y2;  //{ a1*x + b1*y + c1 = 0 is line 1 }

  double a2 = y4-y3;
  double b2 = x3-x4;
  double c2 = x4*y3 - x3*y4;  //{ a2*x + b2*y + c2 = 0 is line 2 }

  double denom = a1*b2 - a2*b1;
  if (denom == 0) { // they don't intersect, they are parallel 
  //log_fatal("Denomonator zero!"); 
  *xi = NAN; 
  *yi = NAN; 
  } else { 
  *xi =(b1*c2 - b2*c1)/denom; 
  *yi =(a2*c1 - a1*c2)/denom; 
  } 

}

//------------------------------------
// Intersection point of a line and a plane (in 3-d)
// THE DOT- AND CROSS-PRODUCT VERSION
I3Position I3Cuts::IntersectionOfLineAndPlane(const I3Particle& t,
			      I3Position A, I3Position B, I3Position C) {
  double vA[3] = {A.GetX(), A.GetY(), A.GetZ()};
  double vB[3] = {B.GetX(), B.GetY(), B.GetZ()};
  double vC[3] = {C.GetX(), C.GetY(), C.GetZ()};
  double vBminusA[3] = {vB[0] - vA[0], vB[1] - vA[1], vB[2] - vA[2]};
  double vCminusA[3] = {vC[0] - vA[0], vC[1] - vA[1], vC[2] - vA[2]};
  double vn[3];

  crossproduct(vn, vBminusA, vCminusA);

  // The track direction components
  I3Direction tdir = t.GetDir();
  double vd[3] = {tdir.GetX(), tdir.GetY(), tdir.GetZ()};
  // The track vertex components
  I3Position tpos = t.GetPos();
  double vx[3] = {tpos.GetX(), tpos.GetY(), tpos.GetZ()};

  double answer[3], temp[3], scale;

  // answer = vn x (vx x vd)
  crossproduct(temp,vx,vd);
  crossproduct(answer,vn,temp);

  // answer += vd * (vn . vA)
  scale = dotproduct(vn,vA);
  for (int i = 0; i < 3; i++)
	answer[i] += vd[i]*scale;

  // answer /= (vn . vd)
  scale = dotproduct(vn,vd); 
  for (int i = 0; i < 3; i++)
	answer[i] /= scale;

  // Output the solution as an I3Position
  log_trace("L-P intersection: (x, y, z ) = (%f, %f, %f)", answer[0],answer[1],answer[2]); 
  I3Position result(answer[0],answer[1],answer[2]);
  return result;

}





//------------------------------------
// Put the border points in order around the center
void I3Cuts::PutPointsInOrder(std::vector<double> *xinput, 
			      std::vector<double> *yinput, 
			      double xcenter, double ycenter,
			      bool justcheck) {
  if (xinput->size() != yinput->size()) log_fatal("X and Y are not the same size");
  int n = xinput->size();
  int i;   // a looping variable
  
  // Make a hash table of angles... it's automatically sorted by angle
  std::map<double,int> anglehash;
  double lastangle = -99999;
  for (i=0; i<n; i++) {
    I3Direction dd((*xinput)[i]-xcenter,(*yinput)[i]-ycenter,0);
    double ang = dd.CalcPhi();
    log_debug("Ang: %f", ang);
    if ((ang < lastangle) && justcheck) 
      log_fatal("Help!  They are out of order at the end!  Irregular shape!");
    anglehash[ang] = i;
    lastangle = ang;
  }
  
  if (!justcheck) {
    // Create new SORTED border points.
    // (May be the same as the original points, that's ok too)
    std::vector<double> x;
    std::vector<double> y;
    std::map<double,int>::iterator imap;
    for (imap = anglehash.begin(); imap != anglehash.end(); imap++) {
      x.push_back((*xinput)[imap->second]);
      y.push_back((*yinput)[imap->second]);
    }
    // Copy 'em over
    for (i=0; i<n; i++) {
      log_trace("copying... %f %f", x[i], y[i]);
      (*xinput)[i] = x[i];
      (*yinput)[i] = y[i];
    }
  
  }
}



//------------------------------------
// Center of mass of an arbitrary polygon or n-gon
void I3Cuts::CMPolygon(std::vector<double> x, 
		       std::vector<double> y, 
		       double *xresult,
		       double *yresult) {
  // Chop the n-gon into (n-2) triangles.
  // Find the center and area of each triangle.
  // Find the weighted average of the center points

  if (x.size() != y.size()) log_fatal("X and Y are not the same size");
  int n = x.size();
  int i;  // a looping variable

  // Everything's great if the points are in order (either 
  // CW or CCW)... but what if they are not?
  // Here is a VERY awkward but workable way of sorting the points by angle.
  // First, take a quick average to get a guess at the CM
  //---- BUG 2/16/09: do NOT do this!
  //---- With IC-40, we have no choice but to assume they are in order!
  /*
  double xquick = 0;
  double yquick = 0;
  for (i=0; i<n; i++) {
    xquick += x[i];
    yquick += y[i];
  }
  xquick /= n;
  yquick /= n;
  PutPointsInOrder(&x,&y,xquick,yquick,0);
  */

  double running_numerator_x = 0;
  double running_numerator_y = 0;
  double running_denominator = 0;

  for (i=0; i<n-2; i++) {
    // Define the triangle
    double x1 = x[0];
    double y1 = y[0];
    double x2 = x[1+i];
    double y2 = y[1+i];
    double x3 = x[2+i];
    double y3 = y[2+i];
    // Find the midpoint.  It's the intersection point of any two
    // lines from a corner to a bisector on the other side.
    double x_mid12 = (x1+x2)/2;
    double y_mid12 = (y1+y2)/2;
    double x_mid13 = (x1+x3)/2;
    double y_mid13 = (y1+y3)/2;
    // Find the center, which is the intersection of two bisectors
    double xtriangle, ytriangle;
    IntersectionOfTwoLines(x3,y3,x_mid12,y_mid12,
			   x2,y2,x_mid13,y_mid13,
			   &xtriangle, &ytriangle);
    // Find the area of the triangle
    I3Direction d12(x2-x1,y2-y1,0);
    I3Direction d13(x3-x1,y3-y1,0);
    double area = 
      0.5*hypot(y2-y1,x2-x1)*hypot(y3-y1,x3-x1)
      *sin(d12.CalcPhi()-d13.CalcPhi());
    // Weighted average...
    running_numerator_x += area*xtriangle;
    running_numerator_y += area*ytriangle;
    running_denominator += area;
	
    // Assuming the points are in order, if one leg "goes backwards" in angle,
    // it just corresponds to a negative area.  This will happen automatically,
    // as "sin(d12.CalcPhi()-d13.CalcPhi())" above will be a negative number.

    log_debug("Triangle %d: (%f %f), (%f %f), (%f %f)",
	   i, x1,y1,x2,y2,x3,y3);
    log_trace("Midpoints (%f %f) and (%f %f)",
	   x_mid12, y_mid12, x_mid13, y_mid13);
    log_trace("Intersection point: (%f %f), Area %f",
	   xtriangle, ytriangle, area);
  }
  *xresult = running_numerator_x/running_denominator;
  *yresult = running_numerator_y/running_denominator;

  log_debug("CM Results: x = %f, y = %f",
	 *xresult, *yresult);
  //log_debug("CM Results: x = %f (%f), y = %f (%f)",
  //	 *xresult, xquick, *yresult, yquick);

  // Sanity-check... make sure the border points are still in order
  // now that we've got the "real" CM.
  // If they appear out-of-order here, it means we've got some kind of 
  // weird shape on our hands, and I'm not sure what to do.
  //---- BUG 2/16/09: do NOT do this!
  //PutPointsInOrder(&x,&y,*xresult,*yresult,1);

}

bool I3Cuts::CCW(double ang1, double ang2, bool exact) {
  if (exact) {
    if (sin(ang2-ang1)>=0) return 1;
    else return 0;
  } else {
    if (sin(ang2-ang1)>0) return 1;
    else return 0;
  }
}


double I3Cuts::TriangleExpansionFactor(double xcm, double ycm, //vertex from which triangle will expand
				       double x1, double y1,  // triangle point 2
				       double x2, double y2,  // triangle point 3
				       double xprime, double yprime)  // "the point" 
{
  //printf("point 1: (%f, %f)\n", x1, y1);
  //printf("point 2: (%f, %f)\n", x2, y2);
  //printf("CM     : (%f, %f)\n", xcm, ycm);
  //printf("the point: (%f, %f)\n", xprime, yprime);
  
  // Compute intersection point
  double xi, yi;
  IntersectionOfTwoLines(x1,y1,x2,y2,xcm,ycm,xprime,yprime,&xi,&yi);
  //printf("intersection: (%f, %f)\n", xi, yi);
  
  // Compute ratio of distances
  double dprime = sqrt((xprime-xcm)*(xprime-xcm) + (yprime-ycm)*(yprime-ycm));
  double di = sqrt((xi-xcm)*(xi-xcm) + (yi-ycm)*(yi-ycm));
  log_trace("dprime = %f, di = %f, c = %f", dprime, di, dprime/di);
  log_trace("---------------------------------");
  return dprime/di;
}       





