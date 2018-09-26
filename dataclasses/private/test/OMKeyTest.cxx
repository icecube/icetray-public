/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$

    @version $Revision$
    @date $Date$
    @author pretz

    @todo
*/

#include <I3Test.h>

#include "icetray/OMKey.h"
#include <string>
using std::string;
using std::cout;
using std::endl;

TEST_GROUP(OMKeyTest);


TEST(comparison_operator)
{
  ENSURE(OMKey(1,1)   != OMKey(1,2),  "different keys are different");
  ENSURE(OMKey(1,1,0) != OMKey(1,2,0),"different keys are different");
  ENSURE(OMKey(1,1)   != OMKey(1,1,1),"different keys are different");
  ENSURE(OMKey(1,1,0) != OMKey(1,1,1),"different keys are different");
  ENSURE(OMKey(1,1,5) != OMKey(1,1,6),"different keys are different");
  ENSURE(OMKey(1,3)   != OMKey(2,3),  "different keys are different");
  ENSURE(OMKey(1,3,0) != OMKey(2,3,0),"different keys are different");
  ENSURE(OMKey(1,3,7) != OMKey(2,3,7),"different keys are different");
  ENSURE(OMKey(1,5,0) < OMKey(1,5,6), "operator< works as expected");
  ENSURE(OMKey(1,5)   < OMKey(1,5,6), "operator< works as expected");
  ENSURE(OMKey(1,5,3) < OMKey(1,5,6), "operator< works as expected");
  ENSURE(OMKey(1,0)   < OMKey(1,1),   "operator< works as expected");
  ENSURE(OMKey(1,0)   < OMKey(2,0),   "operator< works as expected");
  ENSURE(OMKey(1,0,0) < OMKey(2,0,0), "operator< works as expected");
  ENSURE(OMKey(1,0,6) < OMKey(2,0,8), "operator< works as expected");
  ENSURE(OMKey(1,1)   < OMKey(2,2),   "operator< works as expected");
  ENSURE(OMKey(1,1,0) < OMKey(2,2,0), "operator< works as expected");
  ENSURE(OMKey(1,1,1) < OMKey(2,2,2), "operator< works as expected");
  
}

