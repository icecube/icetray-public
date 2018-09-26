/**
 $Id$
 
 @version $Revision$
 @date $Date$
 */

#include <I3Test.h>

#include "dataclasses/I3Orientation.h"
#include "icetray/I3Units.h"
#include <string>
using std::string;
using std::cout;
using std::endl;
using namespace I3Units;

TEST_GROUP(I3OrientationTest); 


/**
 * check that constructor and destructor work
 */
TEST(const_dest)
{
	I3OrientationPtr ptr(new I3Orientation());
}


/**
 * Make sure that the internal rotation storage works correctly
 */
TEST(coord_change)
{
	cout <<"Creating I3Orientation..."<<endl;
	I3Orientation dir;
	cout <<"Setting dir: dir=(0,0,1), up=(1,0,0)..."<<endl;
	dir.SetOrientation(0.,0.,1., 1.,0.,0.);
	//ENSURE_DISTANCE(dir.GetZenith()/deg,125.264,0.001,"dir.GetZenith failed");
	//ENSURE_DISTANCE(dir.GetAzimuth()/deg,-135.0,0.001,"dir.GetAzimuth failed");
	ENSURE_DISTANCE(dir.GetX(),0.0,0.0001,"dir.GetX failed");
	ENSURE_DISTANCE(dir.GetY(),0.0,0.0001,"dir.GetY failed");
	ENSURE_DISTANCE(dir.GetZ(),1.0,0.0001,"dir.GetZ failed");

	cout <<"Setting dir: dir=(0,1,0), up=(1,0,0)..."<<endl;
	dir.SetOrientation(0.,1.,0., 1.,0.,0.);
	//ENSURE_DISTANCE(dir.GetZenith()/deg,125.264,0.001,"dir.GetZenith failed");
	//ENSURE_DISTANCE(dir.GetAzimuth()/deg,-135.0,0.001,"dir.GetAzimuth failed");
	ENSURE_DISTANCE(dir.GetX(),0.0,0.0001,"dir.GetX failed");
	ENSURE_DISTANCE(dir.GetY(),1.0,0.0001,"dir.GetY failed");
	ENSURE_DISTANCE(dir.GetZ(),0.0,0.0001,"dir.GetZ failed");

	cout <<"Setting dir: dir=(1,0,0), up=(0,1,0)..."<<endl;
	dir.SetOrientation(1.,0.,0., 0.,1.,0.);
	//ENSURE_DISTANCE(dir.GetZenith()/deg,125.264,0.001,"dir.GetZenith failed");
	//ENSURE_DISTANCE(dir.GetAzimuth()/deg,-135.0,0.001,"dir.GetAzimuth failed");
	ENSURE_DISTANCE(dir.GetX(),1.0,0.0001,"dir.GetX failed");
	ENSURE_DISTANCE(dir.GetY(),0.0,0.0001,"dir.GetY failed");
	ENSURE_DISTANCE(dir.GetZ(),0.0,0.0001,"dir.GetZ failed");

	cout <<"Setting dir: dir=(1,1,1), up=(1,-1,0)..."<<endl;
	dir.SetOrientation(1.,1.,1., -1.,1.,0.);
	//ENSURE_DISTANCE(dir.GetZenith()/deg,125.264,0.001,"dir.GetZenith failed");
	//ENSURE_DISTANCE(dir.GetAzimuth()/deg,-135.0,0.001,"dir.GetAzimuth failed");
	ENSURE_DISTANCE(dir.GetX(),1.0/sqrt(3.),0.0001,"dir.GetX failed");
	ENSURE_DISTANCE(dir.GetY(),1.0/sqrt(3.),0.0001,"dir.GetY failed");
	ENSURE_DISTANCE(dir.GetZ(),1.0/sqrt(3.),0.0001,"dir.GetZ failed");
	
	cout <<"Creating another I3Orientation..."<<endl;
	I3Orientation dir2;
	dir2.SetOrientation(1.0,0.0,1.0, 0.0,1.0,0.0);  // Not properly normalized
	//Recall Azimuth/Zenith are the direction back to sourcethat originates a particle
	//  with this direction.
	ENSURE_DISTANCE(dir2.GetZenith()/deg,(180.0-45.0),0.001,"dir.GetZenith failed");
	ENSURE_DISTANCE(dir2.GetAzimuth()/deg,180.0,0.001,"dir.GetAzimuth failed");
	ENSURE_DISTANCE(dir2.CalcDirTheta()/deg,(45.0),0.001,"dir.CalcTheta failed");
	ENSURE_DISTANCE(dir2.CalcDirPhi()/deg,0.0,0.001,"dir.CalcPhi failed");
	//Make sure the GetX, etc are properly renormalized.
	ENSURE_DISTANCE(dir2.GetX(),0.7071,0.0001,"dir.GetX failed");
	ENSURE_DISTANCE(dir2.GetY(),0.,0.0001,"dir.GetY failed");
	ENSURE_DISTANCE(dir2.GetZ(),0.7071,0.0001,"dir.GetZ failed");
	//Now change the direction, make sure everthing is updated properly
	dir2.SetOrientation(1.0,1.0,1.0, 0.0,1.0,-1.0);  // Not properly normalized
	ENSURE_DISTANCE(dir2.GetZenith()/deg,(180.0-54.736),0.001,"dir.GetZenith failed");
	ENSURE_DISTANCE(dir2.GetAzimuth()/deg,(225.0),0.001,"dir.GetAzimuth failed");
	ENSURE_DISTANCE(dir2.CalcDirTheta()/deg,(54.736),0.001,"dir.CalcTheta failed");
	ENSURE_DISTANCE(dir2.CalcDirPhi()/deg,45.0,0.001,"dir.GetPhi failed");
	//Make sure the GetX, etc are properly renormalized.
	ENSURE_DISTANCE(dir2.GetX(),0.57735,0.0001,"dir.GetX failed");
	ENSURE_DISTANCE(dir2.GetY(),0.57735,0.0001,"dir.GetY failed");
	ENSURE_DISTANCE(dir2.GetZ(),0.57735,0.0001,"dir.GetZ failed");
	
	cout <<"Setting dir: x=1,y=1,z=0..."<<endl;
	dir.SetOrientation(1,1,0, 0,0,1);
	ENSURE_DISTANCE(dir.GetZenith()/deg,90.0,0.001,"dir.GetZenith failed");
	ENSURE_DISTANCE(dir.GetAzimuth()/deg,225.0,0.001,"dir.GetAzimuth failed");
	ENSURE_DISTANCE(dir.GetX(),0.7071,0.001,"dir.X failed");
	ENSURE_DISTANCE(dir.GetY(),0.7071,0.001,"dir.Y failed");
	ENSURE_DISTANCE(dir.GetZ(),0.0,0.001,"dir.Z failed");
	
	cout <<"Setting values for dir in zen=0,azi=0 (i.e. down)..."<<endl;
	I3Direction ddir;
	ddir = I3Direction(0,0);
	dir.SetOrientation(ddir);
	ENSURE_DISTANCE(dir.GetX(),0.0,0.0001,"dir.GetX failed");
	ENSURE_DISTANCE(dir.GetY(),0.0,0.0001,"dir.GetY failed");
	ENSURE_DISTANCE(dir.GetZ(),-1.0,0.0001,"dir.GetZ failed");
	
	cout <<"Rotating dir around x-axis by +90deg... ==> +y-axis"<<endl;
	ddir.RotateX(90*deg);
	dir.SetOrientation(ddir);
	ENSURE_DISTANCE(dir.GetZenith()/deg,90.0,0.001,"dir.GetZenith failed");
	ENSURE_DISTANCE(dir.GetAzimuth()/deg,270.0,0.001,"dir.GetAzimuth failed");
	ENSURE_DISTANCE(dir.GetX(),0.0,0.0001,"dir.GetX failed");
	ENSURE_DISTANCE(dir.GetY(),1.0,0.0001,"dir.GetY failed");
	ENSURE_DISTANCE(dir.GetZ(),0.0,0.0001,"dir.GetZ failed");
	
	cout <<"Rotating dir around z-axis by -45deg...==> x=1,y=1,z=0"<<endl;
	ddir.RotateZ(-45*deg);
	dir.SetOrientation(ddir);
	ENSURE_DISTANCE(dir.GetZenith()/deg,90.0,0.001,"dir.GetZenith failed");
	ENSURE_DISTANCE(dir.GetAzimuth()/deg,225.0,0.001,"dir.GetAzimuth failed");
	ENSURE_DISTANCE(dir.GetX(),0.707107,0.0001,"dir.GetX failed");
	ENSURE_DISTANCE(dir.GetY(),0.707107,0.0001,"dir.GetY failed");
	ENSURE_DISTANCE(dir.GetZ(),0.0,0.0001,"dir.GetZ failed");
	
	cout <<"Rotating dir around y-axis by -90deg... ==> x=0,y=1,z=1"<<endl;
	ddir.RotateY(-90*deg);
	dir.SetOrientation(ddir);
	ENSURE_DISTANCE(dir.GetZenith()/deg,135.0,0.001,"dir.GetZenith failed");
	ENSURE_DISTANCE(dir.GetAzimuth()/deg,270.0,0.001,"dir.GetAzimuth failed");
	ENSURE_DISTANCE(dir.GetX(),0.0,0.0001,"dir.GetX failed");
	ENSURE_DISTANCE(dir.GetY(),0.707107,0.0001,"dir.GetY failed");
	ENSURE_DISTANCE(dir.GetZ(),0.707107,0.0001,"dir.GetZ failed");
}

TEST(setdir)
{
	I3Orientation d(0.,1.,0.,  1.,0.,0.);
	ENSURE_DISTANCE(d.GetX(),0,0.0001);
	ENSURE_DISTANCE(d.GetY(),1,0.0001);
	ENSURE_DISTANCE(d.GetZ(),0,0.0001);
	ENSURE_DISTANCE(d.GetUpX(),1,0.0001);
	ENSURE_DISTANCE(d.GetUpY(),0,0.0001);
	ENSURE_DISTANCE(d.GetUpZ(),0,0.0001);
	ENSURE_DISTANCE(d.GetRightX(),0,0.0001);
	ENSURE_DISTANCE(d.GetRightY(),0,0.0001);
	ENSURE_DISTANCE(d.GetRightZ(),-1,0.0001);
	ENSURE_DISTANCE(d.GetZenith(),90*deg,0.0001);
	ENSURE_DISTANCE(d.GetAzimuth(),(360-90)*deg,0.0001);
	
	d.SetOrientation(1,1,1,  -1,1,0);
	ENSURE_DISTANCE(d.GetZ(),1./sqrt(3.),0.0001);
	ENSURE_DISTANCE(d.GetY(),1./sqrt(3.),0.0001);
	ENSURE_DISTANCE(d.GetX(),1./sqrt(3.),0.0001);
	ENSURE_DISTANCE(d.GetUpX(),-1./sqrt(2.),0.0001);
	ENSURE_DISTANCE(d.GetUpY(), 1./sqrt(2.),0.0001);
	ENSURE_DISTANCE(d.GetUpZ(), 0./sqrt(2.),0.0001);
	ENSURE_DISTANCE(d.GetZenith(),2.18627,0.0001);
	ENSURE_DISTANCE(d.GetAzimuth(),225*deg,0.0001);
	
	d.SetOrientation(1,1,1,  -1,0,1);
	ENSURE_DISTANCE(d.GetX(),1./sqrt(3.),0.0001);
	ENSURE_DISTANCE(d.GetY(),1./sqrt(3.),0.0001);
	ENSURE_DISTANCE(d.GetZ(),1./sqrt(3.),0.0001);
	ENSURE_DISTANCE(d.GetUpX(),-1./sqrt(2.),0.0001);
	ENSURE_DISTANCE(d.GetUpY(), 0./sqrt(2.),0.0001);
	ENSURE_DISTANCE(d.GetUpZ(), 1./sqrt(2.),0.0001);
	ENSURE_DISTANCE(d.GetZenith(),2.18627,0.0001);
	ENSURE_DISTANCE(d.GetAzimuth(),(360-135)*deg,0.0001);

	I3Direction ddir = d.GetDir();
	ENSURE_DISTANCE(ddir.GetX(),1./sqrt(3.),0.0001);
	ENSURE_DISTANCE(ddir.GetY(),1./sqrt(3.),0.0001);
	ENSURE_DISTANCE(ddir.GetZ(),1./sqrt(3.),0.0001);
	ENSURE_DISTANCE(ddir.GetZenith(),2.18627,0.0001);
	ENSURE_DISTANCE(ddir.GetAzimuth(),(360-135)*deg,0.0001);
	
	d.SetOrientation(1,-1,-1,  1,0,1);
	ENSURE_DISTANCE(d.GetX(), 1./sqrt(3.),0.0001);
	ENSURE_DISTANCE(d.GetY(),-1./sqrt(3.),0.0001);
	ENSURE_DISTANCE(d.GetZ(),-1./sqrt(3.),0.0001);
	ENSURE_DISTANCE(d.GetUpX(), 1./sqrt(2.),0.0001);
	ENSURE_DISTANCE(d.GetUpY(), 0./sqrt(2.),0.0001);
	ENSURE_DISTANCE(d.GetUpZ(), 1./sqrt(2.),0.0001);
	
	// check the constructor from I3Direction
	ddir = I3Direction(1,1,1);
	I3Orientation o(ddir);
	ENSURE_DISTANCE(o.GetX(),1./sqrt(3.),0.0001);
	ENSURE_DISTANCE(o.GetY(),1./sqrt(3.),0.0001);
	ENSURE_DISTANCE(o.GetZ(),1./sqrt(3.),0.0001);
	ENSURE_DISTANCE(o.GetZenith(),2.18627,0.0001);
	ENSURE_DISTANCE(o.GetAzimuth(),(360-135)*deg,0.0001);

	// check the assignment operator from I3Direction
	ddir = I3Direction(1,1,1);
	d = ddir;
	ENSURE_DISTANCE(d.GetX(),1./sqrt(3.),0.0001);
	ENSURE_DISTANCE(d.GetY(),1./sqrt(3.),0.0001);
	ENSURE_DISTANCE(d.GetZ(),1./sqrt(3.),0.0001);
	ENSURE_DISTANCE(d.GetZenith(),2.18627,0.0001);
	ENSURE_DISTANCE(d.GetAzimuth(),(360-135)*deg,0.0001);
	
}

TEST(rotate)
{
	I3Direction dir(74*I3Units::degree, 22.5*I3Units::degree);
	I3Direction up(90*I3Units::degree, 292.5*I3Units::degree);
	I3Orientation orie(dir, up);
	ENSURE_DISTANCE(dir*orie.GetDir(), 1., 1e-12);
	ENSURE_DISTANCE(up*orie.GetUp(), 1., 1e-12);
	
	// make a vector from scalings of the I3Orientation's principal axes
	I3Position displacement = 1*orie.GetUp() + 1*orie.GetRight() + 1*orie.GetDir();
	// rotate into the orientation's coordinate system
	I3Position unit = orie.RotateIn(displacement);
	// in the orientation's coordinate system, the x,y,z coordinates should be the principal axis scalings
	ENSURE_DISTANCE(unit.GetX(), 1., 1e-12);
	ENSURE_DISTANCE(unit.GetY(), 1., 1e-12);
	ENSURE_DISTANCE(unit.GetZ(), 1., 1e-12);
	// rotate back and check
	I3Position back = orie.RotateOut(unit);
	ENSURE_DISTANCE(back.GetX(), displacement.GetX(), 1e-12);
	ENSURE_DISTANCE(back.GetY(), displacement.GetY(), 1e-12);
	ENSURE_DISTANCE(back.GetZ(), displacement.GetZ(), 1e-12);
}
