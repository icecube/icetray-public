/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: geometry.cxx 165886 2018-10-01 14:37:58Z nwhitehorn $

    @version $Revision: 165886 $
    @date $Date: 2018-10-01 07:37:58 -0700 (Mon, 01 Oct 2018) $

*/

#include <I3Test.h>
#include <fstream>
#include <icetray/serialization.h>

#include <icetray/I3Logging.h>

#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/geometry/I3OMGeo.h"
#include "dataclasses/geometry/I3TankGeo.h"

#include "serialization-test.h"

#include <boost/preprocessor.hpp>

using namespace boost::archive;
using namespace std;

TEST_GROUP(geometry);

// (I3OMGeo)(I3TankGeo)


#define TEST_THESE (I3Geometry)

#define NON_I3FO_ITEMS (I3OMGeo)(I3TankGeo)

#define SERIALIZATION_TEST(r,data,t) SERIALIZE(t)

BOOST_PP_SEQ_FOR_EACH(SERIALIZATION_TEST, ~, TEST_THESE);

