/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#include <I3Test.h>

#include "tableio/I3Converter.h"
#include "tableio/converter/dataclasses_map_converters.h"

#include "dataclasses/physics/I3DOMLaunch.h"
#include "dataclasses/geometry/I3Geometry.h"

typedef I3MapOMKeyVectorConverter<convert::I3DOMLaunch> I3DOMLaunchSeriesMapConverter;

TEST_GROUP(I3DOMLaunchConverterTests);

TEST(description) {
    I3ConverterPtr converter(new I3DOMLaunchSeriesMapConverter);
    
    I3DOMLaunchSeriesMapPtr dlsm(new I3DOMLaunchSeriesMap());
    ENSURE_EQUAL( converter->GetNumberOfRows(dlsm), size_t(0), "Nothing in the map");
    
    OMKey omk(35,35);
    (*dlsm)[omk] = std::vector<I3DOMLaunch>();
    ENSURE_EQUAL( converter->GetNumberOfRows(dlsm), size_t(0), "Still nothing in the map");
    
    I3DOMLaunch dl;
    (*dlsm)[omk].push_back(dl);
    ENSURE_EQUAL( converter->GetNumberOfRows(dlsm), size_t(1), "Now there's one element in the map");
    
    I3TableRowDescriptionConstPtr desc = converter->GetDescription(dlsm);
    ENSURE( desc != 0, "converter returned non-null pointer.");
    
}

TEST(type_id) {
        I3ConverterPtr converter(new I3DOMLaunchSeriesMapConverter);
    I3DOMLaunchSeriesMapPtr dlsm(new I3DOMLaunchSeriesMap());
    ENSURE( converter->CanConvert(dlsm), "Converter recognizes its own.");
}

TEST(assignment) {
    /* Explicitly construct a converter with geometry-booking on */
    I3ConverterPtr converter(new I3DOMLaunchSeriesMapConverter(true));
    
    I3DOMLaunchSeriesMapPtr dlsm(new I3DOMLaunchSeriesMap());
    I3FramePtr frame(new I3Frame()); /* A fake frame */
    I3GeometryPtr geo(new I3Geometry());
    I3OMGeo omgeo;

    OMKey omk(35,35);
    std::vector<I3DOMLaunch> dls;
    I3DOMLaunch dl;
    dl.SetStartTime(42.0);
    dl.SetTriggerType(I3DOMLaunch::ONBOARD_LED);
    dl.GetRawATWD(0).assign(128,42);
    dls.push_back(dl);
    (*dlsm)[omk] = dls;

    omgeo.position = I3Position(37., 42., 141.);
    geo->omgeo[omk] = omgeo;

    frame->Put("InIceRawData", dlsm);
    frame->Put("I3Geometry", geo);

    I3TableRowDescriptionConstPtr desc = converter->GetDescription(dlsm);
    I3TableRowPtr rows(new I3TableRow(desc,converter->GetNumberOfRows(dlsm)));
    converter->Convert(dlsm, rows, frame);
    
    ENSURE_EQUAL( rows->Get<double>("start_time"), 42.0);
    ENSURE_EQUAL( rows->Get<I3DOMLaunch::TriggerType>("trigger_type"), I3DOMLaunch::ONBOARD_LED);
    
    size_t index = desc->GetFieldColumn("raw_atwd_0");
    ENSURE_EQUAL( dl.GetRawATWD(0).size(), desc->GetFieldArrayLengths().at(index), "Array is the same length as the vector");
    
    std::vector<int>::const_iterator it;
    uint16_t* pointy = rows->GetPointer<uint16_t>("raw_atwd_0");
    size_t i;
    for (it = dl.GetRawATWD(0).begin(), i = 0; it != dl.GetRawATWD(0).end(); it++, i++) {
        ENSURE_EQUAL( pointy[i], *it, "Vector and array contents match.");
    }

    ENSURE_EQUAL( rows->Get<int32_t>("string"), 35);
    ENSURE_EQUAL( rows->Get<uint32_t>("om"), unsigned(35));
    ENSURE_EQUAL( rows->Get<double>("x"), 37., "OM position was booked properly.");
    ENSURE_EQUAL( rows->Get<double>("y"), 42., "OM position was booked properly.");
    ENSURE_EQUAL( rows->Get<double>("z"), 141., "OM position was booked properly.");

    /* Make sure it fails without the geometry */
    frame->Delete("I3Geometry");

    rows = I3TableRowPtr(new I3TableRow(desc,converter->GetNumberOfRows(dlsm)));
    int nrows = converter->Convert(dlsm, rows, frame);

    ENSURE_EQUAL( nrows, 0, "Converter fails if no geometry is present");

    /* Make sure it still works without geometry, but with geobooking off */
    converter = I3ConverterPtr(new I3DOMLaunchSeriesMapConverter);
    rows = I3TableRowPtr(new I3TableRow(desc,converter->GetNumberOfRows(dlsm)));

    converter->Convert(dlsm, rows, frame);

    ENSURE_EQUAL( rows->Get<int32_t>("string"), 35);
    ENSURE_EQUAL( rows->Get<uint32_t>("om"), unsigned(35));

    ENSURE_EQUAL( rows->Get<double>("start_time"), 42.0);
    ENSURE_EQUAL( rows->Get<I3DOMLaunch::TriggerType>("trigger_type"), I3DOMLaunch::ONBOARD_LED);

    index = desc->GetFieldColumn("raw_atwd_0");
    ENSURE_EQUAL( dl.GetRawATWD(0).size(), desc->GetFieldArrayLengths().at(index), "Array is the same length as the vector");

    pointy = rows->GetPointer<uint16_t>("raw_atwd_0");
    for (it = dl.GetRawATWD(0).begin(), i = 0; it != dl.GetRawATWD(0).end(); it++, i++) {
        ENSURE_EQUAL( pointy[i], *it, "Vector and array contents match.");
    }

}


