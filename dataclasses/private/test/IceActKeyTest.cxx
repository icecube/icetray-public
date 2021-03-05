/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: OMKeyTest.cxx 151490 2016-11-14 17:26:25Z kjmeagher $

    @version $Revision: 151490 $
    @date $Date: 2016-11-14 11:26:25 -0600 (Mon, 14 Nov 2016) $
    @author pretz

    @todo
*/

#include <I3Test.h>

#include "dataclasses/IceActKey.h"
#include <string>
using std::string;
using std::cout;
using std::endl;

TEST_GROUP(IceActKey);


TEST(comparison_operator)
{
  ENSURE(IceActKey(1,1) != IceActKey(1,2), "different keys are different");
  ENSURE(IceActKey(1,1) != IceActKey(2,1), "different keys are different");

  ENSURE(IceActKey(1,1) < IceActKey(1,2), "operator< works as expected");
  ENSURE(IceActKey(1,1) < IceActKey(2,1), "operator< works as expected");

  
}

