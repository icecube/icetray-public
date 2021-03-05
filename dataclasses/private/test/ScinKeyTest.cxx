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

#include "dataclasses/ScintKey.h"
#include <string>
using std::string;
using std::cout;
using std::endl;

TEST_GROUP(ScintKeyTest);


TEST(comparison_operator)
{
  ENSURE(ScintKey(10001,1) != ScintKey(10001,2),  "different keys are different");
  ENSURE(ScintKey(10001,1) != ScintKey(10002,1),  "different keys are different");

  ENSURE(ScintKey(10001,1) < ScintKey(10001,2), "operator< works as expected");
  ENSURE(ScintKey(10001,1) < ScintKey(10002,1), "operator< works as expected");
  
}

