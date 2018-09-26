/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$

    @version $Revision$
    @date $Date$
    @author Troy D. Straszheim

    @todo

*/
#include <I3Test.h>
#include <cmath>
#include "dataclasses/physics/I3MCHit.h"
#include "dataclasses/physics/I3Particle.h"

// this is a typical minimal testsuite

// This string identifies the test suite in the list of test suites.
TEST_GROUP(I3MCHitTest);

/**
 * Checks assignment/copy construction
 */
// the string here is used to identify this specific test.
TEST(assignment_copy)
{
  I3MCHit h, j;
  ENSURE(h.GetNPE() == 1);
  ENSURE(std::isnan(h.GetCharge()));
  ENSURE(h.GetParticleMinorID()==-1);
  ENSURE(h.GetParticleMajorID()==0);

  I3Particle p;

  j.SetParticleID(p);
  j.SetNPE(3);
  j.SetCherenkovDistance(123.45);
  h = j;
  ENSURE_DISTANCE(0.1, 0.1, 0.0001,"ensure test");
  ENSURE_DISTANCE(j.GetNPE(), h.GetNPE(), (float)0.0001,"simple assignment");
  ENSURE_DISTANCE(j.GetCherenkovDistance(),h.GetCherenkovDistance(), 0.01, 
		  "CherenkovDistance test");
  ENSURE(h.GetParticleMajorID() == j.GetParticleMajorID());
  ENSURE(h.GetParticleMinorID() == j.GetParticleMinorID());
}

/**
 * checks chains of operations
 */
TEST(assignment_chain)
{
  I3MCHit u, v, w, x;
  x.SetTime(rand()/0.235234);
  u = u = v = v = w = x;
  ENSURE_DISTANCE(u.GetTime(), 
		  x.GetTime(), 
		  0.0001,"chain of assignment operators");
}


