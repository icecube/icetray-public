/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: common.cxx 165886 2018-10-01 14:37:58Z nwhitehorn $

    @version $Revision: 165886 $
    @date $Date: 2018-10-01 07:37:58 -0700 (Mon, 01 Oct 2018) $

*/

#include <I3Test.h>
#include <fstream>
#include <icetray/serialization.h>

#include <icetray/I3Logging.h>

#include <dataclasses/I3Direction.h>		
#include <dataclasses/I3Bool.h>
#include <dataclasses/I3Double.h>
#include <dataclasses/I3Position.h>
#include <dataclasses/I3Time.h>
#include <dataclasses/TriggerKey.h>
#include <dataclasses/OMKey.h>

#include "serialization-test.h"

#include <boost/preprocessor.hpp>

using namespace boost::archive;
using namespace std;

TEST_GROUP(common);

#define TEST_THESE \
  (I3Direction) \
  (I3Bool) \
  (I3Double) \
  (I3Position) \
  (I3Time) \
  (OMKey)

#define NON_I3FO_ITEMS \
  (TriggerKey)
  
#define SERIALIZATION_TEST(r,data,t) SERIALIZE(t)

BOOST_PP_SEQ_FOR_EACH(SERIALIZATION_TEST, ~, TEST_THESE);

