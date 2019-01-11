/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: status.cxx 165886 2018-10-01 14:37:58Z nwhitehorn $

    @version $Revision: 165886 $
    @date $Date: 2018-10-01 07:37:58 -0700 (Mon, 01 Oct 2018) $

*/

#include <I3Test.h>
#include <fstream>
#include <icetray/serialization.h>

#include <icetray/I3Logging.h>

#include "dataclasses/status/I3DOMStatus.h"
#include "dataclasses/status/I3DetectorStatus.h"
#include "dataclasses/status/I3TriggerStatus.h"


#include "serialization-test.h"

#include <boost/preprocessor.hpp>

using namespace boost::archive;
using namespace std;

TEST_GROUP(status);

#define TEST_THESE (I3DetectorStatus)

#define NON_I3FO_ITEMS (I3DOMStatus)(I3TriggerStatus)

#define SERIALIZATION_TEST(r,data,t) SERIALIZE(t)

BOOST_PP_SEQ_FOR_EACH(SERIALIZATION_TEST, ~, TEST_THESE);

