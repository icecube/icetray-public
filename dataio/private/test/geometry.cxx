/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$

    @version $Revision$
    @date $Date$

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

TEST_GROUP(geometry);

// (I3OMGeo)(I3TankGeo)

#define TEST_THESE (I3Geometry)

#define NON_I3FO_ITEMS (I3OMGeo)(I3TankGeo)

#define SERIALIZATION_TEST(r,data,t) SERIALIZE(t)

BOOST_PP_SEQ_FOR_EACH(SERIALIZATION_TEST, ~, TEST_THESE);
