/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$

    @version $Revision$
    @date $Date$
    @author dule
*/

#include <I3Test.h>

#include "icetray/I3Units.h"
#include "phys-services/I3Calculator.h"
#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/I3Position.h"
#include "dataclasses/I3Direction.h"

#include <string>
#include <cmath>
#include <iostream>
#include <sstream>

using std::string;
using std::cout;
using std::endl;
using namespace I3Calculator;

static I3Particle inftrack()
{
  I3Particle inftrack;
  inftrack.SetPos(0,0,0);
  inftrack.SetDir(0,1,1);
  inftrack.SetShape(I3Particle::InfiniteTrack);
  return inftrack;
}

static I3Particle muon()
{
  I3Particle muon;
  muon.SetPos(10,0,0);
  muon.SetDir(90*I3Units::deg,0); // muon moving toward -x axis
  muon.SetShape(I3Particle::InfiniteTrack);
  return muon;
}

static I3Particle starttrack()
{
  I3Particle starttrack;
  starttrack.SetPos(1,1,1);
  starttrack.SetDir(0,0);
  starttrack.SetTime(100);
  starttrack.SetShape(I3Particle::StartingTrack);
  return starttrack;
}

static I3Particle casc1()
{
  I3Particle casc1;
  casc1.SetPos(2,2,2);
  casc1.SetTime(100);
  casc1.SetShape(I3Particle::Cascade);
  return casc1;
}

static I3Particle casc2()
{
  I3Particle casc2;
  casc2.SetPos(2,2,7);
  casc2.SetShape(I3Particle::Cascade);
  return casc2;
}

static I3Position p(1,1,1);
static I3Position q(2,2,2);
static I3Position r(0,0,0);
static I3Position a1;


void TRAFO(double *Xdir, double trafo[3][3]);


TEST_GROUP(I3Calculator)

TEST(IsOnTrack)
{
  I3Position on(2,0.09*I3Units::m,0);
  ENSURE(IsOnTrack(muon(),on,0.1*I3Units::m));

  I3Position n = inftrack().GetPos() + 2.38*inftrack().GetDir();
  ENSURE(IsOnTrack(inftrack(),n,0.1*I3Units::m));

  ENSURE(!IsOnTrack(starttrack(),on,0.1*I3Units::m));

  on=I3Position(1,1,1.1,I3Position::car);
  ENSURE(!IsOnTrack(starttrack(),on,0.1*I3Units::m));
}

TEST(ClosestApproachDistance)
{
  ENSURE_DISTANCE(ClosestApproachDistance(inftrack(),p),1.,0.0001);

  ENSURE_DISTANCE(ClosestApproachDistance(starttrack(),q),sqrt(3.),0.0001);

  a1=I3Position(1,0,-7,I3Position::car);
  ENSURE_DISTANCE(ClosestApproachDistance(starttrack(),a1),1.,0.0001);

  a1=I3Position(1,1,0,I3Position::car);
  ENSURE_DISTANCE(ClosestApproachDistance(starttrack(),a1),0.,0.0001);

  a1=I3Position(1,1,2,I3Position::car);
  ENSURE_DISTANCE(ClosestApproachDistance(starttrack(),a1),1.,0.0001);

  a1=I3Position(1,0,2,I3Position::car);
  ENSURE_DISTANCE(ClosestApproachDistance(starttrack(),a1),sqrt(2.0),0.0001);
}

TEST(CherenkovPosition)
{ 
  a1=I3Position(1,0,-1,I3Position::car);
  I3Position pos = CherenkovPosition(starttrack(),a1);
  ENSURE_DISTANCE(pos.GetZ(),0.1616288,0.0001);

  a1=I3Position(-2,1,0,I3Position::car);
  pos = CherenkovPosition(muon(),a1);
  ENSURE_DISTANCE(pos.GetX(),-0.838371,0.0001);
}

TEST(CherenkovTime)
{ 
  a1=I3Position(0,0,0,I3Position::car);
  ENSURE(std::isnan(CherenkovTime(starttrack(),a1)));

  a1=I3Position(1,0,-1,I3Position::car);
  ENSURE_DISTANCE(CherenkovTime(starttrack(),a1)/I3Units::ns,9.731156,0.0001);

  a1=I3Position(1,0,-11,I3Position::car);
  ENSURE_DISTANCE(CherenkovTime(starttrack(),a1)/I3Units::ns,43.08756,0.0001);

  a1=I3Position(-2,1,0,I3Position::car);
  ENSURE_DISTANCE(CherenkovTime(muon(),a1)/I3Units::ns,43.08756,0.0001);
}

TEST(CherenkovDistance)
{
  a1=I3Position(1,0,0,I3Position::car);
  ENSURE(std::isnan(CherenkovDistance(starttrack(),a1)));

  a1=I3Position(1,0,-1,I3Position::car);
  ENSURE_DISTANCE(CherenkovDistance(starttrack(),a1),1.532769,0.0001);

  a1=I3Position(1,0,-11,I3Position::car);
  ENSURE_DISTANCE(CherenkovDistance(starttrack(),a1),1.532769,0.0001);

  a1=I3Position(-2,1,0,I3Position::car);
  ENSURE_DISTANCE(CherenkovDistance(muon(),a1),1.532769,0.0001);

  ENSURE_DISTANCE(CherenkovDistance(muon(),q),4.33532,0.0001);
}

TEST(CherenkovAngle)
{ 
  a1=I3Position(0,1,0,I3Position::car);
  ENSURE_DISTANCE(CherenkovApproachAngle(muon(),a1)/I3Units::deg,90.0,0.0001);

  a1=I3Position(0,0,1,I3Position::car);
  ENSURE_DISTANCE(CherenkovApproachAngle(muon(),a1)/I3Units::deg,49.2761,0.0001);

  ENSURE_DISTANCE(CherenkovApproachAngle(muon(),p,I3Direction(0.,0.,1.))/I3Units::deg,117.47272,0.0001);
}

TEST(CascadeDistance)
{ 
  ENSURE_DISTANCE(CherenkovDistance(casc1(),r), 3.46410, 0.0001);
}

TEST(CascadeTime)
{
  ENSURE_DISTANCE(CherenkovTime(casc1(),r)/I3Units::ns, 15.6725, 0.001);
}

TEST(TimeResidual_track)
{
  a1=I3Position(0,0,-4,I3Position::car);
  ENSURE_DISTANCE(TimeResidual(starttrack(),a1,120), -1.0055, 0.0001);
}

TEST(TimeResidual_cascade)
{
  ENSURE_DISTANCE(TimeResidual(casc1(),r,120), 4.32749, 0.0001);
}

TEST(Angle)
{
  ENSURE_DISTANCE(Angle(inftrack(),muon())/I3Units::deg,90.,0.001);
    
  I3Particle track1(I3Particle::InfiniteTrack);
  track1.SetDir(0,1,-1);
  I3Particle track2(I3Particle::InfiniteTrack);
  track2.SetDir(1,0,-1);

  ENSURE_DISTANCE(Angle(track1,track2)/I3Units::deg,60.,0.001);
}

TEST(Distance)
{ 
  ENSURE_DISTANCE(Distance(casc1(),casc2()),5.0,0.0001);
}


TEST(garbage)
{
  // try
  //{
      I3Particle particle;
      I3Position position;

      I3Position chpos;
      double chtime;
      double chdist;
      double changle;
      CherenkovCalc(particle, position, chpos, chtime, chdist, changle);

      ENSURE(std::isnan(chpos.GetX()));
      ENSURE(std::isnan(chtime));
      ENSURE(std::isnan(chdist));
      ENSURE(std::isnan(changle));

      I3Position appos;
      double apdist;
      I3Position appos2;
      double apdist2;
      ClosestApproachCalc(particle, position, appos, apdist, appos2, apdist2);

      ENSURE(std::isnan(appos.GetX()));
      ENSURE(std::isnan(apdist));
      ENSURE(std::isnan(appos2.GetX()));
      ENSURE(std::isnan(apdist2));

      particle.SetPos(0,0,0);
      particle.SetDir(0,0,1);
      position=I3Position(2,2,2,I3Position::car);
      CherenkovCalc(particle, position, chpos, chtime, chdist, changle);

      ENSURE(std::isnan(chpos.GetX()));
      ENSURE(std::isnan(chtime));
      ENSURE(std::isnan(chdist));
      ENSURE(std::isnan(changle));

      ClosestApproachCalc(particle, position, appos, apdist, appos2, apdist2);

      ENSURE(!std::isnan(appos.GetX()));
      ENSURE(!std::isnan(apdist));
      ENSURE(!std::isnan(appos2.GetX()));
      ENSURE(!std::isnan(apdist2));

      particle.SetShape(I3Particle::InfiniteTrack);
      CherenkovCalc(particle, position, chpos, chtime, chdist, changle);

      ENSURE(!std::isnan(chpos.GetX()));
      ENSURE(!std::isnan(chtime));
      ENSURE(!std::isnan(chdist));
      ENSURE(!std::isnan(changle));

      particle.SetDir(NAN,NAN);
      ClosestApproachCalc(particle, position, appos, apdist, appos2, apdist2);

      ENSURE(std::isnan(appos.GetX()));
      ENSURE(std::isnan(apdist));
      ENSURE(std::isnan(appos2.GetX()));
      ENSURE(std::isnan(apdist2));

      //ENSURE(0,"That should have thrown");
      //}
      //catch(const exception& e)
      //{
      // that should have thrown 'cause the particle shape wasn't set
      //}
}

TEST(JAMS_time_residual)
{
  I3Particle track;
  track.SetTime(0);
  track.SetPos(0,0,0);
  track.SetDir(0,1,-1);
  track.SetShape(I3Particle::InfiniteTrack);
  double TG_CER = tan(acos(1./I3Constants::n_ice));
  I3Position pos(0,1,-10);
  double time(10);

  //--Calculate dt,rho from phys-services, SIMPLEST WAY
  double rho = ClosestApproachDistance(track, pos);
  double dt = TimeResidual(track, pos, time);

  //--Calculate dt,rho using phys-services, but break up calculation
  double rho_ = ClosestApproachDistance(track, pos);
  double l = DistanceAlongTrack(track,ClosestApproachPosition(track, pos));
  double dt_ = time - track.GetTime() - (l)/I3Constants::c
    - rho_*TG_CER/I3Constants::c;

  //--Calculate dt,rho Peter's way, but using phys-services
  I3Position p1 = InTrackSystem(track, pos);
  double rho1 = p1.GetRho();
  double Z = p1.GetZ() - track.GetZ();
  double dt1 = time - track.GetTime() - ( Z + rho1*TG_CER ) / I3Constants::c;

  //--Calculate dt,rho Peter's way from code, using TRAFO
  double dir[3];
  dir[0] = track.GetDir().GetX();
  dir[1] = track.GetDir().GetY();
  dir[2] = track.GetDir().GetZ();
  double trafo[3][3];
  TRAFO(dir, trafo);
  double Tom   = time - track.GetTime();
  double Xom   = pos.GetX() - track.GetX();
  double Yom   = pos.GetY() - track.GetY();
  double Zom   = pos.GetZ() - track.GetZ();
  double XX    = Xom * trafo[0][0] + Yom * trafo[1][0] + Zom * trafo[2][0];
  double YY    = Xom * trafo[0][1] + Yom * trafo[1][1] + Zom * trafo[2][1];
  double ZZ    = Xom * trafo[0][2] + Yom * trafo[1][2] + Zom * trafo[2][2];
  double rho2   = sqrt(XX*XX + YY*YY);
  double dt2    = Tom - (ZZ + rho2*TG_CER)/I3Constants::c;

  //--Compare the different ways of calculation
  double RHO = 4.5*sqrt(2.);
  double DT = -35.4181;
  double PERC = 0.001;

  ENSURE_DISTANCE(rho,  RHO, std::abs(RHO*PERC));
  ENSURE_DISTANCE(rho_, RHO, std::abs(RHO*PERC));
  ENSURE_DISTANCE(rho1, RHO, std::abs(RHO*PERC));
  ENSURE_DISTANCE(rho2, RHO, std::abs(RHO*PERC));

  ENSURE_DISTANCE(dt,  DT, std::abs(DT*PERC));
  ENSURE_DISTANCE(dt_, DT, std::abs(DT*PERC));
  ENSURE_DISTANCE(dt1, DT, std::abs(DT*PERC));
  ENSURE_DISTANCE(dt2, DT, std::abs(DT*PERC));
}


TEST(JAMS_time_residual_many)
{
  double TG_CER = tan(acos(1./I3Constants::n_ice));
  I3Particle track;
  track.SetShape(I3Particle::InfiniteTrack);
  track.SetTime(0);
  track.SetPos(0,0,0);

  for (int i=1; i<10; i++) {
    double zen = 180*I3Units::deg/10 * (double)i;
    double azi = 360*I3Units::deg/10 * (double)i;
    track.SetDir(zen, azi);
    double time(10);

    // vary x,y,z of the OM from -120 to +120...
    for (int j1=-3; j1<3; j1++,j1++) {
      for (int j2=-3; j2<3; j2++,j2++) {
	for (int j3=-3; j3<3; j3++,j3++) {
	  I3Position pos(40.*(double)j1, 40.*(double)j2, 40.*(double)j3);

    //--Calculate dt,rho from phys-services, SIMPLEST WAY
    double rho = ClosestApproachDistance(track, pos);
    double dt = TimeResidual(track, pos, time, I3Constants::n_ice, I3Constants::n_ice);

    //--Calculate dt,rho using phys-services, but break up calculation
    double rho_ = ClosestApproachDistance(track, pos);
    double l = DistanceAlongTrack(track,ClosestApproachPosition(track, pos));
    double dt_ = time - track.GetTime() - (l)/I3Constants::c
      - rho_*TG_CER/I3Constants::c;

    //--Calculate dt,rho Peter's way, but using phys-services
    I3Position p1 = InTrackSystem(track, pos);
    double rho1 = p1.GetRho();
    double Z = p1.GetZ() - track.GetZ();
    double dt1 = time - track.GetTime() - ( Z + rho1*TG_CER ) / I3Constants::c;

    //--Calculate dt,rho Peter's way from code, using TRAFO
    double dir[3];
    dir[0] = track.GetDir().GetX();
    dir[1] = track.GetDir().GetY();
    dir[2] = track.GetDir().GetZ();
    double trafo[3][3];
    TRAFO(dir, trafo);
    double Tom   = time - track.GetTime();
    double Xom   = pos.GetX() - track.GetX();
    double Yom   = pos.GetY() - track.GetY();
    double Zom   = pos.GetZ() - track.GetZ();
    double XX    = Xom * trafo[0][0] + Yom * trafo[1][0] + Zom * trafo[2][0];
    double YY    = Xom * trafo[0][1] + Yom * trafo[1][1] + Zom * trafo[2][1];
    double ZZ    = Xom * trafo[0][2] + Yom * trafo[1][2] + Zom * trafo[2][2];
    double rho2   = sqrt(XX*XX + YY*YY);
    double dt2    = Tom - (ZZ + rho2*TG_CER)/I3Constants::c;

      cout<<track.GetDir().GetX()<<" "<<track.GetDir().GetY()<<" "
      <<track.GetDir().GetZ()<<" "<<endl;//###
      cout<<time<<" "<<l/I3Constants::c<<" "<<rho_*TG_CER/I3Constants::c<<endl;
      cout<<time<<" "<<Z/I3Constants::c<<" "<<rho1*TG_CER/I3Constants::c<<endl;
      cout<<Tom<<" "<<ZZ/I3Constants::c<<" "<<rho2*TG_CER/I3Constants::c<<endl;
      cout<<track.GetDir().CalcTheta()/I3Units::deg<<endl;
      cout<<track.GetDir().CalcPhi()/I3Units::deg<<endl;
      cout<<"...  "<<dt<<"   "<<rho<<"   "<<dt-rho<<endl;
      cout<<"---  "<<dt_<<"   "<<rho_<<"   "<<dt_-rho_<<endl;
      cout<<"===  "<<dt1<<"   "<<rho1<<"   "<<dt1-rho1<<endl;
      cout<<"###  "<<dt2<<"   "<<rho2<<"   "<<dt2-rho2<<endl;

      //--Compare the different ways of calculation
      double PREC = 0.0001;
      ENSURE_DISTANCE(rho,  rho, std::abs(rho*PREC));
      ENSURE_DISTANCE(rho_, rho, std::abs(rho*PREC));
      ENSURE_DISTANCE(rho1, rho, std::abs(rho*PREC));
      ENSURE_DISTANCE(rho2, rho, std::abs(rho*PREC));
      
      ENSURE_DISTANCE(dt,  dt, std::abs(dt*PREC));
      ENSURE_DISTANCE(dt_, dt, std::abs(dt*PREC));
      ENSURE_DISTANCE(dt1, dt, std::abs(dt*PREC));
      ENSURE_DISTANCE(dt2, dt, std::abs(dt*PREC));
	}
      }
    }
  }
}


// from Peter Steffen's JAMS code (in amajams)
//-----------------------------------------
void TRAFO ( double Xdir[3] , double trafo[3][3] ) {
  double DXx,DXy,DXz, DYx,DYy,DYz, DZx,DZy,DZz, Norm;

/*                                            |  Z-axis = track dir.   |*/
  DZx    = Xdir[0];
  DZy    = Xdir[1];
  DZz    = Xdir[2];

  if(DZx*DZx + DZy*DZy < 1.e-10) {
    DXx    = 1.;
    DXy    = 0.;
    DXz    = 0.;
    DYx    = 0.;
    DYy    = 1.;
    DYz    = 0.;
    DZx    = 0.;
    DZy    = 0.;
    DZz    = 1.;
  }

  else{
/*                                            |  X: transvers Z-axis   |*/
    DXx    =  DZy;
    DXy    = -DZx;
    DXz    =  .0;
    Norm   = sqrt(DXx*DXx + DXy*DXy);
    DXx    =  DXx / Norm;
    DXy    =  DXy / Norm;

/*                                            |  Y-axis                |*/
    DYx    =  DZy*DXz - DZz*DXy;
    DYy    =  DZz*DXx - DZx*DXz;
    DYz    =  DZx*DXy - DZy*DXx;
    Norm   = sqrt(DYx*DYx + DYy*DYy + DYz*DYz);
    DYx    =  DYx / Norm;
    DYy    =  DYy / Norm;
    DYz    =  DYz / Norm;

  }

  trafo[0][0] = DXx;
  trafo[1][0] = DXy;
  trafo[2][0] = DXz;
  trafo[0][1] = DYx;
  trafo[1][1] = DYy;
  trafo[2][1] = DYz;
  trafo[0][2] = DZx;
  trafo[1][2] = DZy;
  trafo[2][2] = DZz;

  return;
}


TEST(TransverseDirections)
{
    double deg2rad = M_PI/180.0;
    double tolerance = 0.0001;
    for ( int izen = 0; izen <=180; izen += 30 ){
        for ( int iazi = 0; iazi <=360; iazi += 30 ){
            std::ostringstream anglespec;
            anglespec << " at "
                      << "zenith=" << izen << "deg and "
                      << "azimuth=" << iazi << "deg ";
            std::string where = anglespec.str();
            std::string problem;
            I3Direction dir(izen*deg2rad, iazi*deg2rad);
            std::pair<I3Direction,I3Direction>
                perp( I3Calculator::GetTransverseDirections(dir) );

            I3Position v1( dir.GetX(), dir.GetY(), dir.GetZ() );
            I3Position v2( perp.first.GetX(), perp.first.GetY(), perp.first.GetZ() );
            I3Position v3( perp.second.GetX(), perp.second.GetY(), perp.second.GetZ() );	    

            problem = "perpendicular failure";
            ENSURE_DISTANCE( v1*v2, 0.0, tolerance, problem+where );
            ENSURE_DISTANCE( v2*v3, 0.0, tolerance, problem+where );
            ENSURE_DISTANCE( v3*v1, 0.0, tolerance, problem+where );

            problem = "magnitude failure";
            ENSURE_DISTANCE( v1.Mag2(), 1.0, tolerance, problem+where );
            ENSURE_DISTANCE( v2.Mag2(), 1.0, tolerance, problem+where );
            ENSURE_DISTANCE( v3.Mag2(), 1.0, tolerance, problem+where );

            I3Position v123( v1.Cross(v2) - v3 );
            I3Position v231( v2.Cross(v3) - v1 );
            I3Position v312( v3.Cross(v1) - v2 );
            problem = "cross product failure";
            ENSURE_DISTANCE( v123.Mag2(), 0.0, tolerance, problem+where );
            ENSURE_DISTANCE( v231.Mag2(), 0.0, tolerance, problem+where );
            ENSURE_DISTANCE( v312.Mag2(), 0.0, tolerance, problem+where );
        }
    }
}

TEST(TrivialRotation)
{
    double zenith = 0.25 * M_PI * I3Units::radian;
    double azimuth = 0.75 * M_PI * I3Units::radian;
    double angle = 0.5 * M_PI * I3Units::radian;
    double tolerance = 0.001 * I3Units::radian;
    I3Direction axis(zenith,azimuth);
    I3Direction  dir(zenith,azimuth);
    I3Calculator::Rotate(axis,dir,angle);
    ENSURE_DISTANCE( dir.GetZenith(), zenith, tolerance,
                     "zenith changed when rotating around itself" );
    ENSURE_DISTANCE( dir.GetAzimuth(), azimuth, tolerance,
                     "azimuth changed when rotating around itself" );
    std::pair<I3Direction,I3Direction> perp( I3Calculator::GetTransverseDirections(axis) );
    I3Calculator::Rotate(axis,perp.first,90*I3Units::degree);
    I3Position diff( perp.first.GetX() - perp.second.GetX(),
		     perp.first.GetY() - perp.second.GetY(),
		     perp.first.GetZ() - perp.second.GetZ() );
    ENSURE_DISTANCE( diff.Mag2(), 0.0, tolerance,
                     "90 degrees on right handed triad did not work" );
}

TEST(Rotation30)
{
    const double xyz1[3] = { 0, cos(30*I3Units::degree), sin(30*I3Units::degree) };
    const double xyz2[3] = { 0, -sin(30*I3Units::degree), cos(30*I3Units::degree) };
    const std::string names[3] = {"x","y","z"};
    const std::string msg0 = "30 degrees around ";
    const std::string msg1 = "-axis did not work (";
    const std::string msg2 = "-coordinate wrong)";
    const double tolerance = 0.001;

    for ( int i=0; i<3; ++i ){
        double dircos0[3] = {0,0,0};
        double dircos1[3] = {0,0,0};
        double dircos2[3] = {0,0,0};
        dircos0[(i+0)%3] = 1;
        dircos1[(i+1)%3] = 1;
        dircos2[(i+2)%3] = 1;
        I3Direction axis(     dircos0[0], dircos0[1], dircos0[2] );
        I3Direction testdir1( dircos1[0], dircos1[1], dircos1[2] );
        I3Direction testdir2( dircos2[0], dircos2[1], dircos2[2] );
        I3Calculator::Rotate(axis,testdir1,30*I3Units::degree);
        I3Calculator::Rotate(axis,testdir2,30*I3Units::degree);
        std::string a=names[i];
        ENSURE_DISTANCE( testdir1.GetX(), xyz1[(3-i)%3], tolerance,
                         msg0+names[i]+msg1+"x"+msg2 );
        ENSURE_DISTANCE( testdir1.GetY(), xyz1[(4-i)%3], tolerance,
                         msg0+names[i]+msg1+"y"+msg2 );
        ENSURE_DISTANCE( testdir1.GetZ(), xyz1[(5-i)%3], tolerance,
                         msg0+names[i]+msg1+"z"+msg2 );
        ENSURE_DISTANCE( testdir2.GetX(), xyz2[(3-i)%3], tolerance,
                         msg0+names[i]+msg1+"x"+msg2 );
        ENSURE_DISTANCE( testdir2.GetY(), xyz2[(4-i)%3], tolerance,
                         msg0+names[i]+msg1+"y"+msg2 );
        ENSURE_DISTANCE( testdir2.GetZ(), xyz2[(5-i)%3], tolerance,
                         msg0+names[i]+msg1+"z"+msg2 );
        std::cout << "rotation around " << names[i] << " OK" << std::endl;
    }

    // need some more tests

}
