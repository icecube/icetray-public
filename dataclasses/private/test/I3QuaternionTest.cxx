/**
    copyright  (C) 2015
    the icecube collaboration
    $Id$

    @version $Revision$
    @date $Date$
    @author boersma
*/

#include <I3Test.h>

#include "dataclasses/I3Quaternion.h"
#include "dataclasses/I3Direction.h"
#include "dataclasses/I3Position.h"
#include "icetray/I3Units.h"
#include <string>
#include <iostream>
#include <cmath>

using std::string;
using std::cout;
using std::endl;
using std::sin;
using std::cos;
using std::sqrt;
using I3Units::degree;

// helper functions for quaternion tests
static void ENSURE_QUATERNION_DISTANCE(const I3Quaternion &q,
                                       double x, double y, double z, double w,
                                       double eps, string mesg){
    ENSURE_DISTANCE(q.getX(),x,eps,mesg+": X");
    ENSURE_DISTANCE(q.getY(),y,eps,mesg+": Y");
    ENSURE_DISTANCE(q.getZ(),z,eps,mesg+": Z");
    ENSURE_DISTANCE(q.getW(),w,eps,mesg+": W");
}
static void ENSURE_QUATERNION_DISTANCE(const I3Quaternion &q1,
                                       const I3Quaternion &q2,
                                       double eps, string mesg){
    ENSURE_QUATERNION_DISTANCE(
            q1, q2.getX(), q2.getY(), q2.getZ(), q2.getW(), eps, mesg);
}

// helper function for quaternion tests
static void ENSURE_QUATERNION_EQUAL(const I3Quaternion &q,
                                    double x, double y, double z, double w){
    ENSURE_EQUAL(q.getX(),x);
    ENSURE_EQUAL(q.getY(),y);
    ENSURE_EQUAL(q.getZ(),z);
    ENSURE_EQUAL(q.getW(),w);
}


TEST_GROUP(I3QuaternionTest);

/**
 * check that constructor and destructor work
 */
TEST(const_dest)
{
  I3QuaternionPtr ptr(new I3Quaternion());
}

// TODO: discuss whether default values should actually be NAN, like elsewhere in icetray.
TEST(default_construction)
{
    I3Quaternion q;
    ENSURE_QUATERNION_EQUAL(q,0,0,0,0);
}

TEST(construction_assignment_set)
{
    cout << "testing constructor" << endl;
    I3Quaternion q(1,2,3,4);
    ENSURE_QUATERNION_EQUAL(q,1,2,3,4);

    cout << "testing assignment operator" << endl;
    q=I3Quaternion(5,6,7,8);
    ENSURE_QUATERNION_EQUAL(q,5,6,7,8);

    cout << "testing set method" << endl;
    q.set(9,10,11,12);
    ENSURE_QUATERNION_EQUAL(q,9,10,11,12);

    cout << "testing copy constructor" << endl;
    I3Quaternion qq(q);
    ENSURE_QUATERNION_EQUAL(qq,9,10,11,12);
}

TEST(boolean_expressions)
{
    I3Quaternion q1(2,3,5,7);
    I3Quaternion q2(2,11,13,17);
    I3Quaternion q3=q1;

    cout << "testing == operator for UNEQUAL operands (should yield FALSE)" << endl;
    ENSURE((q1 == q2) == false); // this test would fail before 2015-05-15 (svn r132520)
    ENSURE((q2 == q1) == false); // this test would fail before 2015-05-15 (svn r132520)

    cout << "testing != operator for UNEQUAL operands (should yield TRUE)" << endl;
    ENSURE(q1 != q2);
    ENSURE(q2 != q1);

    cout << "testing == operator for EQUAL operands (should yield TRUE)" << endl;
    ENSURE(q1 == q3);
    ENSURE(q3 == q1);

    cout << "testing != operator for EQUAL operands (should yield FALSE)" << endl;
    ENSURE((q1 != q3) == false);
    ENSURE((q3 != q1) == false);
}

TEST(unary_operations)
{
    const I3Quaternion q(+2,+3,-5,7);
    double norm = 2*2 + 3*3 + 5*5 + 7*7;

    cout << "testing norm()" << endl;
    ENSURE_DISTANCE(q.norm(), norm, 1e-8);

    cout << "testing magnitude()" << endl;
    ENSURE_DISTANCE(q.magnitude(), sqrt(norm), 1e-8);

    cout << "testing conjugate()" << endl;
    I3Quaternion qc = q.conjugate();
    ENSURE_QUATERNION_DISTANCE(qc,-2,-3,+5,7,1e-12,"conjugate()");

    cout << "testing scale()" << endl;
    double s = -3.3;
    I3Quaternion qs = q.scale(s);
    ENSURE_QUATERNION_DISTANCE(qs,+2*s,3*s,-5*s,7*s,1e-12,"scale()");
    qs = s*q;
    ENSURE_QUATERNION_DISTANCE(qs,+2*s,3*s,-5*s,7*s,1e-12,"left multiply with scale factor");
    qs = q*s;
    ENSURE_QUATERNION_DISTANCE(qs,+2*s,3*s,-5*s,7*s,1e-12,"right multiply with scale factor");

    cout << "testing unit(): norm" << endl;
    I3Quaternion qu = q.unit();
    ENSURE_DISTANCE(qu.norm(),1.0,1e-8,"unit() norm");
    cout << "testing unit(): direction" << endl;
    qs = qu.scale(sqrt(norm));
    ENSURE_DISTANCE((q-qs).norm(),0.0,1e-8,"unit() direction");

    cout << "testing inverse()" << endl;
    I3Quaternion qinv = q.inverse();
    ENSURE_QUATERNION_DISTANCE(qinv,-2/norm,-3/norm,+5/norm,7/norm,1e-12,"inverse()");
}

TEST(binary_operations)
{
    I3Quaternion q1(2,3,5,7);
    I3Quaternion q2(11,13,17,19);
    I3Quaternion q;

    cout << "testing q1+q2" << endl;
    q = q1+q2;
    ENSURE_QUATERNION_DISTANCE(q, 13, 16, 22, 26, 1e-12,"q1+q2");

    cout << "testing q1-q2" << endl;
    q = q1-q2;
    ENSURE_QUATERNION_DISTANCE(q, -9, -10, -12, -12, 1e-12,"q1-q2");

    cout << "testing q1*q2" << endl;
    q = q1*q2;
    ENSURE_QUATERNION_DISTANCE(q, 101, 169, 207, -13, 1e-8,"q1*q2");

    cout << "testing q2*q1" << endl;
    q = q2*q1;
    ENSURE_QUATERNION_DISTANCE(q, 129, 127, 221, -13, 1e-8,"q2*q1");

    cout << "testing q1/q2" << endl;
    q = q1/q2;
    ENSURE_QUATERNION_DISTANCE(q, -25./940., -55./940., -17./940., 279./940., 1e-8,"q1/q2");

    cout << "testing q2/q1" << endl;
    q = q2/q1;
    ENSURE_QUATERNION_DISTANCE(q, 25./87., 55./87., 17./87., 279./87., 1e-8,"q2/q1");
}

TEST(infix_operations)
{
    // Since quaternion multiplication is not commutative,
    // it is not a priori clear whether the infix operator *=
    // multiplies from the left or from the right.
    I3Quaternion q1(2,3,5,7);
    I3Quaternion q2(11,13,17,19);
    I3Quaternion qbin, qinfix;

    cout << "testing infix operators: multiplication" << endl;
    qinfix = q1;
    qinfix *= q2; // what do we get: qinfix=q1*q2 or qinfix=q2*q1?
    qbin = q1*q2;
    // qbin = q2*q1; // this way it will fail
    ENSURE_QUATERNION_DISTANCE(qinfix,qbin,1e-10,"infix multiplication");

    cout << "testing infix operators: division" << endl;
    qinfix = q1;
    qinfix /= q2; // qinfix=q1/q2=q1*(1./q2) or qinfix=(1./q2)*q1?
    qbin = q1/q2;
    // qbin = (1./q2)*q1; // this way it will fail
    ENSURE_QUATERNION_DISTANCE(qinfix,qbin,1e-10,"infix division");

    cout << "testing infix operators: addition" << endl;
    qinfix = q1;
    qinfix += q2;
    qbin = q1+q2;
    ENSURE_QUATERNION_DISTANCE(qinfix,qbin,1e-10,"infix addition");

    cout << "testing infix operators: subtraction" << endl;
    qinfix = q1;
    qinfix -= q2;
    qbin = q1-q2;
    ENSURE_QUATERNION_DISTANCE(qinfix,qbin,1e-10,"infix subtraction");

    cout << "testing infix operators: scale multiply" << endl;
    double s = -4.44;
    qinfix = q1;
    qinfix *= s;
    qbin = s*q1;
    ENSURE_QUATERNION_DISTANCE(qinfix,qbin,1e-10,"infix scale left multiply");
    qbin = q1*s;
    ENSURE_QUATERNION_DISTANCE(qinfix,qbin,1e-10,"infix scale right multiply");

    cout << "testing infix operators: scale division" << endl;
    qinfix = q1;
    qinfix /= s;
    qbin = q1/s;
    ENSURE_QUATERNION_DISTANCE(qinfix,qbin,1e-10,"infix scale division");
}

TEST(rotate_I3Direction)
{
    // This tests that quaternion rotations are equivalent
    // to the around-axis-rotations built into I3Direction.
    // It is assumed that I3Directions are always correctly normalized,
    // so the dot product of two directions is only equal to 1 if
    // those directions are equal.

    double angle = 333.*degree; // arbitrary angle
    I3Quaternion qx(sin(angle/2),0,0,cos(angle/2));
    I3Quaternion qy(0,sin(angle/2),0,cos(angle/2));
    I3Quaternion qz(0,0,sin(angle/2),cos(angle/2));
    I3Direction input(13.*degree,133.*degree);

    cout << "testing rotation around X axis" << endl;
    I3Direction output = qx.rotate(input);
    input.RotateX(angle); // rotates in place
    ENSURE_DISTANCE(input*output,1.0,1e-12,"dir rotation around X");

    cout << "testing rotation around Y axis" << endl;
    output = qy.rotate(input);
    input.RotateY(angle); // rotates in place
    ENSURE_DISTANCE(input*output,1.0,1e-12,"dir rotation around Y");

    cout << "testing rotation around Z axis" << endl;
    output = qz.rotate(input);
    input.RotateZ(angle); // rotates in place
    ENSURE_DISTANCE(input*output,1.0,1e-12,"dir rotation around Z");
}

TEST(rotate_I3Position)
{
    // This tests that quaternion rotations are equivalent
    // to the around-axis-rotations built into I3Direction.
    // The test is a bit lazy, because we only check that
    // the dot product of two object is equal to the magnitude
    // squared of one of them. But if anything is wrongly
    // implemented (wrong direction and scale) then it would
    // be very unlikely that the dot product would *not* be wrong.

    double angle = 111.*degree; // arbitrary angle
    I3Quaternion qx(sin(angle/2),0,0,cos(angle/2));
    I3Quaternion qy(0,sin(angle/2),0,cos(angle/2));
    I3Quaternion qz(0,0,sin(angle/2),cos(angle/2));
    I3Position input(-3.,88.,12); // arbitrary position
    double m2 = input.Mag2();

    cout << "testing rotation around X axis" << endl;
    I3Position output = qx.rotate(input);
    input.RotateX(angle); // rotates in place
    ENSURE_DISTANCE(input*output,m2,m2*1e-8,"pos rotation around X");

    cout << "testing rotation around Y axis" << endl;
    output = qy.rotate(input);
    input.RotateY(angle); // rotates in place
    ENSURE_DISTANCE(input*output,m2,m2*1e-8,"pos rotation around Y");

    cout << "testing rotation around Z axis" << endl;
    output = qz.rotate(input);
    input.RotateZ(angle); // rotates in place
    ENSURE_DISTANCE(input*output,m2,m2*1e-8,"pos rotation around Z");
}

TEST(rotate_axes)
{

    double angle = 222.*degree; // arbitrary angle
    I3Quaternion qx(sin(angle/2),0,0,cos(angle/2));
    I3Quaternion qy(0,sin(angle/2),0,cos(angle/2));
    I3Quaternion qz(0,0,sin(angle/2),cos(angle/2));
    double x,y,z;

    cout << "testing axis rotation: X axis while rotating around X" << endl;
    qx.rotatedXAxis(x,y,z);
    ENSURE_DISTANCE(x,1.,1e-12);
    ENSURE_DISTANCE(y,0.,1e-12);
    ENSURE_DISTANCE(z,0.,1e-12);

    cout << "testing axis rotation: Y axis while rotating around X" << endl;
    qx.rotatedYAxis(x,y,z);
    ENSURE_DISTANCE(x,0.,1e-12);
    ENSURE_DISTANCE(y,cos(angle),1e-12);
    ENSURE_DISTANCE(z,sin(angle),1e-12);

    cout << "testing axis rotation: Z axis while rotating around X" << endl;
    qx.rotatedZAxis(x,y,z);
    ENSURE_DISTANCE(x,0.,1e-12);
    ENSURE_DISTANCE(y,-sin(angle),1e-12);
    ENSURE_DISTANCE(z,cos(angle),1e-12);

    cout << "testing axis rotation: X axis while rotating around Y" << endl;
    qy.rotatedXAxis(x,y,z);
    ENSURE_DISTANCE(x,cos(angle),1e-12);
    ENSURE_DISTANCE(y,0.,1e-12);
    ENSURE_DISTANCE(z,-sin(angle),1e-12);

    cout << "testing axis rotation: Y axis while rotating around Y" << endl;
    qy.rotatedYAxis(x,y,z);
    ENSURE_DISTANCE(x,0.,1e-12);
    ENSURE_DISTANCE(y,1.,1e-12);
    ENSURE_DISTANCE(z,0.,1e-12);

    cout << "testing axis rotation: Z axis while rotating around Y" << endl;
    qy.rotatedZAxis(x,y,z);
    ENSURE_DISTANCE(x,sin(angle),1e-12);
    ENSURE_DISTANCE(y,0.,1e-12);
    ENSURE_DISTANCE(z,cos(angle),1e-12);

    cout << "testing axis rotation: X axis while rotating around Z" << endl;
    qz.rotatedXAxis(x,y,z);
    ENSURE_DISTANCE(x,cos(angle),1e-12);
    ENSURE_DISTANCE(y,sin(angle),1e-12);
    ENSURE_DISTANCE(z,0.,1e-12);

    cout << "testing axis rotation: Y axis while rotating around Z" << endl;
    qz.rotatedYAxis(x,y,z);
    ENSURE_DISTANCE(x,-sin(angle),1e-12);
    ENSURE_DISTANCE(y,cos(angle),1e-12);
    ENSURE_DISTANCE(z,0.,1e-12);

    cout << "testing axis rotation: Z axis while rotating around Z" << endl;
    qz.rotatedZAxis(x,y,z);
    ENSURE_DISTANCE(x,0.,1e-12);
    ENSURE_DISTANCE(y,0.,1e-12);
    ENSURE_DISTANCE(z,1.,1e-12);

    cout << "testing axis of arbitrary rotation: position along axis should remain invariant" << endl;
    I3Quaternion qrnd(-5,-1,7,11);  // arbitrary
    I3Quaternion qrot = qrnd.unit();
    I3Position input(qrnd.getX(),qrnd.getY(),qrnd.getZ());
    I3Position output(qrot.rotate(input));
    ENSURE_DISTANCE(input.GetX(),output.GetX(),1e-10);
    ENSURE_DISTANCE(input.GetY(),output.GetY(),1e-10);
    ENSURE_DISTANCE(input.GetZ(),output.GetZ(),1e-10);
}


TEST(rotate_inplace)
{
    double v[3] = {40.,50.,60.};
    double a=40.,b=50.,c=60.;
    double xx,xy,xz;
    double yx,yy,yz;
    double zx,zy,zz;
    I3Quaternion qrnd(12.,-7,8,-4); // arbitrary quaternion
    I3Quaternion qrot = qrnd.unit();

    cout << "testing 3-vector rotation: array vs 3 refs" << endl;
    qrot.rot3VectorInPlace(v);
    qrot.rot3VectorInPlace(a,b,c);
    ENSURE_DISTANCE(a,v[0],1e-12);
    ENSURE_DISTANCE(b,v[1],1e-12);
    ENSURE_DISTANCE(c,v[2],1e-12);

    cout << "testing 3-vector rotation: check with rotation matrix" << endl;
    qrot.rotatedXAxis(xx,xy,xz);
    qrot.rotatedYAxis(yx,yy,yz);
    qrot.rotatedZAxis(zx,zy,zz);
    a=40.;
    b=50.;
    c=60.;
    ENSURE_DISTANCE(v[0],a*xx+b*yx+c*zx,1e-10);
    ENSURE_DISTANCE(v[1],a*xy+b*yy+c*zy,1e-10);
    ENSURE_DISTANCE(v[2],a*xz+b*yz+c*zz,1e-10);
}
