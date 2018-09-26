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

#include "dataclasses/physics/I3Particle.h"

#include "tableio/I3TableRowDescription.h"
#include "tableio/I3TableRow.h"
#include "tableio/I3MemoryChunk.h"

TEST_GROUP(I3TableRowDescriptionTests);


TEST(simple_creation) {
    I3TableRowDescription desc;
    // define an int field with name "intval" unit "counts" and docstring "an int variable"
    desc.AddField<int>("intval","counts", "an int variable");

    ENSURE_EQUAL( desc.GetNumberOfFields() , static_cast<unsigned int>(1), "number of fields should be one");
    ENSURE_EQUAL( desc.GetTotalByteSize() , I3MEMORYCHUNK_SIZE, "total byte size should be sizeof(int)+padding");
    ENSURE_EQUAL( desc.GetTotalChunkSize() , static_cast<size_t>(1), "total chunk size should 1");
    ENSURE_EQUAL( desc.GetFieldColumn("intval"), static_cast<unsigned int>(0), "index of field should be zero");
    
    desc.AddField<double>("doubleval", "Volt", "a double variable");
    ENSURE_EQUAL( desc.GetNumberOfFields() , static_cast<unsigned int>(2), "number of fields should be one");
    if (sizeof(int)+sizeof(double) < I3MEMORYCHUNK_SIZE) {
        ENSURE_EQUAL( desc.GetTotalByteSize() , I3MEMORYCHUNK_SIZE, "total size should be sizeof(int)+padding+sizeof(double)");
    } else {
        ENSURE_EQUAL( desc.GetTotalByteSize() , 2*I3MEMORYCHUNK_SIZE, "total size should be sizeof(int)+padding+sizeof(double)");
    }
    ENSURE_EQUAL( desc.GetFieldColumn("intval"), static_cast<unsigned int>(0), "index lookup");
    ENSURE_EQUAL( desc.GetFieldColumn("doubleval"), static_cast<unsigned int>(1), "index lookup");
    ENSURE_EQUAL( desc.GetFieldNames().at(0), "intval", "fieldnames vector works");
    ENSURE_EQUAL( desc.GetFieldNames().at(1), "doubleval", "fieldnames vector works");
    
    ENSURE_EQUAL( (int)desc.GetFieldTypes().at(0).kind, (int)I3Datatype::Int, "Types vector works");
    ENSURE_EQUAL( desc.GetFieldTypes().at(0).size, sizeof(int), "Types vector works");
    ENSURE_EQUAL( desc.GetFieldTypes().at(0).is_signed, true, "Types vector works");
    
    ENSURE_EQUAL( (int)desc.GetFieldTypes().at(1).kind, (int)I3Datatype::Float, "Types vector works");
    ENSURE_EQUAL( desc.GetFieldTypes().at(1).size, sizeof(double), "Types vector works");
    ENSURE_EQUAL( desc.GetFieldTypes().at(1).is_signed, true, "Types vector works");
    
    ENSURE_EQUAL( desc.GetFieldTypeSizes().at(0), sizeof(int), "typeSizes vector works");
    ENSURE_EQUAL( desc.GetFieldTypeSizes().at(1), sizeof(double), "typeSizes vector works");
    ENSURE_EQUAL( desc.GetFieldArrayLengths().at(0), static_cast<size_t>(1), "fieldArrayLengths vector works");
    ENSURE_EQUAL( desc.GetFieldArrayLengths().at(1), static_cast<size_t>(1), "fieldArrayLengths vector works");
    ENSURE_EQUAL( desc.GetFieldByteOffsets().at(0), static_cast<size_t>(0), "fieldByteOffsets vector works");
    ENSURE_EQUAL( desc.GetFieldByteOffsets().at(1), 8u, "fieldByteOffsets vector works");
    // ENSURE_EQUAL( desc.GetFieldChunkOffsets().at(0), static_cast<size_t>(0), "fieldChunkOffsets vector works");
    // ENSURE_EQUAL( desc.GetFieldChunkOffsets().at(1), static_cast<size_t>(1), "fieldChunkOffsets vector works");
    ENSURE_EQUAL( desc.GetFieldDocStrings().at(0), "an int variable", "test docstring");
    ENSURE_EQUAL( desc.GetFieldDocStrings().at(1), "a double variable", "test docstring");
    ENSURE_EQUAL( desc.GetFieldUnits().at(0), "counts", "test unit string");
    ENSURE_EQUAL( desc.GetFieldUnits().at(1), "Volt", "test unit string");
}

TEST(comparison) {
   I3TableRowDescriptionPtr desc1(new I3TableRowDescription());
   I3TableRowDescriptionPtr desc2(new I3TableRowDescription());
   
   desc1->AddField<int>("intval","counts", "an int variable");
   desc2->AddField<int>("intval","counts", "an int variable");
   
   ENSURE_EQUAL( *desc1 == desc2, true, "Equal numbers of identical fields => equal");
   
   desc2->AddField<double>("doubleval","mV", "an int variable");
   
   ENSURE_EQUAL( *desc1 == desc2, false, "Unequal numbers of fields => unequal");
   
   desc2->AddField<double>("doubleval","NPE", "an int variable");
   
   ENSURE_EQUAL( *desc1 == desc2, false, "Fields with different units => unequal");
   
   I3TableRowDescriptionPtr desc3(new I3TableRowDescription());
   I3TableRowDescriptionPtr desc4(new I3TableRowDescription());
   
   desc3->AddField<int>("intval","counts", "FADC counts");
   desc4->AddField<int>("intval","counts", "FADC counts, multiplied by -1");
   
   ENSURE_EQUAL( *desc1 == desc2, false, "Fields with different docstrings => unequal");
}

TEST(padding) {
        I3TableRowDescription desc;
	desc.AddField<uint8_t>("byte", "unit", "doc");
	desc.AddField<uint32_t>("int", "unit", "doc");
	desc.AddField<uint16_t>("short", "unit", "doc");
	desc.AddField<double>("double", "unit", "doc");
	
	ENSURE_EQUAL( desc.GetFieldByteOffsets().at(1), 4u, "int is properly aligned");
	ENSURE_EQUAL( desc.GetFieldByteOffsets().at(2), 8u, "short is properly aligned");
	ENSURE_EQUAL( desc.GetFieldByteOffsets().at(3), 16u, "double is properly aligned");
}

TEST(array_creation) {
    I3TableRowDescription desc;
    desc.AddField<int32_t>("int10", "unit", "doc", 10);
    desc.AddField<double>("double256", "unit", "doc", 256);
    desc.AddField<int64_t>("long1", "unit", "doc");
    size_t bytesize = 10*sizeof(int32_t) + 256*sizeof(double) + sizeof(int64_t);
    size_t chunksize = (bytesize + I3MEMORYCHUNK_SIZE - 1) / I3MEMORYCHUNK_SIZE;
    ENSURE_EQUAL( desc.GetTotalChunkSize() , chunksize, "total chunk size");
    ENSURE_EQUAL( desc.GetNumberOfFields(), static_cast<unsigned int>(3), "number of fields is three");

    ENSURE_EQUAL( desc.GetFieldTypes().at(2).size, sizeof(int64_t), "hdfTypes vector works");
    ENSURE_EQUAL( desc.GetFieldTypeSizes().at(0), sizeof(int32_t), "typeSizes vector works");
    ENSURE_EQUAL( desc.GetFieldTypeSizes().at(1), sizeof(double), "typeSizes vector works");
    ENSURE_EQUAL( desc.GetFieldTypeSizes().at(2), sizeof(int64_t), "typeSizes vector works");
    ENSURE_EQUAL( desc.GetFieldByteOffsets().at(0), static_cast<size_t>(0), "fieldByteOffsets correct");
    ENSURE_EQUAL( desc.GetFieldByteOffsets().at(1), 10*sizeof(int32_t), "fieldByteOffsets correct");
    ENSURE_EQUAL( desc.GetFieldByteOffsets().at(2), (10*sizeof(int32_t) + 256*sizeof(double)), "fieldByteOffsets correct");
}


TEST(joining_descriptions) {
    I3TableRowDescription base_d;
    base_d.AddField<unsigned int>("Run", "unit", "doc");
    base_d.AddField<unsigned int>("Event", "unit", "doc");

    I3TableRowDescription omkey_d;
    omkey_d.AddField<int>("String", "unit", "doc");
    omkey_d.AddField<unsigned int>("OM", "unit", "doc");

    I3TableRowDescription position_d;
    position_d.AddField<double>("x", "m", "x vertex position");
    position_d.AddField<double>("y", "m", "y vertex position");
    position_d.AddField<double>("z", "m", "z vertex position");

    // use the | operator to join descriptions
    I3TableRowDescription map_omkey_pos_d = base_d | omkey_d | position_d;
    
    ENSURE_EQUAL( map_omkey_pos_d.GetNumberOfFields(), static_cast<unsigned int>(7), "number of fields");
    ENSURE_EQUAL( base_d.GetNumberOfFields(), static_cast<unsigned int>(2), "elements are unchanged");
    ENSURE_EQUAL( omkey_d.GetNumberOfFields(), static_cast<unsigned int>(2), "elements are unchanged");
    ENSURE_EQUAL( position_d.GetNumberOfFields(), static_cast<unsigned int>(3), "elements are unchanged");

    ENSURE_EQUAL( map_omkey_pos_d.GetFieldNames().at(0), "Run" );
    ENSURE_EQUAL( map_omkey_pos_d.GetFieldNames().at(1), "Event" );
    ENSURE_EQUAL( map_omkey_pos_d.GetFieldNames().at(2), "String" );
    ENSURE_EQUAL( map_omkey_pos_d.GetFieldNames().at(3), "OM" );
    ENSURE_EQUAL( map_omkey_pos_d.GetFieldNames().at(4), "x" );
    ENSURE_EQUAL( map_omkey_pos_d.GetFieldNames().at(5), "y" );
    ENSURE_EQUAL( map_omkey_pos_d.GetFieldNames().at(6), "z" );

    ENSURE_EQUAL( map_omkey_pos_d.GetFieldColumn("Run"),    static_cast<unsigned>(0));
    ENSURE_EQUAL( map_omkey_pos_d.GetFieldColumn("Event"),  static_cast<unsigned>(1));
    ENSURE_EQUAL( map_omkey_pos_d.GetFieldColumn("String"), static_cast<unsigned>(2));
    ENSURE_EQUAL( map_omkey_pos_d.GetFieldColumn("OM"),     static_cast<unsigned>(3));
    ENSURE_EQUAL( map_omkey_pos_d.GetFieldColumn("x"),      static_cast<unsigned>(4));
    ENSURE_EQUAL( map_omkey_pos_d.GetFieldColumn("y"),      static_cast<unsigned>(5));
    ENSURE_EQUAL( map_omkey_pos_d.GetFieldColumn("z"),      static_cast<unsigned>(6));

    size_t offsets[7] = {0, 4, 8, 12, 16, 24, 32};
    for (size_t i = 0; i < 7; ++i) {
        ENSURE_EQUAL( map_omkey_pos_d.GetFieldByteOffsets().at(i), offsets[i], "check byte offsets");
        // ENSURE_EQUAL( map_omkey_pos_d.GetFieldChunkOffsets().at(i), i, "check byte offsets");
    }

    size_t endpos = offsets[6] + sizeof(double);
    size_t chunksize = (endpos + I3MEMORYCHUNK_SIZE - 1)/I3MEMORYCHUNK_SIZE;
    ENSURE_EQUAL( map_omkey_pos_d.GetTotalChunkSize(), chunksize, "check total chunk size");
}

TEST(joining_alignment) {
        I3TableRowDescription foo;
	foo.AddField<uint16_t>("foo", "unit", "doc");
	
	I3TableRowDescription bar;
	bar.AddField<uint32_t>("bar", "unit", "doc");
	
	I3TableRowDescription baz = foo | bar;
	
	ENSURE_EQUAL( baz.GetTotalChunkSize(), 1u, "both fields fit in a chunk");
	ENSURE_EQUAL( baz.GetFieldByteOffsets().at(1), 4u, "int is properly aligned");
}

TEST(enum_description) {
    I3TableRowDescriptionPtr desc = I3TableRowDescriptionPtr(new I3TableRowDescription());
    desc->AddField<unsigned int>("Run", "", "doc");
    desc->AddField<unsigned int>("Event", "", "doc");

    #define ptype_fields (unknown)(Gamma)(EPlus)(EMinus)(MuPlus)(MuMinus)(Pi0)\
                         (PiPlus)(PiMinus)(K0_Long)(KPlus)(KMinus)(Neutron)\
                         (PPlus)(PMinus)(K0_Short)(NuE)(NuEBar)(NuMu)(NuMuBar)\
                         (TauPlus)(TauMinus)(NuTau)(NuTauBar)(He4Nucleus)\
                         (Li7Nucleus)(Be9Nucleus)(B11Nucleus)(C12Nucleus)\
                         (N14Nucleus)(O16Nucleus)(F19Nucleus)(Ne20Nucleus)\
                         (Na23Nucleus)(Mg24Nucleus)(Al27Nucleus)(Si28Nucleus)\
                         (P31Nucleus)(S32Nucleus)(Cl35Nucleus)(Ar40Nucleus)\
                         (K39Nucleus)(Ca40Nucleus)(Sc45Nucleus)(Ti48Nucleus)\
                         (V51Nucleus)(Cr52Nucleus)(Mn55Nucleus)(Fe56Nucleus)\
                         (CherenkovPhoton)(Nu)(Monopole)(Brems)(DeltaE)\
                         (PairProd)(NuclInt)(MuPair)(Hadrons)(FiberLaser)\
                         (N2Laser)(YAGLaser)(STauPlus)(STauMinus)
    MAKE_ENUM_VECTOR(ptype_vector,I3Particle,ParticleType,ptype_fields);
    desc->AddEnumField<I3Particle::ParticleType>("Type", ptype_vector, "", "doc");
    desc->AddField<double>("Value", "unit","doc");

    I3TableRow row = I3TableRow(desc);
    row.Set<unsigned int>("Run", 1);
    row.Set<unsigned int>("Event", 2);
    row.Set<I3Particle::ParticleType>("Type", I3Particle::NuE);
    row.Set<double>("Value", 3.14);
    ENSURE_EQUAL(row.Get<unsigned int>("Run"), (unsigned) 1, "read correctly");
    ENSURE_EQUAL(row.Get<unsigned int>("Event"), (unsigned) 2, "read correctly");
    ENSURE_EQUAL(row.Get<I3Particle::ParticleType>("Type"), I3Particle::NuE, "type stored correctly");
    ENSURE_DISTANCE(row.Get<double>("Value"), 3.14, 0.0000001, "read correctly");
    
    row.Set("Type", I3Particle::NuMu);
    ENSURE_EQUAL(row.Get<unsigned int>("Run"), (unsigned) 1, "read correctly");
    ENSURE_EQUAL(row.Get<unsigned int>("Event"), (unsigned) 2, "read correctly");
    ENSURE_EQUAL(row.Get<I3Particle::ParticleType>("Type"), I3Particle::NuMu, "type stored correctly");
    ENSURE_DISTANCE(row.Get<double>("Value"), 3.14, 0.0000001, "read correctly");


    /* RDMCParticleTypes
    #define ptype_fields (unknown)(Gamma)(EPlus)nn(EMinus)(Nu)(MuPlus)(MuMinus)\
                         (Pi0)(PiPlus)(PiMinus)(KPlus)(KMinus)(PPlus)(PMinus)\
                         (TauPlus)(TauMinus)(Monopole)(NuE)(NuMu)(NuTau)\
                         (NuEBar)(NuMuBar)(NuTauBar)(Brems)(DeltaE)(PairProd)\
                         (NuclInt)(MuPair)(Hadrons)(FiberLaser)(N2Laser)\
                         (YAGLaser)(ZPrimary)(APrimary)(CRProton)(CRHelium)\
                         (CROxygen)(CRSilicon)(CRIron)(Elph)
    */



}

