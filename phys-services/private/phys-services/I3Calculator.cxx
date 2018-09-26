/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$

    @version $Revision$
    @date $Date$
    @author

    @todo

*/
#include "phys-services/I3Calculator.h"

using namespace std;

//--------------------------------------------------------------

void I3Calculator::CherenkovCalc(const I3Particle& particle,  // input
				 const I3Position& position,  // input
				 I3Position& chpos,           // output
				 double& chtime,              // output
				 double& chdist,              // output
				 double& chapangle,           // output
				 const double IndexRefG,      // input
				 const double IndexRefP,      // input
				 const I3Direction& direction) // input
{
  I3Position appos;     // output 
  double apdist;        // output
  //--Only calculate if particle has a direction
  if(particle.IsTrack()) 
  {
    double changle = acos(1/IndexRefP); // calculate Cherenkov angle

    I3Position appos_ss;
    double apdist_ss;
    ClosestApproachCalc(particle,position, appos, apdist, appos_ss, apdist_ss);
    double s = DistanceAlongTrack(particle, appos); //distance between track position and closest approach position

    chdist = apdist/sin(changle);  //distance between cherenkov position and OM
    double a = s - apdist/tan(changle); //distance between track position and cherenkov position
    chtime = (a + chdist*IndexRefG)/I3Constants::c; //time between track position and OM

    double pos1_x = particle.GetX() + a*particle.GetDir().GetX(); //cher pos
    double pos1_y = particle.GetY() + a*particle.GetDir().GetY();
    double pos1_z = particle.GetZ() + a*particle.GetDir().GetZ();
    chpos=I3Position(pos1_x, pos1_y, pos1_z, I3Position::car);

    double chdist_x = position.GetX() - pos1_x;  //x component of vector between cherenkov position and OM
    double chdist_y = position.GetY() - pos1_y;  //y component of vector between cherenkov position and OM
    double chdist_z = position.GetZ() - pos1_z;  //z component of vector between cherenkov position and OM

    //angle between (vector between cherenkov position and OM).
    //and (OM axis as given by the direction argument)
    //direction is already normalized, chdist_* is not
    chapangle = std::acos(-(direction.GetX()*chdist_x + direction.GetY()*chdist_y + direction.GetZ()*chdist_z)/chdist);

    if((particle.GetShape()==I3Particle::StartingTrack && a<0) || 
             //track starts after pos1 (cherenkov point)
       (particle.GetShape()==I3Particle::StoppingTrack && a>0) || 
             //track stops before pos1 (cherenkov point)
       (particle.GetShape()==I3Particle::ContainedTrack && 
         (a<0 || particle.GetLength()<a)))  
             //track starts after or stops before pos1 (cherenkov point)
    { 
      chpos = I3Position(); // NullPos()
      chtime = NAN;
      chdist = NAN;
      chapangle = NAN;
    }

  }
  else
  {
    //--Don't calculate if track does not have direction
    log_debug("CherenkovCalc() - particle is not a track. Not calculating.");
    chpos = I3Position(); // NullPos()
    chtime = NAN;
    chdist = NAN;
    chapangle = NAN;
  }
  return;
}

//--------------------------------------------------------------
bool I3Calculator::IsOnTrack(const I3Particle& track, const I3Position& position, const double Precision)
{
  if (track.IsTrack()) {
    I3Position appos,appos2;
    double apdist,apdist2;
    ClosestApproachCalc(track,position,appos2,apdist2,appos,apdist);
    if (!std::isnan(apdist) && apdist<=Precision) return true;
    else return false;
  }
  else {
    log_debug("IsOnTrack() - particle is not a track.");
    return false;
  }
}


//--------------------------------------------------------------
void I3Calculator::ClosestApproachCalc(const I3Particle& particle,
				       const I3Position& position,
				       I3Position& appos_inf,
				       double& apdist_inf,
				       I3Position& appos_stopstart,
				       double& apdist_stopstart) 
{
  if(particle.HasDirection()) 
  {
    double theta  = particle.GetZenith();
    double phi    = particle.GetAzimuth();
    double pos0_x = particle.GetX();
    double pos0_y = particle.GetY();
    double pos0_z = particle.GetZ();
    
    double e_x = -sin(theta)*cos(phi);  //unit vector of direction of particle
    double e_y = -sin(theta)*sin(phi);
    double e_z = -cos(theta);
    
    double h_x = position.GetX() - pos0_x;  //vector between particle position and OM
    double h_y = position.GetY() - pos0_y;
    double h_z = position.GetZ() - pos0_z;
    
    double s = e_x*h_x + e_y*h_y + e_z*h_z; //distance between particle position and closest approach position
    
    double pos2_x = pos0_x + s*e_x; //closest approach position
    double pos2_y = pos0_y + s*e_y;
    double pos2_z = pos0_z + s*e_z;
    
    appos_inf=I3Position(pos2_x, pos2_y, pos2_z, I3Position::car); //closest approach position
    apdist_inf = (position-appos_inf).Magnitude();  //closest approach distance
    
    // Adjustment for contained/stopping/starting tracks
    appos_stopstart = appos_inf;
    apdist_stopstart = apdist_inf;
    if((particle.GetShape()==I3Particle::StartingTrack && s<0) || 
       //track starts after pos2 (closest approach position)
       (particle.GetShape()==I3Particle::StoppingTrack && s>0) || 
       //track stops before pos2 (closest approach position)
       (particle.GetShape()==I3Particle::ContainedTrack && s<0))
      //track starts after pos2 (closest approach position)
      { 
	appos_stopstart = particle.GetPos();
	apdist_stopstart = (position-appos_stopstart).Magnitude();
      }
    if(particle.GetShape()==I3Particle::ContainedTrack && particle.GetLength()<s)
      //track stops before pos2 (closest approach position)
      {
	appos_stopstart = particle.GetStopPos();
	apdist_stopstart = (position-appos_stopstart).Magnitude();
      }
  } // if it has direction
  else
  {
    //--Don't calculate if particle does not have direction
    log_debug("ClosestApproachCalc() - particle has no direction. "
	     "Not calculating.");
    appos_inf = I3Position(); // NullPosition();
    apdist_inf = NAN;
    appos_stopstart = I3Position(); // NullPosition();
    apdist_stopstart = NAN;
  }
  return;
}

//--------------------------------------------------------------
I3Position I3Calculator::ClosestApproachPosition(const I3Particle& particle, const I3Position& position)
{
  if (particle.HasPosition()) {
    if (particle.IsTrack()) {
      I3Position appos,appos2;
      double apdist,apdist2;
      ClosestApproachCalc(particle,position,appos2,apdist2,appos,apdist);
      return appos;
    }
    else if (particle.IsCascade()) {
      return particle.GetPos();
    }
  }
  log_debug("ClosestApproachPosition() - particle has no position, "
	    "or is neither a track nor cascade.");
  I3Position nullpos;
  return nullpos;
}


//--------------------------------------------------------------
double I3Calculator::ClosestApproachDistance(const I3Particle& particle, const I3Position& position)
{
  if (particle.HasDirection()) {
    if (particle.IsTrack()) {
      I3Position appos,appos2;
      double apdist,apdist2;
      ClosestApproachCalc(particle,position,appos2,apdist2,appos,apdist);
      return apdist;
    }
    else if (particle.IsCascade()) {
      return (position-particle.GetPos()).Magnitude();
    }
  }
  log_debug("ClosestApproachDistance() - particle has no position, "
            "or is neither a track nor cascade.");
  return NAN;
}


//--------------------------------------------------------------
double I3Calculator::DistanceAlongTrack(const I3Particle& track, const I3Position& ompos) {
  if (track.IsTrack()) {
    I3Position pos(ompos);
    pos -= track.GetPos();
    pos.RotateZ(-track.GetDir().CalcPhi());
    pos.RotateY(-track.GetDir().CalcTheta());
    return pos.GetZ();
  }
  else {
    log_debug("DistanceAlongTrack() - particle is not a track.");
    return NAN;
  }
}

//--------------------------------------------------------------
I3Position I3Calculator::CherenkovPosition(const I3Particle& particle, const I3Position& position, const double IndexRefG, const double IndexRefP)
{
  if (particle.IsTrack()) {
    I3Position chpos;
    double chtime,chdist,chapangle;
    CherenkovCalc(particle,position,chpos,chtime,chdist,chapangle,IndexRefG,IndexRefP);
    return chpos;
  }
  else if (particle.IsCascade()) {
    return particle.GetPos();
  }
  else {
    log_debug("CherenkovPosition() - particle is not a track.");
    I3Position nullpos;
    return nullpos;
  }
}


//--------------------------------------------------------------
double I3Calculator::CherenkovTime(const I3Particle& particle, const I3Position& position, const double IndexRefG, const double IndexRefP )
{
  if (particle.IsTrack()) {
    I3Position chpos;
    double chtime,chdist,chapangle;
    CherenkovCalc(particle, position, chpos, chtime, chdist,
		  chapangle, IndexRefG, IndexRefP);
   return chtime;
  }
  else if (particle.IsCascade()) {
    double speed = I3Constants::c/IndexRefG;
    return (position-particle.GetPos()).Magnitude() / speed;
  }
  else {
    log_debug("CherenkovTime() - particle is neither a track nor a cascade.");
    return NAN;
  }
}


//--------------------------------------------------------------
double I3Calculator::CherenkovDistance(const I3Particle& particle, const I3Position& position, const double IndexRefG, const double IndexRefP)
{
  if (particle.IsTrack()) {
    I3Position chpos;
    double chtime,chdist,chapangle;
    CherenkovCalc(particle, position, chpos, chtime, chdist,
		  chapangle, IndexRefG, IndexRefP);
    return chdist;
  }
  else if (particle.IsCascade()) {
    return (position-particle.GetPos()).Magnitude();
  }
  else {
    log_debug("CherenkovDistance() - particle is neither track nor cascade.");
    return NAN;
  }
}


//--------------------------------------------------------------
double I3Calculator::CherenkovApproachAngle(const I3Particle& track, const I3Position& position, const I3Direction& direction, const double IndexRefG, const double IndexRefP)
{
  if (track.IsTrack()) {
    I3Position chpos;
    double chtime,chdist,chapangle;
    CherenkovCalc(track,position,chpos,chtime,chdist,chapangle,
		  IndexRefG,IndexRefP,direction);
    return chapangle;
  }
  else {
    log_debug("CherenkovApproachAngle() - particle is not a track'");
    return NAN;
  }
}

//--------------------------------------------------------------
double I3Calculator::TimeResidual(const I3Particle& particle, const I3Position& hitpos, double hittime, const double IndexRefG, const double IndexRefP)
{
  double T_exp = CherenkovTime(particle, hitpos, IndexRefG, IndexRefP );
  double T_meas = hittime - particle.GetTime();
  return T_meas - T_exp;
}

//--------------------------------------------------------------
double I3Calculator::Angle(const I3Particle& p1, const I3Particle& p2)
{
  if (p1.HasDirection() && p2.HasDirection()) {
    I3Direction dir1(p1.GetDir());
    I3Direction dir2(p2.GetDir());
    dir1.RotateZ(-dir2.CalcPhi());
    dir1.RotateY(-dir2.CalcTheta());
    double theta = dir1.CalcTheta();
    return theta;
  }
  else {
    log_debug("Angle() - one of particles has no direction.");
    return NAN;
  }
}

//--------------------------------------------------------------
double I3Calculator::Distance(const I3Particle& p1, const I3Particle& p2)
{
  if (p1.HasPosition() && p2.HasPosition()) {
    return (p1.GetPos()-p2.GetPos()).Magnitude();
  }
  else {
    log_debug("Distance() - one of particles has no position.");
    return NAN;    
  }
}

//--------------------------------------------------------------
double I3Calculator::FourDistance(const I3Particle& p1, const I3Particle& p2)
{
  // 4-distance here is defined as:
  //   sqrt( (ct1-ct2)^2 - (x1-x2)^2 + (y1-y2)^2 + (z1-z2)^2 )
  if (p1.HasPosition() && p2.HasPosition() && 
      !std::isnan(p1.GetTime()) && !std::isnan(p2.GetTime())) {
    return sqrt(abs(pow(I3Constants::c*(p1.GetTime()-p2.GetTime()),2) - 
		    pow(p1.GetX()-p2.GetX(),2) -
		    pow(p1.GetY()-p2.GetY(),2) -
		    pow(p1.GetZ()-p2.GetZ(),2)));
  }
  else {
    log_debug("FourDistance() - particles' direction or time not present.");
    return NAN;
  }
}

//--------------------------------------------------------------
I3Position I3Calculator::InTrackSystem(const I3Direction& direction, const I3Position& pos)
{
  I3Position p(pos);
  p.RotateZ(-direction.CalcPhi());
  p.RotateY(-direction.CalcTheta());
  p.RotateZ(direction.CalcPhi()); //get x-y orientation right
  return p;
}


//--------------------------------------------------------------
I3Direction I3Calculator::InTrackSystem(const I3Direction& direction, const I3Direction& dir)
{
  I3Direction d(dir);
  d.RotateZ(-direction.CalcPhi());
  d.RotateY(-direction.CalcTheta());
  d.RotateZ(direction.CalcPhi()); //get x-y orientation right
  return d;
}


//--------------------------------------------------------------
I3Position I3Calculator::InTrackSystem(const I3Particle& track, const I3Position& pos)
{
  if (track.IsTrack()) {
    return InTrackSystem(track.GetDir(), pos);
  }
  else {
    log_debug("InTrackSystem() - particle is not a track.");
    I3Position nullpos;
    return nullpos;
  }
}


//--------------------------------------------------------------
I3Direction I3Calculator::InTrackSystem(const I3Particle& track, const I3Direction& dir)
{
  if (track.IsTrack()) {
    return InTrackSystem(track.GetDir(), dir);
  }
  else {
    log_debug("InTrackSystem() - particle is not a track.");    
    I3Direction nulldir;
    return nulldir;
  }
}


//--------------------------------------------------------------
I3Position I3Calculator::InNominalSystem(const I3Direction& direction, const I3Position& pos)
{
  I3Position p(pos);
  p.RotateZ(-direction.CalcPhi()); //get x-y orientation right
  p.RotateY(direction.CalcTheta());
  p.RotateZ(direction.CalcPhi());
  return p;
}


//--------------------------------------------------------------
I3Direction I3Calculator::InNominalSystem(const I3Direction& direction, const I3Direction& dir)
{
  I3Direction d(dir);
  d.RotateZ(-direction.CalcPhi()); //get x-y orientation right
  d.RotateY(direction.CalcTheta());
  d.RotateZ(direction.CalcPhi());
  return d;
}


//--------------------------------------------------------------
I3Position I3Calculator::InNominalSystem(const I3Particle& track, const I3Position& pos)
{
  if (track.IsTrack()) {
    return InNominalSystem(track.GetDir(), pos);
  }
  else {
    log_debug("InNominalSystem() - particle is not a track.");
    I3Position nullpos;
    return nullpos;
  }
}


//--------------------------------------------------------------
I3Direction I3Calculator::InNominalSystem(const I3Particle& track, const I3Direction& dir)
{
  if (track.IsTrack()) {
    return InNominalSystem(track.GetDir(), dir);
  }
  else {
    log_debug("InNominalSystem() - particle is not a track.");
    I3Direction nulldir;
    return nulldir;
  }
}

//--------------------------------------------------------------
std::pair<I3Direction,I3Direction>
I3Calculator::GetTransverseDirections(const I3Direction &dir){
  double dircos[3] = {dir.GetX(), dir.GetY(), dir.GetZ() };
  int imin=0;
  if ( fabs(dircos[1]) < fabs(dircos[0]) ) imin = 1;
  if ( fabs(dircos[2]) < fabs(dircos[imin]) ) imin = 2;
  log_trace("imin=%d",imin);
  double px = dircos[imin];
  double py = dircos[(imin+1)%3];
  double pz = dircos[(imin+2)%3];
#ifndef I3_COMPILE_OUT_VERBOSE_LOGGING
  double d[3] = {px,py,pz};
#endif
  log_trace("px=%f py=%f pz=%f", px, py, pz );
  double nyz = 1.0/sqrt(py*py+pz*pz);
  double q1[3] = {       0.,    pz*nyz,   -py*nyz };
  double q2[3] = { -1.0/nyz, px*py*nyz, px*pz*nyz };
  log_trace("q1x=%f q1y=%f q1z=%f", q1[0], q1[1], q1[2] );
  log_trace("q2x=%f q2y=%f q2z=%f", q2[0], q2[1], q2[2] );
  std::pair<I3Direction,I3Direction> q1q2;
  q1q2.first = I3Direction( q1[(3-imin)%3],
                            q1[(4-imin)%3],
                            q1[(5-imin)%3] );
  q1q2.second = I3Direction( q2[(3-imin)%3],
                             q2[(4-imin)%3],
                             q2[(5-imin)%3] );

  log_trace( "dircos[0]=%f dircos[1]=%f dircos[2]=%f",
             dircos[0], dircos[1], dircos[2] );
  log_trace( "dorcos[0]=%f dorcos[1]=%f dorcos[2]=%f",
             d[(3-imin)%3], d[(4-imin)%3], d[(5-imin)%3] );
  return q1q2;
}

//--------------------------------------------------------------
I3Direction I3Calculator::GetReverseDirection(const I3Direction &dir){
  return I3Direction( -dir.GetX(), -dir.GetY(), -dir.GetZ() );
}

//--------------------------------------------------------------
void I3Calculator::Rotate(const I3Direction &axis, I3Direction &dir, double angle ){

  // paranoia: user should not give references to the same object
  // (it's fine if axis and dir have the same value,
  //  but they should not be the same object)
  assert(&axis != &dir);

  double sa = sin(angle);
  double ca = cos(angle);
  if ( (sa == 0.0) &&  (ca == 1.0) ){
    return;
  }
  double dx = axis.GetX();
  double dy = axis.GetY();
  double dz = axis.GetZ();
  double X = dir.GetX();
  double Y = dir.GetY();
  double Z = dir.GetZ();

  // code copied from TRotation.cxx ;-)
  double newX = (ca+(1-ca)*dx*dx      )*X + (   (1-ca)*dx*dy-sa*dz)*Y + (   (1-ca)*dx*dz+sa*dy)*Z;
  double newY = (   (1-ca)*dy*dx+sa*dz)*X + (ca+(1-ca)*dy*dy      )*Y + (   (1-ca)*dy*dz-sa*dx)*Z;
  double newZ = (   (1-ca)*dz*dx-sa*dy)*X + (   (1-ca)*dz*dy+sa*dx)*Y + (ca+(1-ca)*dz*dz      )*Z;
  dir = I3Direction(newX,newY,newZ);

}

//--------------------------------------------------------------
I3Position I3Calculator::InShowerSystem(const I3Particle& particle,
                                        const I3Position& pos)
{
  return InShowerSystem(particle.GetPos(), particle.GetDir(), pos);
}

//--------------------------------------------------------------
I3Position I3Calculator::InShowerSystem(const I3Position& core,
                                        const I3Direction& dir,
                                        const I3Position& pos)
{
  const double theta = dir.GetZenith();
  const double phi = dir.GetAzimuth();

  I3Position v(pos - core);
  v.RotateZ(-phi);
  v.RotateY(-theta);
  return v;
}
