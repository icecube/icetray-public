/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: HasBool.cxx 7856 2005-05-20 17:43:47Z olivas $

    @version $Revision: 1.2 $
    @date $Date: 2005-05-20 19:43:47 +0200 (Fri, 20 May 2005) $

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

