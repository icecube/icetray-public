/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> Last changed by: $LastChangedBy$
 */

#include "I3Test.h"

#include "dataclasses/I3Double.h"
#include "icetray/I3Bool.h"


#include "tableio/I3TableService.h"
#include "tableio/I3TableWriter.h"
#include "tableio/I3Converter.h"

#include <boost/python/class.hpp>

TEST_GROUP(I3DoubleConverterTests);

// Use the Python-side registry to emulate the old DSO registry
template <typename T>
I3ConverterPtr
BuildConverter()
{
	namespace bp = boost::python;
	bp::object tableio(bp::handle<>(PyImport_Import(bp::str("icecube.tableio").ptr())));
	bp::object defaults(tableio.attr("I3ConverterRegistry").attr("defaults"));
	bp::object cls(bp::object(T()).attr("__class__"));
	if (bp::extract<bool>(defaults.attr("__contains__")(cls)))
		return bp::extract<I3ConverterPtr>(defaults[cls]());
	else
		throw std::runtime_error("No converter registered!");
}

TEST(type_id) {
    I3ConverterPtr converter = BuildConverter<I3Double>();
    boost::shared_ptr<const I3Double> pi(new I3Double(3.14));
    boost::shared_ptr<const I3Bool> falsehood(new I3Bool(false));
    
    ENSURE( converter->CanConvert(pi), "typeid works.");
    ENSURE( !converter->CanConvert(falsehood), "typeid works.");
    
}

TEST(convert_one_row) {
    I3ConverterPtr converter = BuildConverter<I3Double>();

    I3DoubleConstPtr x = I3DoubleConstPtr( new I3Double(3.14) );
    I3FrameObjectConstPtr frameobj = x; 

    // create empty row
    // use the I3Double in x to initialize the description
    ENSURE( (bool)converter->GetDescription(x), "get description");
    I3TableRowPtr rows = I3TableRowPtr(new I3TableRow(converter->GetDescription()));

    // inspect empty row
    ENSURE_EQUAL(rows->GetNumberOfRows(), static_cast<unsigned int>(1), "one row created");
    ENSURE_EQUAL(rows->GetDescription(), converter->GetDescription(), "description created and equal");
    ENSURE_EQUAL(rows->GetDescription()->GetNumberOfFields(), static_cast<unsigned int>(1), "description ok");
    ENSURE_EQUAL(rows->GetDescription()->GetFieldNames().at(0), "value", "description ok");
    ENSURE_EQUAL( rows->Get<double>("value"), 0, "rows are initialized to zero");
    
    // convert
    converter->Convert(frameobj, rows);
    
    // check
    ENSURE_DISTANCE( rows->Get<double>("value"), x->value, 1e-6, "I3Double value transferred to row");
}

TEST(convert_into_many_rows) {
    I3ConverterPtr converter = BuildConverter<I3Double>();

    I3DoublePtr x = I3DoublePtr( new I3Double(3.14) );
    I3FrameObjectConstPtr frameobj = x; 

    // create 10 empty rows
    // use the I3Double in x to initialize the description
    ENSURE( (bool)converter->GetDescription(x), "get description");
    I3TableRowPtr rows = I3TableRowPtr(new I3TableRow(converter->GetDescription(), 10));

    // inspect empty row
    ENSURE_EQUAL(rows->GetNumberOfRows(), static_cast<unsigned int>(10), "rows created");
    ENSURE_EQUAL(rows->GetDescription(), converter->GetDescription(), "description created and equal");
    ENSURE_EQUAL(rows->GetDescription()->GetNumberOfFields(), static_cast<unsigned int>(1), "description ok");
    ENSURE_EQUAL(rows->GetDescription()->GetFieldNames().at(0), "value", "description ok");
    for (int i = 0; i < 10; i++) {
        rows->SetCurrentRow(i);
        ENSURE_EQUAL( rows->Get<double>("value"), 0, "rows are initialized to zero");
        // convert
        x->value = 3.0 * i + 2;
        converter->Convert(frameobj, rows);
    }
    
    // check
    for (int i = 0; i < 10; i++) {
        rows->SetCurrentRow(i);
        ENSURE_DISTANCE( rows->Get<double>("value"), 3.0 * i + 2, 1e-6, "I3Double value transferred to row");

    }
}

