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

#include <I3Test.h>

#include <vector>

#include <tableio/I3TableRow.h>
#include <tableio/I3TableRowDescription.h>

TEST_GROUP(I3TableRowTests);

TEST(creation) {
    I3TableRowDescriptionPtr base_d = I3TableRowDescriptionPtr( new I3TableRowDescription());
    base_d->AddField<uint32_t>("Run", "", "doc");
    base_d->AddField<uint32_t>("Event", "", "doc");

    for(int i=0; i<10000; ++i) {
        I3TableRow row(base_d);
    }
}

TEST(assignment) {
    I3TableRowDescriptionPtr base_d = I3TableRowDescriptionPtr( new I3TableRowDescription());
    base_d->AddField<uint32_t>("Run", "", "doc");
    base_d->AddField<uint32_t>("Event", "", "doc");

    I3TableRow row(base_d);
    row.Set<uint32_t>("Run",  1);
    row.Set<uint32_t>("Event",  2);
    
    ENSURE_EQUAL(row.Get<uint32_t>("Run"), static_cast<uint32_t>(1), "reread values");
    ENSURE_EQUAL(row.Get<uint32_t>("Event"), static_cast<uint32_t>(2), "reread values");
}

TEST(type_checking) {
    I3TableRowDescriptionPtr desc = I3TableRowDescriptionPtr( new I3TableRowDescription());
    desc->AddField<int32_t>("field", "unit", "doc");
    I3TableRow row(desc);

    bool thrown = false;
    try { row.Set<double>("field", 1); } // use wrong type 
    catch(...) { thrown = true; }
    ENSURE_EQUAL(thrown, true, "the above operation should throw an exception");

    row.Set<int>("field", 2);
    thrown = false;
    try { row.Get<uint32_t>("field"); } // use wrong type 
    catch(...) { thrown = true; }
    ENSURE_EQUAL(thrown, true, "the above operation should throw an exception");
    
    try { row.Get<uint32_t>("field"); } // use wrong type 
    catch(...) { thrown = true; }
    ENSURE_EQUAL(thrown, true, "the above operation should throw an exception");
    
    // type checking works over the size of the field
    // and its representation properties (integer-ness, signedness)
    // so attempts to get a field of the same size but
    // different properties should fail
    thrown = false;
    try { row.Get<float>("field"); } // use wrong type
    catch(...) { thrown = true; }
    ENSURE_EQUAL(thrown, true, "type checking works!");
}

// this enum will be represented as a 32-bit signed integer
struct Foo {
enum DummyEnummy {
    foo = INT32_MIN ,bar = INT32_MAX
};
};
#define DUMMY_ENUMMY (foo)(bar)

TEST(enum_checking) {
    I3TableRowDescriptionPtr desc = I3TableRowDescriptionPtr( new I3TableRowDescription());
    desc->AddField<int32_t>("field", "unit", "doc");
    MAKE_ENUM_VECTOR(dummies,Foo,Foo::DummyEnummy,DUMMY_ENUMMY);
    desc->AddEnumField<Foo::DummyEnummy>("enum", dummies, "unit", "doc");
    
    I3TableRow row(desc);
    
    ENSURE_EQUAL( sizeof(Foo::DummyEnummy), sizeof(int32_t) );    
    EXPECT_THROW(row.Set<Foo::DummyEnummy>("field",Foo::foo), "Shouldn't be able to set int field with enum type");
    EXPECT_THROW(row.Set<int32_t>("enum",0), "Shouldn't be able to set enum field with int type");
    
    try {  
       row.Set<Foo::DummyEnummy>("enum",Foo::foo);
    } catch (...) { FAIL("Enum field passes"); }

}

TEST(bool_casting) {
   I3TableRowDescriptionPtr desc = I3TableRowDescriptionPtr( new I3TableRowDescription());
   desc->AddField<bool>("field", "", "doc");
   ENSURE_EQUAL( std::string("bool"), desc->GetFieldUnits().at(0), "Bool unit field is set automatically")
   bool thrown = false;      
   try { desc->AddField<bool>("field2","this_is_not_bool",""); } // use some other unit
   catch(...) { thrown = true; }
   ENSURE_EQUAL(thrown, true, "Can't AddField<bool>() with a unit that is not 'bool'.");

   I3TableRow row(desc);
   row.Set<bool>("field",true);
   ENSURE_EQUAL( true, row.Get<bool>("field"), "Bool values are set properly.");
   row.Set<bool>("field",1);
   ENSURE_EQUAL( true, row.Get<bool>("field"), "Integer values are set properly.");
   row.Set<bool>("field",false);
   ENSURE_EQUAL( false, row.Get<bool>("field"), "Bool values are set properly.");
   row.Set<bool>("field",0);
   ENSURE_EQUAL( false, row.Get<bool>("field"), "Integer values are set properly.");
   
   row.Set<bool>("field",-17);
   ENSURE_EQUAL( true, row.Get<bool>("field"), "Anything not false is true.");
   row.Set<bool>("field",123);
   ENSURE_EQUAL( true, row.Get<bool>("field"), "Anything not false is true.");
}

TEST(array_assignment) {
    I3TableRowDescriptionPtr base_d = I3TableRowDescriptionPtr( new I3TableRowDescription());
    base_d->AddField<uint32_t>("Run", "", "doc");
    base_d->AddField<uint32_t>("Event", "", "doc");
    base_d->AddField<double>("Waveform", "mV", "doc", 128);
    base_d->AddField<bool>("Saturated", "", "doc");

    I3TableRow row(base_d);
    row.Set<uint32_t>("Run", 1);
    row.Set<uint32_t>("Event", 2);
    double* wf = row.GetPointer<double>("Waveform");
    for (int i = 0; i < 128; ++i) {
        wf[i] = 3.0*i + 2 ;
    }
    wf = 0;
    row.Set<bool>("Saturated", true);
    
    // reread it
    ENSURE_EQUAL(row.Get<uint32_t>("Run"), static_cast<uint32_t>(1), "reread values");
    ENSURE_EQUAL(row.Get<uint32_t>("Event"), static_cast<uint32_t>(2), "reread values");

    wf = row.GetPointer<double>("Waveform");
    for (int i = 0; i < 128; ++i) {
        ENSURE_DISTANCE(wf[i], 3.0*i + 2, 0.0000001);
    }
    wf = 0;
    
    ENSURE_EQUAL(row.Get<bool>("Saturated"), true, "reread values");

}

TEST(copy_constructor) {
    I3TableRowDescriptionPtr base_d = I3TableRowDescriptionPtr( new I3TableRowDescription());
    base_d->AddField<uint32_t>("Run", "", "doc");
    base_d->AddField<uint32_t>("Event", "", "doc");
    base_d->AddField<double>("Waveform", "mV", "doc", 128);
    base_d->AddField<bool>("Saturated", "", "doc");

    I3TableRow row(base_d);
    row.Set<uint32_t>("Run", 1);
    row.Set<uint32_t>("Event", 2);
    double* wf1 = 0;
    double* wf2 = 0;
    wf1 = row.GetPointer<double>("Waveform");
    for (int i = 0; i < 128; ++i) {
        wf1[i] = 3.0*i + 2 ;
    }
    wf1 = 0;
    row.Set<bool>("Saturated", true);

    I3TableRow row2 (row);
    ENSURE_EQUAL( row.Get<uint32_t>("Run"), row2.Get<uint32_t>("Run"), "fields equal");
    ENSURE_EQUAL( row.Get<uint32_t>("Event"), row2.Get<uint32_t>("Event"), "fields equal");
    ENSURE_EQUAL( row.Get<bool>("Saturated"), row2.Get<bool>("Saturated"), "fields equal");
    wf1 = row.GetPointer<double>("Waveform");
    wf2 = row2.GetPointer<double>("Waveform");
    for (int i = 0; i < 128; ++i) {
        ENSURE_EQUAL(wf1[i], wf2[i], "array fields equal");
    }
}
